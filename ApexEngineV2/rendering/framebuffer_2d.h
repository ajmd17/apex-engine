#ifndef FRAMEBUFFER_2D_H
#define FRAMEBUFFER_2D_H

#include "./framebuffer.h"
#include "./texture_2D.h"

#include <memory>
#include <array>

namespace apex {

class Framebuffer2D : public Framebuffer {
public:
    Framebuffer2D(
        int width,
        int height,
        bool has_color_texture = true,
        bool has_depth_texture = true,
        bool has_normal_texture = true,
        bool has_position_texture = true,
        bool has_data_texture = false // { material.shininess, material.roughness, _, perform deferred lighting? }
    );
    virtual ~Framebuffer2D();

    virtual void Store(FramebufferAttachment attachment, std::shared_ptr<Texture> &texture) override;

    virtual void Use() override;

    static std::shared_ptr<Texture> MakeTexture(
        Framebuffer::FramebufferAttachment attachment,
        int width,
        int height,
        unsigned char *bytes = nullptr
    );
};

} // namespace apex

#endif
