#ifndef TERRAIN_SHADER_H
#define TERRAIN_SHADER_H

#include "../rendering/shaders/lighting_shader.h"

namespace apex {
class TerrainShader : public LightingShader {
public:
    TerrainShader(const ShaderProperties &properties);
    virtual ~TerrainShader() = default;

    virtual void ApplyMaterial(const Material &mat) override;
};
} // namespace apex

#endif
