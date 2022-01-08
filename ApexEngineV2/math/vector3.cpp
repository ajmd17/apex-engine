#include "vector3.h"
#include "matrix_util.h"
#include "quaternion.h"

#include <cmath>

namespace apex {

Vector3::Vector3()
    : x(0.0f), 
      y(0.0f), 
      z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
    : x(x), 
      y(y), 
      z(z)
{
}

Vector3::Vector3(float xyz)
    : x(xyz), 
      y(xyz), 
      z(xyz)
{
}

Vector3::Vector3(const Vector3 &other)
    : x(other.x), 
      y(other.y), 
      z(other.z)
{
}

Vector3 &Vector3::operator=(const Vector3 &other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

float Vector3::operator[](size_t index) const
{
    switch (index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        throw std::out_of_range("out of range!");
    }
}

float &Vector3::operator[](size_t index)
{
    switch (index) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        throw std::out_of_range("out of range!");
    }
}

Vector3 Vector3::operator+(const Vector3 &other) const
{
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 &Vector3::operator+=(const Vector3 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3 &other) const
{
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 &Vector3::operator-=(const Vector3 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3 Vector3::operator*(const Vector3 &other) const
{
    return Vector3(x * other.x, y * other.y, z * other.z);
}

Vector3 &Vector3::operator*=(const Vector3 &other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
}

Vector3 Vector3::operator*(const Matrix3 &mat) const
{
    return Vector3(x * mat(0, 0) + y * mat(0, 1) + z * mat(0, 2),
        x * mat(1, 0) + y * mat(1, 1) + z * mat(1, 2),
        x * mat(2, 0) + y * mat(2, 1) + z * mat(2, 2));
}

Vector3 &Vector3::operator*=(const Matrix3 &mat)
{
    operator=(Vector3(x * mat(0, 0) + y * mat(0, 1) + z * mat(0, 2),
        x * mat(1, 0) + y * mat(1, 1) + z * mat(1, 2),
        x * mat(2, 0) + y * mat(2, 1) + z * mat(2, 2)));
    return *this;
}

Vector3 Vector3::operator*(const Matrix4 &mat) const
{
    return Vector3(x * mat(0, 0) + y * mat(0, 1) + z * mat(0, 2) + 1.0 * mat(0, 3),
        x * mat(1, 0) + y * mat(1, 1) + z * mat(1, 2) + 1.0 * mat(1, 3),
        x * mat(2, 0) + y * mat(2, 1) + z * mat(2, 2) + 1.0 * mat(2, 3));
}

Vector3 &Vector3::operator*=(const Matrix4 &mat)
{
    operator=(Vector3(x * mat(0, 0) + y * mat(0, 1) + z * mat(0, 2) + 1.0 * mat(0, 3),
        x * mat(1, 0) + y * mat(1, 1) + z * mat(1, 2) + 1.0 * mat(1, 3),
        x * mat(2, 0) + y * mat(2, 1) + z * mat(2, 2) + 1.0 * mat(2, 3)));
    return *this;
}

Vector3 Vector3::operator*(const Quaternion &quat) const
{
    Vector3 result;
    result.x = quat.w * quat.w * x + 2 * 
        quat.y * quat.w * z - 2 * 
        quat.z * quat.w * y + 
        quat.x * quat.x * x + 2 * 
        quat.y * quat.x * y + 2 * 
        quat.z * quat.x * z - 
        quat.z * quat.z * x - quat.y * quat.y * x;

    result.y = 2 * quat.x * quat.y * x + 
        quat.y * quat.y * y + 2 * 
        quat.z * quat.y * z + 2 * 
        quat.w * quat.z * x - 
        quat.z * quat.z * y + 
        quat.w * quat.w * y - 2 * 
        quat.x * quat.w * z - 
        quat.x * quat.x * y;

    result.z = 2 * quat.x * quat.z * x + 2 * 
        quat.y * quat.z * y + 
        quat.z * quat.z * z - 2 * 
        quat.w * quat.y * x - 
        quat.y * quat.y * z + 2 * 
        quat.w * quat.x * y - 
        quat.x * quat.x * z + 
        quat.w * quat.w * z;
    return result;
}

Vector3 &Vector3::operator*=(const Quaternion &quat)
{
    return operator=((*this) * quat);
}

Vector3 Vector3::operator/(const Vector3 &other) const
{
    return Vector3(x / other.x, y / other.y, z / other.z);
}

Vector3 &Vector3::operator/=(const Vector3 &other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
    return *this;
}

bool Vector3::operator==(const Vector3 &other) const
{
    return x == other.x && y == other.y && z == other.z;
}

bool Vector3::operator!=(const Vector3 &other) const
{
    return !((*this) == other);
}

float Vector3::Length() const
{
    return sqrt(LengthSquared());
}

float Vector3::LengthSquared() const
{
    return x * x + y * y + z * z;
}

float Vector3::DistanceSquared(const Vector3 &other) const
{
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return dx * dx + dy * dy + dz * dz;
}

float Vector3::Distance(const Vector3 &other) const
{
    return sqrt(DistanceSquared(other));
}

Vector3 &Vector3::Normalize()
{
    float len = Length();
    float len_sqr = len * len;
    if (len_sqr == 0 || len_sqr == 1) {
        return *this;
    }

    (*this) *= (1.0f / len);
    return *this;
}

Vector3 &Vector3::Cross(const Vector3 &other)
{
    float x1 = y * other.z - z * other.y;
    float y1 = z * other.x - x * other.z;
    float z1 = x * other.y - y * other.x;
    operator=(Vector3(x1, y1, z1));
    return *this;
}

Vector3 &Vector3::Rotate(const Vector3 &axis, float radians)
{
    Matrix4 rotation;
    MatrixUtil::ToRotation(rotation, axis, radians);
    (*this) *= rotation;
    return *this;
}

Vector3 &Vector3::Lerp(const Vector3 &to, const float amt)
{
    x = MathUtil::Lerp(x, to.x, amt);
    y = MathUtil::Lerp(y, to.y, amt);
    z = MathUtil::Lerp(z, to.z, amt);
    return *this;
}

float Vector3::Dot(const Vector3 &other) const
{
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::Abs(const Vector3 &vec)
{
    return Vector3(std::fabs(vec.x), std::fabs(vec.y), std::fabs(vec.z));
}

Vector3 Vector3::Round(const Vector3 &vec)
{
    return Vector3(std::round(vec.x), std::round(vec.y), std::round(vec.z));
}

Vector3 Vector3::Clamp(const Vector3 &vec, float min_value, float max_value)
{
    return Max(min_value, Min(vec, max_value));
}

Vector3 Vector3::Min(const Vector3 &a, const Vector3 &b)
{
    return Vector3(MathUtil::Min(a.x, b.x),
        MathUtil::Min(a.y, b.y),
        MathUtil::Min(a.z, b.z));
}

Vector3 Vector3::Max(const Vector3 &a, const Vector3 &b)
{
    return Vector3(MathUtil::Max(a.x, b.x),
        MathUtil::Max(a.y, b.y),
        MathUtil::Max(a.z, b.z));
}

Vector3 Vector3::Zero()
{
    return Vector3(0, 0, 0);
}

Vector3 Vector3::One()
{
    return Vector3(1, 1, 1);
}

Vector3 Vector3::UnitX()
{
    return Vector3(1, 0, 0);
}

Vector3 Vector3::UnitY()
{
    return Vector3(0, 1, 0);
}

Vector3 Vector3::UnitZ()
{
    return Vector3(0, 0, 1);
}

std::ostream &operator<<(std::ostream &out, const Vector3 &vec) // output
{
    out << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
    return out;
}

} // namespace apex
