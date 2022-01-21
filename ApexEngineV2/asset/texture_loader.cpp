#include "texture_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../util/img/stb_image.h"

#include "../rendering/texture_2D.h"
#include "../opengl.h"

#include <iostream>

namespace apex {
std::shared_ptr<Loadable> TextureLoader::LoadFromFile(const std::string &path)
{
    int width, height, comp;
    unsigned char *bytes = stbi_load(path.c_str(), &width, &height, &comp, 0);

    if (bytes == nullptr) {
        return nullptr;
    }

    auto tex = std::make_shared<Texture2D>(width, height, bytes);

    switch (comp) {
        case STBI_rgb_alpha:
            tex->SetFormat(GL_RGBA);
            tex->SetInternalFormat(GL_RGBA8);
            break;
        case STBI_rgb:
            tex->SetFormat(GL_RGB);
            tex->SetInternalFormat(GL_RGB8);
            break;
        // case STBI_grey_alpha:
        //     tex->SetFormat(GL_RG);
        //     tex->SetInternalFormat(GL_RG8);
        //     break;
        // case STBI_grey:
        //     tex->SetFormat(GL_RED);
        //     tex->SetInternalFormat(GL_R8);
        //     break;
        default:
            std::cout << "Unknown image format!" << std::endl;
            throw "Unknown image format";
    }

    /*tex->Use(); // uploads data

    stbi_image_free(bytes);
    tex->bytes = nullptr;

    tex->End();*/

    return tex;
}
} // namespace apex
