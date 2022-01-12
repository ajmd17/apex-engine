#ifndef POST_SHADER_H
#define POST_SHADER_H

#include "../shader.h"

namespace apex {
class PostShader : public Shader {
public:
    PostShader(const ShaderProperties &properties);
    virtual ~PostShader() = default;

    virtual void ApplyMaterial(const Material &mat);
    virtual void ApplyTransforms(const Transform &transform, Camera *camera) = 0;
};
} // namespace apex

#endif
