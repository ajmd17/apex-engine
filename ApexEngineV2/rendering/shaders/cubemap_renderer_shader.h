#ifndef CUBEMAP_RENDERER_SHADER_H
#define CUBEMAP_RENDERER_SHADER_H

#include <array>

#include "../shader.h"
#include "../../math/matrix4.h"

namespace apex {
class CubemapRendererShader : public Shader {
public:
    CubemapRendererShader(const ShaderProperties &properties);
    virtual ~CubemapRendererShader() = default;

    virtual void ApplyMaterial(const Material &mat);
    virtual void ApplyTransforms(const Transform &transform, Camera *camera);

private:
    std::array<Matrix4, 6> m_shadow_matrices;
};
} // namespace apex

#endif
