#ifndef POST_FILTER_H
#define POST_FILTER_H

#include <string>
#include <memory>

#include "../shaders/post_shader.h"
#include "../camera/camera.h"
#include "../framebuffer_2d.h"
#include "../material.h"
#include "../../util.h"

namespace apex {
class PostFilter {
public:
    PostFilter(
        const std::shared_ptr<PostShader> &shader,
        BitFlags_t modifies_attachments = Framebuffer::FRAMEBUFFER_ATTACHMENT_COLOR
    );
    virtual ~PostFilter() = default;

    virtual void SetUniforms(Camera *cam) = 0;

    inline BitFlags_t ModifiesAttachments() const { return m_modifies_attachments; }
    inline bool ModifiesAttachment(Framebuffer::FramebufferAttachment attachment) const
        { return m_modifies_attachments & BitFlags_t(attachment); }

    inline std::shared_ptr<PostShader> &GetShader() { return m_shader; }
    inline const std::shared_ptr<PostShader> &GetShader() const { return m_shader; }

    void Begin(Camera *cam, const Framebuffer::FramebufferAttachments_t &attachments);
    void End(Camera *cam, Framebuffer *fbo, Framebuffer::FramebufferAttachments_t &attachments, bool copy_textures = true);

protected:
    std::shared_ptr<PostShader> m_shader;
    Material m_material;

private:
    BitFlags_t m_modifies_attachments;
};
}; // namespace apex

#endif
