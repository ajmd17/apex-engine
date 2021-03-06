#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "shader.h"
#include "../math/bounding_box.h"

#include <memory>

namespace apex {

class CoreEngine;

class Renderable {
    friend class Renderer;
public:
    enum RenderBucket {
        RB_OPAQUE,
        RB_TRANSPARENT,
        RB_SKY,
        RB_PARTICLE,
        RB_SCREEN,
    };

    Renderable(RenderBucket bucket = RB_OPAQUE);
    virtual ~Renderable() = default;

    inline RenderBucket GetRenderBucket() const { return m_bucket; }
    inline void SetRenderBucket(RenderBucket bucket) { m_bucket = bucket; }
    inline std::shared_ptr<Shader> GetShader() { return m_shader; }
    inline void SetShader(const std::shared_ptr<Shader> &shader) { m_shader = shader; }
    inline const BoundingBox &GetAABB() const { return m_aabb; }

    virtual void Render() = 0;

protected:
    RenderBucket m_bucket;
    std::shared_ptr<Shader> m_shader;
    BoundingBox m_aabb;
};

} // namespace apex

#endif