#include "box_physics_shape.h"
#include "box_collision.h"
#include "sphere_physics_shape.h"
#include "plane_physics_shape.h"

#include <cassert>

namespace apex {
namespace physics {

BoxPhysicsShape::BoxPhysicsShape(const Vector3 &dimensions)
    : PhysicsShape(PhysicsShape_box),
      m_dimensions(dimensions)
{
}

BoxPhysicsShape::BoxPhysicsShape(const BoxPhysicsShape &other)
    : PhysicsShape(PhysicsShape_box),
      m_dimensions(other.m_dimensions)
{
}

BoundingBox BoxPhysicsShape::GetBoundingBox()
{
    BoundingBox bounds(m_dimensions * -0.5, m_dimensions * 0.5);

    BoundingBox bounding_box;

    for (const auto &corner : bounds.GetCorners()) {
        bounding_box.Extend(corner * m_transform);
    }

    return bounding_box;
}

bool BoxPhysicsShape::CollidesWith(BoxPhysicsShape *other, CollisionList &out)
{
    Vector3 to_center = other->GetAxis(3) - GetAxis(3);

    const float length = to_center.Length();

    if (std::isnan(length) || std::isinf(length)) {
        return false;
    }

    double penetration = MathUtil::MaxSafeValue<double>();
    unsigned int best = MathUtil::MaxSafeValue<unsigned int>();

    for (int i = 0; i < 3; i++) {
        if (!BoxCollision::TryAxis(*this, *other, GetAxis(i), to_center,
            i, penetration, best)) {
            return false;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (!BoxCollision::TryAxis(*this, *other, other->GetAxis(i), to_center,
            i + 3, penetration, best)) {
            return false;
        }
    }

    unsigned int best_single_axis = best;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            Vector3 axis = GetAxis(i);
            axis.Cross(other->GetAxis(j));

            unsigned int index = (i * 3 + j) + 6;

            if (!BoxCollision::TryAxis(*this, *other, axis, to_center,
                index, penetration, best)) {
                return false;
            }
        }
    }

    // check to make sure there was a result
    assert(MathUtil::SafeValue(best) != MathUtil::MaxSafeValue<decltype(best)>());

    CollisionInfo collision;

    if (best < 3) {
        BoxCollision::FillPointFaceBoxBox(*this, *other, to_center, collision, best, penetration);
        out.m_collisions.push_back(collision);
        return true;
    } else if (best < 6) {
        BoxCollision::FillPointFaceBoxBox(*other, *this, to_center * -1.0f, collision, best - 3, penetration);
        // reverse the normal because we checked the opposite order
        collision.m_contact_normal *= -1.0f;
        out.m_collisions.push_back(collision);
        return true;
    } else {
        best -= 6;
        unsigned int a_axis_index = best / 3;
        unsigned int b_axis_index = best % 3;

        Vector3 a_axis = GetAxis(a_axis_index);
        Vector3 b_axis = other->GetAxis(b_axis_index);

        Vector3 axis = a_axis;
        axis.Cross(b_axis);
        axis.Normalize();

        if (axis.Dot(to_center) > 0) {
            axis *= -1.0f;
        }

        Vector3 a_point_on_edge = m_dimensions * 0.5f;
        Vector3 b_point_on_edge = other->GetDimensions() * 0.5f;

        for (unsigned int i = 0; i < 3; i++) {
            if (i == a_axis_index) {
                a_point_on_edge[i] = 0;
            } else if (GetAxis(i).Dot(axis) > 0) {
                a_point_on_edge[i] *= -1;
            }

            if (i == b_axis_index) {
                b_point_on_edge[i] = 0;
            } else if (other->GetAxis(i).Dot(axis) < 0) {
                b_point_on_edge[i] *= -1;
            }
        }

        a_point_on_edge *= m_transform;
        b_point_on_edge *= other->GetTransform();

        Vector3 vertex = BoxCollision::ContactPoint(a_point_on_edge, a_axis, m_dimensions[a_axis_index] * 0.5f,
            b_point_on_edge, b_axis, other->GetDimensions()[b_axis_index] * 0.5f,
            best_single_axis > 2);

        collision.m_contact_point = vertex;
        collision.m_contact_normal = axis;
        collision.m_contact_penetration = penetration;
        out.m_collisions.push_back(collision);

        return true;
    }

    return false;
}

bool BoxPhysicsShape::CollidesWith(SpherePhysicsShape *sphere, CollisionList &out)
{
    Matrix4 transform = m_transform;
    Matrix4 inverse = transform;
    inverse.Invert();

    Vector3 center = sphere->GetAxis(3);
    Vector3 center_transformed = center * inverse;

    if (fabs(center_transformed.GetX()) - sphere->GetRadius() > (m_dimensions.GetX() * 0.5f) ||
        fabs(center_transformed.GetY()) - sphere->GetRadius() > (m_dimensions.GetY() * 0.5f) ||
        fabs(center_transformed.GetZ()) - sphere->GetRadius() > (m_dimensions.GetZ()* 0.5f)) {
        return false;
    }

    Vector3 closest = Vector3::Zero();

    double distance = center_transformed.GetX();
    if (distance > m_dimensions.GetX() * 0.5f) {
        distance = m_dimensions.GetX() * 0.5f;
    }
    if (distance < -(m_dimensions.GetX() * 0.5f)) {
        distance = -(m_dimensions.GetX() * 0.5f);
    }
    closest.SetX(distance);

    distance = center_transformed.GetY();
    if (distance > m_dimensions.GetY() * 0.5f) {
        distance = m_dimensions.GetY() * 0.5f;
    }
    if (distance < -(m_dimensions.GetY() * 0.5f)) {
        distance = -(m_dimensions.GetY() * 0.5f);
    }
    closest.SetY(distance);

    distance = center_transformed.GetZ();
    if (distance > m_dimensions.GetZ() * 0.5f) {
        distance = m_dimensions.GetZ() * 0.5f;
    }
    if (distance < -(m_dimensions.GetZ() * 0.5f)) {
        distance = -(m_dimensions.GetZ() * 0.5f);
    }
    closest.SetZ(distance);

    distance = (closest - center_transformed).LengthSquared();
    if (distance > sphere->GetRadius() * sphere->GetRadius()) {
        return false;
    }

    Vector3 closest_transformed = closest * m_transform;

    Vector3 contact_normal = closest_transformed - center;
    contact_normal.Normalize();

    CollisionInfo collision;
    collision.m_contact_point = closest_transformed;
    collision.m_contact_normal = contact_normal;
    collision.m_contact_penetration = sphere->GetRadius() - sqrt(distance);
    out.m_collisions.push_back(collision);

    return true;
}

bool BoxPhysicsShape::CollidesWith(PlanePhysicsShape *shape, CollisionList &out)
{
    // try a simple intersection test first.
    double proj_rad = BoxCollision::TransformToAxis(*this, shape->GetDirection());
    double dist = shape->GetDirection().Dot(GetAxis(3)) - proj_rad;
    if (dist > shape->GetOffset()) {
        return false;
    }

    static const double pts[8][3] = {
        { 1, 1, 1 }, { -1, 1, 1 }, { 1, -1, 1 }, { -1, -1, 1 },
        { 1, 1, -1 }, { -1, 1, -1 }, { 1, -1, -1 }, { -1, -1, -1 }
    };

    unsigned int num_collisions = 0;
    // check each vertex of the box
    for (int i = 0; i < 8; i++) {
        // calculate vertex position
        auto vertex = Vector3(pts[i][0], pts[i][1], pts[i][2]);
        vertex *= m_dimensions * 0.5f;
        vertex *= m_transform;

        double dist = vertex.Dot(shape->GetDirection());
        if (dist <= shape->GetOffset()) {
            CollisionInfo collision;
            collision.m_contact_normal = shape->GetDirection();
            collision.m_contact_penetration = shape->GetOffset() - dist;
            collision.m_contact_point = shape->GetDirection() *
                (dist - shape->GetOffset()) + vertex;

            num_collisions++;
            out.m_collisions.push_back(collision);
        }
    }

    return bool(num_collisions > 0);
}

} // namespace physics
} // namespace apex
