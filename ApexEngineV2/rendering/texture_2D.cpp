#include "texture_2D.h"
#include "../core_engine.h"
#include "../gl_util.h"
#include "../util.h"
#include <cassert>

namespace apex {

Texture2D::Texture2D()
    : Texture()
{
    is_uploaded = false;
    is_created = false;
}

Texture2D::Texture2D(int width, int height, unsigned char *bytes)
    : Texture(width, height, bytes)
{
    is_uploaded = false;
    is_created = false;
}

Texture2D::~Texture2D()
{
    if (is_created) {
        glDeleteTextures(1, &id);
    }
    is_uploaded = false;
    is_created = false;
}

void Texture2D::CopyData(Texture * const other)
{
    ex_assert(other != nullptr);
    ex_assert(width == other->GetWidth());
    ex_assert(height == other->GetHeight());
    ex_assert(ifmt == other->GetInternalFormat());
    ex_assert(fmt == other->GetFormat());

    glCopyTexImage2D(GL_TEXTURE_2D, 0, fmt, 0, 0, width, height, 0);

    CatchGLErrors("Failed to copy texture data", false);
}

void Texture2D::Use(bool upload_data)
{
    if (!is_created) {
        glGenTextures(1, &id);
        CatchGLErrors("Failed to generate texture.", true);

        is_created = true;
    }

    glBindTexture(GL_TEXTURE_2D, id);

    if (!is_uploaded) {
        glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER, mag_filter);
        glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_WRAP_T, wrap_t);

        if (upload_data) {
            glTexImage2D(GL_TEXTURE_2D, 0, ifmt,
                width, height, 0, fmt, GL_UNSIGNED_BYTE, bytes);

            CatchGLErrors("glTexImage2D failed.", false);

            if (min_filter == GL_LINEAR_MIPMAP_LINEAR ||
                min_filter == GL_LINEAR_MIPMAP_NEAREST ||
                min_filter == GL_NEAREST_MIPMAP_NEAREST) {
                glGenerateMipmap(GL_TEXTURE_2D);
                CatchGLErrors("Failed to generate Texture2D mipmaps.", false);
            }
        }

        is_uploaded = true;
    }
}

void Texture2D::End()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace apex
