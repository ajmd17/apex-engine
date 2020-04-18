#ifndef CAMERA_H
#define CAMERA_H

#include "../../math/vector3.h"
#include "../../math/matrix4.h"

namespace apex {
class Camera {
public:
    Camera(int width, int height, float near, float far);
    virtual ~Camera() = default;

    inline int GetWidth() const       { return m_width; }
    inline void SetWidth(int width)   { m_width = width; }
    inline int GetHeight() const      { return m_height; }
    inline void SetHeight(int height) { m_height = height; }
    inline float GetNear() const      { return m_near; }
    inline void SetNear(float near)   { m_near = near; }
    inline float GetFar() const       { return m_far; }
    inline void SetFar(float far)     { m_far = far; }

    inline const Vector3 &GetTranslation() const           { return m_translation; }
    virtual void SetTranslation(const Vector3 &translation);
    inline const Vector3 &GetDirection() const             { return m_direction; }
    inline void SetDirection(const Vector3 &direction)     { m_direction = direction; }
    inline const Vector3 &GetUpVector() const              { return m_up; }
    inline void SetUpVector(const Vector3 &up)             { m_up = up; }

    inline Vector3 GetTarget() const { return m_translation + m_direction; }

    inline const Matrix4 &GetViewMatrix() const { return m_view_mat; }
    inline void SetViewMatrix(const Matrix4 &view_mat) { m_view_mat = view_mat; }
    inline const Matrix4 &GetProjectionMatrix() const { return m_proj_mat; }
    inline void SetProjectionMatrix(const Matrix4 &proj_mat) { m_proj_mat = proj_mat; }

    inline Matrix4 GetViewProjectionMatrix() const { return m_view_mat * m_proj_mat; }

    void Rotate(const Vector3 &axis, float radians);
    void Update(double dt);

    virtual void UpdateLogic(double dt) = 0;
    virtual void UpdateMatrices() = 0;

protected:
    Vector3 m_translation, m_direction, m_up;
    Matrix4 m_view_mat, m_proj_mat;

    int m_width, m_height;
    float m_near, m_far;
};
}

#endif
