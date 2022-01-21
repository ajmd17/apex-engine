#include "./cubemap.h"
#include "../gl_util.h"
#include "../core_engine.h"
#include "../math/math_util.h"

#include <iostream>

namespace apex {

Cubemap::Cubemap(const std::array<std::shared_ptr<Texture2D>, 6> &textures)
    : Texture(TextureType::TEXTURE_TYPE_3D),
      m_textures(textures)
{
}

Cubemap::~Cubemap()
{
}

void Cubemap::Initialize()
{
    CoreEngine::GetInstance()->Enable(CoreEngine::GLEnums::TEXTURE_CUBE_MAP);

    CatchGLErrors("Failed to enable CoreEngine::GLEnums::TEXTURE_CUBE_MAP", false);

    Texture::Initialize();
}

void Cubemap::UploadGpuData()
{
    for (size_t i = 0; i < m_textures.size(); i++) {
        const auto &tex = m_textures[i];

        if (tex == nullptr) {
            throw std::runtime_error("Could not upload cubemap because texture #" + std::to_string(i + 1) + " was nullptr.");
        } else if (tex->GetBytes() == nullptr) {
            throw std::runtime_error("Could not upload cubemap because texture #" + std::to_string(i + 1) + " had no bytes set.");
        }

        CoreEngine::GetInstance()->TexImage2D(
            CoreEngine::GLEnums::TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            tex->GetInternalFormat(),
            tex->GetWidth(),
            tex->GetHeight(),
            0,
            tex->GetFormat(),
            CoreEngine::GLEnums::UNSIGNED_BYTE,
            tex->GetBytes()
        );            
    }

    CoreEngine::GetInstance()->TexParameteri(
        CoreEngine::GLEnums::TEXTURE_CUBE_MAP,
        CoreEngine::GLEnums::TEXTURE_MAG_FILTER,
        CoreEngine::GLEnums::LINEAR
    );

    CoreEngine::GetInstance()->TexParameteri(
        CoreEngine::GLEnums::TEXTURE_CUBE_MAP,
        CoreEngine::GLEnums::TEXTURE_MIN_FILTER,
        CoreEngine::GLEnums::LINEAR
    );

    CoreEngine::GetInstance()->TexParameteri(
        CoreEngine::GLEnums::TEXTURE_CUBE_MAP,
        CoreEngine::GLEnums::TEXTURE_WRAP_S,
        CoreEngine::GLEnums::CLAMP_TO_EDGE
    );

    CoreEngine::GetInstance()->TexParameteri(
        CoreEngine::GLEnums::TEXTURE_CUBE_MAP,
        CoreEngine::GLEnums::TEXTURE_WRAP_T,
        CoreEngine::GLEnums::CLAMP_TO_EDGE
    );

    CoreEngine::GetInstance()->TexParameteri(
        CoreEngine::GLEnums::TEXTURE_CUBE_MAP,
        CoreEngine::GLEnums::TEXTURE_WRAP_R,
        CoreEngine::GLEnums::CLAMP_TO_EDGE
    );
    // glTexParameteri(CoreEngine::GLEnums::TEXTURE_CUBE_MAP, CoreEngine::GLEnums::TEXTURE_BASE_LEVEL, 0);
    // glTexParameteri(CoreEngine::GLEnums::TEXTURE_CUBE_MAP, CoreEngine::GLEnums::TEXTURE_MAX_LEVEL, CUBEMAP_NUM_MIPMAPS);

    // glGenerateMipmap(CoreEngine::GLEnums::TEXTURE_CUBE_MAP);

    CatchGLErrors("Failed to upload cubemap");
}

void Cubemap::Use()
{
    CoreEngine::GetInstance()->BindTexture(CoreEngine::GLEnums::TEXTURE_CUBE_MAP, id);
}

void Cubemap::End()
{
    CoreEngine::GetInstance()->BindTexture(CoreEngine::GLEnums::TEXTURE_CUBE_MAP, 0);
}

} // namespace apex
