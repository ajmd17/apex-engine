#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "texture_2D.h"

#include <memory>

namespace apex {

class Framebuffer {
public:
    Framebuffer(int width, int height);
    virtual ~Framebuffer();

    inline unsigned int GetId() const { return id; }

    virtual const std::shared_ptr<Texture> GetColorTexture() const = 0;
    virtual const std::shared_ptr<Texture> GetNormalTexture() const = 0;
    virtual const std::shared_ptr<Texture> GetPositionTexture() const = 0;
    virtual const std::shared_ptr<Texture> GetDepthTexture() const = 0;

    virtual void StoreColor() = 0;
    virtual void StoreDepth() = 0;

    virtual void Use() = 0;
    void End();

protected:
    unsigned int id;
    int width, height;
    bool is_created, is_uploaded;
};

} // namespace apex

#endif
