#include "cubemap_renderer_shader.h"
#include "../environment.h"
#include "../../asset/asset_manager.h"
#include "../../asset/text_loader.h"
#include "../../util/shader_preprocessor.h"
#include "../../math/matrix_util.h"

namespace apex {
CubemapRendererShader::CubemapRendererShader(const ShaderProperties &properties)
    : Shader(properties),
      m_directions({
        std::make_pair(Vector3(1, 0, 0), Vector3(0, -1, 0)),
        std::make_pair(Vector3(-1, 0, 0), Vector3(0, -1, 0)),
        std::make_pair(Vector3(0, 1, 0), Vector3(0, 0, 1)),
        std::make_pair(Vector3(0, -1, 0), Vector3(0, 0, -1)),
        std::make_pair(Vector3(0, 0, 1), Vector3(0, -1, 0)),
        std::make_pair(Vector3(0, 0, -1), Vector3(0, -1, 0))
      })
{
    const std::string vs_path("res/shaders/cubemap_renderer.vert");
    const std::string fs_path("res/shaders/cubemap_renderer.frag");
    const std::string gs_path("res/shaders/cubemap_renderer.geom");

    AddSubShader(
        Shader::SubShaderType::SUBSHADER_VERTEX,
        AssetManager::GetInstance()->LoadFromFile<TextLoader::LoadedText>(vs_path)->GetText(),
        properties,
        vs_path
    );

    AddSubShader(
        Shader::SubShaderType::SUBSHADER_FRAGMENT,
        AssetManager::GetInstance()->LoadFromFile<TextLoader::LoadedText>(fs_path)->GetText(),
        properties,
        fs_path
    );

    AddSubShader(
        Shader::SubShaderType::SUBSHADER_GEOMETRY,
        AssetManager::GetInstance()->LoadFromFile<TextLoader::LoadedText>(gs_path)->GetText(),
        properties,
        gs_path
    );
}

void CubemapRendererShader::ApplyMaterial(const Material &mat)
{
    Shader::ApplyMaterial(mat);

    SetUniform("u_diffuseColor", mat.diffuse_color);

    int texture_index = 1;

    for (auto it = mat.textures.begin(); it != mat.textures.end(); it++) {
        if (it->second == nullptr) {
            continue;
        }

        Texture::ActiveTexture(texture_index);
        it->second->Begin();
        SetUniform(it->first, texture_index);
        SetUniform(std::string("Has") + it->first, 1);

        texture_index++;
    }

    Environment::GetInstance()->GetSun().Bind(0, this);
}

void CubemapRendererShader::ApplyTransforms(const Transform &transform, Camera *camera)
{
    Shader::ApplyTransforms(transform, camera);

    SetUniform("u_camerapos", camera->GetTranslation());
}
} // namespace apex
