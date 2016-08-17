#include "clouds_shader.h"
#include "../environment.h"
#include "../../asset/asset_manager.h"
#include "../../asset/text_loader.h"
#include "../../util/shader_preprocessor.h"

namespace apex {
CloudsShader::CloudsShader(const ShaderProperties &properties)
    : Shader(properties)
{
    const std::string vs_path("res/shaders/clouds.vert");
    const std::string fs_path("res/shaders/clouds.frag");

    AddSubShader(SubShader(CoreEngine::VERTEX_SHADER,
        ShaderPreprocessor::ProcessShader(
            AssetManager::GetInstance()->LoadFromFile<TextLoader::LoadedText>(vs_path)->GetText(),
            properties, vs_path)
        ));

    AddSubShader(SubShader(CoreEngine::FRAGMENT_SHADER,
        ShaderPreprocessor::ProcessShader(
            AssetManager::GetInstance()->LoadFromFile<TextLoader::LoadedText>(fs_path)->GetText(),
            properties, fs_path)
        ));


    cloud_map = AssetManager::GetInstance()->LoadFromFile<Texture2D>("res\\textures\\clouds2.png");
    if (cloud_map == nullptr) {
        throw std::runtime_error("Could not load cloud map!");
    }

    _global_time = 0.0f;
    _cloud_color = Vector4(1.0);
}

void CloudsShader::ApplyMaterial(const Material &mat)
{
    CoreEngine::GetInstance()->ActiveTexture(0);
    SetUniform("m_CloudMap", 0);
    cloud_map->Use();

    SetUniform("m_GlobalTime", _global_time);
    SetUniform("m_CloudColor", _cloud_color);

    if (mat.HasParameter("BlendMode") && mat.GetParameter("BlendMode")[0] == 1) {
        CoreEngine::GetInstance()->Enable(CoreEngine::BLEND);
        CoreEngine::GetInstance()->BlendFunc(CoreEngine::SRC_ALPHA, CoreEngine::ONE_MINUS_SRC_ALPHA);
    }
}

void CloudsShader::ApplyTransforms(const Matrix4 &model, const Matrix4 &view, const Matrix4 &proj)
{
    Shader::ApplyTransforms(model, view, proj);
}

void CloudsShader::SetCloudColor(const Vector4 &cloud_color)
{
    _cloud_color = cloud_color;
}

void CloudsShader::SetGlobalTime(float global_time)
{
    _global_time = global_time;
}
}