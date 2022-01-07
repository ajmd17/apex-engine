#include "vector4.h"
#include "matrix_util.h"
#include "math_util.h"
#include "vector3.h"

namespace apex {

Vector4::Vector4()
    : x(0.0f), 
      y(0.0f), 
      z(0.0f), 
      w(0.0f)
{
}

Vector4::Vector4(float x, float y, float z, float w)
    : x(x), 
      y(y),
      z(z), 
      w(w)
{
}

Vector4::Vector4(float xyzw)
    : x(xyzw), 
      y(xyzw), 
      z(xyzw), 
      w(xyzw)
{
}

Vector4::Vector4(const Vector4 &other)
    : x(other.x), 
      y(other.y), 
      z(other.z), 
      w(other.w)
{
}

Vector4 &Vector4::operator=(const Vector4 &other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Vector4 Vector4::operator+(const Vector4 &other) const
{
    return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vector4 &Vector4::operator+=(const Vector4 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vector4 Vector4::operator-(const Vector4 &other) const
{
    return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vector4 &Vector4::operator-=(const Vector4 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vector4 Vector4::operator*(const Vector4 &other) const
{
    return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
}

Vector4 &Vector4::operator*=(const Vector4 &other)
{
    x *= other.x;
    y *= other.y;
    z *= other.z;
    w *= other.w;
    return *this;
}

Vector4 Vector4::operator*(const Matrix4 &mat) const
{
    return Vector4(x * mat(0, 0) + y * mat(0, 1) + z * mat(0, 2) + w * mat(0, 3),
        x * mat(1, 0) + y * mat(1, 1) + z * mat(1, 2) + w * mat(1, 3),
        x * mat(2, 0) + y * mat(2, 1) + z * mat(2, 2) + w * mat(2, 3),
        x * mat(3, 0) + y * mat(3, 1) + z * mat(3, 2) + w * mat(3, 3));
}

Vector4 &Vector4::operator*=(const Matrix4 &mat)
{
    operator=(Vector4(x * mat(0, 0) + y * mat(0, 1) + z * mat(0, 2) + w * mat(0, 3),
        x * mat(1, 0) + y * mat(1, 1) + z * mat(1, 2) + w * mat(1, 3),
        x * mat(2, 0) + y * mat(2, 1) + z * mat(2, 2) + w * mat(2, 3),
        x * mat(3, 0) + y * mat(3, 1) + z * mat(3, 2) + w * mat(3, 3)));
    return *this;
}

Vector4 Vector4::operator/(const Vector4 &other) const
{
    return Vector4(x / other.x, y / other.y, z / other.z, w / other.w);
}

Vector4 &Vector4::operator/=(const Vector4 &other)
{
    x /= other.x;
    y /= other.y;
    z /= other.z;
    w /= other.w;
    return *this;
}

bool Vector4::operator==(const Vector4 &other) const
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Vector4::operator!=(const Vector4 &other) const
{
    return !((*this) == other);
}

float Vector4::Length() const
{
    return sqrt(LengthSquared());
}

float Vector4::LengthSquared() const
{
    return x * x + y * y + z * z + w * w;
}

float Vector4::DistanceSquared(const Vector4 &other) const
{
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    float dw = w - other.w;
    return dx * dx + dy * dy + dz * dz + dw * dw;
}

float Vector4::Distance(const Vector4 &other) const
{
    return sqrt(DistanceSquared(other));
}

Vector4 &Vector4::Normalize()
{
    float len = Length();
    float len_sqr = len * len;
    if (len_sqr == 0 || len_sqr == 1) {
        return *this;
    }

    (*this) *= (1.0f / len);
    return *this;
}

Vector4 &Vector4::Rotate(const Vector3 &axis, float radians)
{
    Matrix4 rotation;
    MatrixUtil::ToRotation(rotation, axis, radians);
    (*this) *= rotation;
    return *this;
}

Vector4 &Vector4::Lerp(const Vector4 &to, const float amt)
{
    x = MathUtil::Lerp(x, to.x, amt);
    y = MathUtil::Lerp(y, to.y, amt);
    z = MathUtil::Lerp(z, to.z, amt);
    w = MathUtil::Lerp(w, to.w, amt);
    return *this;
}

float Vector4::Dot(const Vector4 &other) const
{
    return x * other.x + y * other.y + z * other.z + w * other.w;
}

Vector4 Vector4::Abs(const Vector4 &vec)
{
    return Vector4(vec.x, vec.y, vec.z, vec.w);
}

Vector4 Vector4::Round(const Vector4 &vec)
{
    return Vector4(std::round(vec.x), std::round(vec.y), std::round(vec.z), std::round(vec.w));
}

Vector4 Vector4::Clamp(const Vector4 &vec, float min_value, float max_value)
{
    return Max(min_value, Min(vec, max_value));
}

Vector4 Vector4::Min(const Vector4 &a, const Vector4 &b)
{
    return Vector4(MathUtil::Min(a.x, b.x),
        MathUtil::Min(a.y, b.y),
        MathUtil::Min(a.z, b.z),
        MathUtil::Min(a.w, b.w));
}

Vector4 Vector4::Max(const Vector4 &a, const Vector4 &b)
{
    return Vector4(MathUtil::Max(a.x, b.x),
        MathUtil::Max(a.y, b.y),
        MathUtil::Max(a.z, b.z),
        MathUtil::Max(a.w, b.w));
}

Vector4 Vector4::Zero()
{
    return Vector4(0, 0, 0, 0);
}

Vector4 Vector4::One()
{
    return Vector4(1, 1, 1, 1);
}

Vector4 Vector4::UnitX()
{
    return Vector4(1, 0, 0, 0);
}

Vector4 Vector4::UnitY()
{
    return Vector4(0, 1, 0, 0);
}

Vector4 Vector4::UnitZ()
{
    return Vector4(0, 0, 1, 0);
}

Vector4 Vector4::UnitW()
{
    return Vector4(0, 0, 0, 1);
}

std::ostream &operator<<(std::ostream &out, const Vector4 &vec) // output
{
    out << "[" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "]";
    return out;
}

} // namespace apex
