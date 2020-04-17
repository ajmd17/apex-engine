#include "mesh.h"
#include "../opengl.h"

namespace apex {

const Mesh::MeshAttribute Mesh::MeshAttribute::Positions = {0, 3, 0 };
const Mesh::MeshAttribute Mesh::MeshAttribute::Normals = { 0, 3, 1 };
const Mesh::MeshAttribute Mesh::MeshAttribute::TexCoords0 = { 0, 2, 2 };
const Mesh::MeshAttribute Mesh::MeshAttribute::TexCoords1 = { 0, 2, 3 };
const Mesh::MeshAttribute Mesh::MeshAttribute::Tangents = { 0, 3, 4 };
const Mesh::MeshAttribute Mesh::MeshAttribute::Bitangents = { 0, 3, 5 };
const Mesh::MeshAttribute Mesh::MeshAttribute::BoneWeights = { 0, 4, 6 };
const Mesh::MeshAttribute Mesh::MeshAttribute::BoneIndices = { 0, 4, 7 };

Mesh::Mesh()
{
    SetAttribute(ATTR_POSITIONS, MeshAttribute::Positions);
    SetPrimitiveType(PRIM_TRIANGLES);
    is_uploaded = false;
    is_created = false;
}

Mesh::~Mesh()
{
    if (is_created) {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }

    is_uploaded = false;
    is_created = false;
}

void Mesh::SetVertices(const std::vector<Vertex> &verts)
{
    vertices = verts;
    indices.clear();
    for (size_t i = 0; i < verts.size(); i++) {
        indices.push_back(static_cast<MeshIndex>(i));
    }

    // update the aabb
    m_aabb.Clear();
    for (Vertex &vertex : vertices) {
        m_aabb.Extend(vertex.GetPosition());
    }

    is_uploaded = false;
}

void Mesh::SetVertices(const std::vector<Vertex> &verts, const std::vector<MeshIndex> &ind)
{
    vertices = verts;
    indices = ind;

    // update the aabb
    m_aabb.Clear();
    for (Vertex &vertex : vertices) {
        m_aabb.Extend(vertex.GetPosition());
    }

    is_uploaded = false;
}

void Mesh::SetAttribute(MeshAttributeType type, const MeshAttribute &attribute)
{
    attribs[type] = attribute;
    is_uploaded = false;
}

std::vector<float> Mesh::CreateBuffer()
{
    unsigned int vert_size = 0, prev_size = 0, offset = 0;

    for (auto &&attr : attribs) {
        offset += prev_size;
        attr.second.offset = offset;
        prev_size = attr.second.size;
        vert_size += prev_size;
    }

    vertex_size = vert_size;

    std::vector<float> buffer(vert_size * vertices.size());

    auto pos_it = attribs.find(ATTR_POSITIONS);
    auto norm_it = attribs.find(ATTR_NORMALS);
    auto tc0_it = attribs.find(ATTR_TEXCOORDS0);
    auto tc1_it = attribs.find(ATTR_TEXCOORDS1);
    auto tan_it = attribs.find(ATTR_TANGENTS);
    auto bitan_it = attribs.find(ATTR_BITANGENTS);
    auto bonew_it = attribs.find(ATTR_BONEWEIGHTS);
    auto bonei_it = attribs.find(ATTR_BONEINDICES);

    for (size_t i = 0; i < vertices.size(); i++) {
        auto &vertex = vertices[i];

        if (pos_it != attribs.end()) {
            buffer[(i * vert_size) + pos_it->second.offset] = vertex.GetPosition().x;
            buffer[(i * vert_size) + pos_it->second.offset + 1] = vertex.GetPosition().y;
            buffer[(i * vert_size) + pos_it->second.offset + 2] = vertex.GetPosition().z;
        }
        if (norm_it != attribs.end()) {
            buffer[(i * vert_size) + norm_it->second.offset] = vertex.GetNormal().x;
            buffer[(i * vert_size) + norm_it->second.offset + 1] = vertex.GetNormal().y;
            buffer[(i * vert_size) + norm_it->second.offset + 2] = vertex.GetNormal().z;
        }
        if (tc0_it != attribs.end()) {
            buffer[(i * vert_size) + tc0_it->second.offset] = vertex.GetTexCoord0().x;
            buffer[(i * vert_size) + tc0_it->second.offset + 1] = vertex.GetTexCoord0().y;
        }
        if (tc1_it != attribs.end()) {
            buffer[(i * vert_size) + tc1_it->second.offset] = vertex.GetTexCoord1().x;
            buffer[(i * vert_size) + tc1_it->second.offset + 1] = vertex.GetTexCoord1().y;
        }
        if (tan_it != attribs.end()) {
            buffer[(i * vert_size) + tan_it->second.offset] = vertex.GetTangent().x;
            buffer[(i * vert_size) + tan_it->second.offset + 1] = vertex.GetTangent().y;
            buffer[(i * vert_size) + tan_it->second.offset + 2] = vertex.GetTangent().z;
        }
        if (bitan_it != attribs.end()) {
            buffer[(i * vert_size) + bitan_it->second.offset] = vertex.GetBitangent().x;
            buffer[(i * vert_size) + bitan_it->second.offset + 1] = vertex.GetBitangent().y;
            buffer[(i * vert_size) + bitan_it->second.offset + 2] = vertex.GetBitangent().z;
        }
        if (bonei_it != attribs.end()) {
            buffer[(i * vert_size) + bonei_it->second.offset] = static_cast<float>(vertex.GetBoneIndex(0));
            buffer[(i * vert_size) + bonei_it->second.offset + 1] = static_cast<float>(vertex.GetBoneIndex(1));
            buffer[(i * vert_size) + bonei_it->second.offset + 2] = static_cast<float>(vertex.GetBoneIndex(2));
            buffer[(i * vert_size) + bonei_it->second.offset + 3] = static_cast<float>(vertex.GetBoneIndex(3));
        }
        if (bonew_it != attribs.end()) {
            buffer[(i * vert_size) + bonew_it->second.offset] = vertex.GetBoneWeight(0);
            buffer[(i * vert_size) + bonew_it->second.offset + 1] = vertex.GetBoneWeight(1);
            buffer[(i * vert_size) + bonew_it->second.offset + 2] = vertex.GetBoneWeight(2);
            buffer[(i * vert_size) + bonew_it->second.offset + 3] = vertex.GetBoneWeight(3);
        }
    }

    return buffer;
}

void Mesh::CalculateTangents()
{
    Vertex *v[3];
    Vector2 uv[3];

    for (size_t i = 0; i < indices.size(); i += 3) {
        for (int j = 0; j < 3; j++) {
            v[j] = &vertices[indices[i + j]];
            uv[j] = v[j]->GetTexCoord0();
        }

        Vector3 edge1 = v[1]->GetPosition() - v[0]->GetPosition();
        Vector3 edge2 = v[2]->GetPosition() - v[0]->GetPosition();

        Vector2 edge1uv = uv[1] - uv[0];
        Vector2 edge2uv = uv[2] - uv[0];

        const float cp = edge1uv.x * edge2uv.y - edge1uv.y * edge2uv.x;

        if (cp != 0.0f) {
            const float mul = 1.0f / cp;

            Vector3 tangent;
            tangent.x = edge2uv.y * edge1.x - edge1uv.y * edge2.x;
            tangent.y = edge2uv.y * edge1.y - edge1uv.y * edge2.y;
            tangent.z = edge2uv.y * edge1.z - edge1uv.y * edge2.z;
            tangent *= mul;
            tangent.Normalize();

            Vector3 bitangent;

            bitangent.x = -edge2uv.x * edge1.x + edge1uv.x * edge2.x;
            bitangent.y = -edge2uv.x * edge1.y + edge1uv.x * edge2.y;
            bitangent.z = -edge2uv.x * edge1.z + edge1uv.x * edge2.z;
            bitangent *= mul;
            bitangent.Normalize();

            for (int j = 0; j < 3; j++) {
                v[j]->SetTangent(tangent);
                v[j]->SetBitangent(bitangent);
            }
        }
    }

    SetAttribute(ATTR_TANGENTS, MeshAttribute::Tangents);
    SetAttribute(ATTR_BITANGENTS, MeshAttribute::Bitangents);
}

void Mesh::Render()
{
    if (!is_created) {
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);
        is_created = true;
    }

    if (!is_uploaded) {
        std::vector<float> buffer = CreateBuffer();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), &buffer[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(MeshIndex), &indices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        is_uploaded = true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    for (auto &&attr : attribs) {
        glEnableVertexAttribArray(attr.second.index);
        glVertexAttribPointer(attr.second.index, attr.second.size, GL_FLOAT,
            false, vertex_size * sizeof(float), (void*)(attr.second.offset * sizeof(float)));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(primitive_type, indices.size(), GL_UNSIGNED_INT, 0);

    for (auto &&attr : attribs) {
        glDisableVertexAttribArray(attr.second.index);
    }

    // Unbind the buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace apex
