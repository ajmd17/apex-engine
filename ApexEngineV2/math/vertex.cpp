#include "vertex.h"

namespace apex {
Vertex &Vertex::operator=(const Vertex &other)
{
    position = other.position;
    normal = other.normal;
    texcoord0 = other.texcoord0;
    texcoord1 = other.texcoord1;
    tangent = other.tangent;
    bitangent = other.bitangent;
    nboneindices = other.nboneindices;
    nboneweights = other.nboneweights;
    bone_weights = other.bone_weights;
    bone_indices = other.bone_indices;

    return *this;
}

Vertex Vertex::operator*(float scalar) const
{
    Vertex other(*this);
    other.SetPosition(GetPosition() * scalar);

    return other;
}

Vertex Vertex::operator*(const Matrix4 &mat) const
{
    Vertex other(*this);
    other.SetPosition(GetPosition() * mat);

    return other;
}

Vertex Vertex::operator*(const Transform &transform) const
{
    return operator*(transform.GetMatrix());
}

Vertex &Vertex::operator*=(float scalar)
{
    return *this = operator*(scalar);
}

Vertex &Vertex::operator*=(const Matrix4 &mat)
{
    return *this = operator*(mat);
}

Vertex &Vertex::operator*=(const Transform &transform)
{
    return *this = operator*(transform);
}
} // namespace apex
