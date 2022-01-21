#include "texture_2D.h"
#include "../core_engine.h"
#include "../gl_util.h"
#include <cassert>

namespace apex {

Texture2D::Texture2D()
    : Texture(TextureType::TEXTURE_TYPE_2D)
{
}

Texture2D::Texture2D(int width, int height, unsigned char *bytes)
    : Texture(TextureType::TEXTURE_TYPE_2D, width, height, bytes)
{
}

Texture2D::~Texture2D()
{
}

void Texture2D::UploadGpuData()
{
    CoreEngine::GetInstance()->TexParameteri(CoreEngine::GLEnums::TEXTURE_2D,
        CoreEngine::GLEnums::TEXTURE_MAG_FILTER, mag_filter);
    CoreEngine::GetInstance()->TexParameteri(CoreEngine::GLEnums::TEXTURE_2D,
        CoreEngine::GLEnums::TEXTURE_MIN_FILTER, min_filter);
    CoreEngine::GetInstance()->TexParameteri(CoreEngine::GLEnums::TEXTURE_2D,
        CoreEngine::GLEnums::TEXTURE_WRAP_S, wrap_s);
    CoreEngine::GetInstance()->TexParameteri(CoreEngine::GLEnums::TEXTURE_2D,
        CoreEngine::GLEnums::TEXTURE_WRAP_T, wrap_t);

    CoreEngine::GetInstance()->TexImage2D(CoreEngine::GLEnums::TEXTURE_2D, 0, ifmt,
        width, height, 0, fmt, CoreEngine::GLEnums::UNSIGNED_BYTE, bytes);

    CatchGLErrors("glTexImage2D failed.", false);

    if (min_filter == CoreEngine::GLEnums::LINEAR_MIPMAP_LINEAR ||
        min_filter == CoreEngine::GLEnums::LINEAR_MIPMAP_NEAREST ||
        min_filter == CoreEngine::GLEnums::NEAREST_MIPMAP_NEAREST) {
        CoreEngine::GetInstance()->GenerateMipmap(CoreEngine::GLEnums::TEXTURE_2D);

        CatchGLErrors("Failed to generate Texture2D mipmaps.", false);
    }
}

void Texture2D::Use()
{
    CoreEngine::GetInstance()->BindTexture(CoreEngine::GLEnums::TEXTURE_2D, id);
}

void Texture2D::End()
{
    CoreEngine::GetInstance()->BindTexture(CoreEngine::GLEnums::TEXTURE_2D, 0);
}

} // namespace apex
