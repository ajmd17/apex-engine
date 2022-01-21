#include "framebuffer.h"
#include "../core_engine.h"

#include <iostream>

namespace apex {

Framebuffer::Framebuffer(int width, int height)
    : width(width),
      height(height)
{
    is_uploaded = false;
    is_created = false;
}

Framebuffer::~Framebuffer()
{
    if (is_created) {
        CoreEngine::GetInstance()->DeleteFramebuffers(1, &id);
    }
    is_uploaded = false;
    is_created = false;
}

void Framebuffer::End()
{
    CoreEngine::GetInstance()->BindFramebuffer(CoreEngine::GLEnums::FRAMEBUFFER, 0);
}

} // namespace apex
