#include "gamma_correct.h"
#include "../../../asset/asset_manager.h"
#include "../../../asset/text_loader.h"
#include "../../../util/shader_preprocessor.h"

namespace apex {
GammaCorrectShader::GammaCorrectShader(const ShaderProperties &properties)
    : PostShader(properties)
{
    const std::string fs_path("res/shaders/filters/gammacorrect.frag");

    AddSubShader(SubShader(GL_FRAGMENT_SHADER,
        ShaderPreprocessor::ProcessShader(
            AssetManager::GetInstance()->LoadFromFile<TextLoader::LoadedText>(fs_path)->GetText(),
            properties, fs_path
        )
    ));
}

void GammaCorrectShader::ApplyTransforms(const Matrix4 &transform, Camera *camera)
{
    Shader::ApplyTransforms(transform, camera);
}
} // namespace apex
