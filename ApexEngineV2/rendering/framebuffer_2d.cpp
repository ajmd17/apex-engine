#include "./framebuffer_2d.h"
#include "../gl_util.h"

namespace apex {

std::shared_ptr<Texture> Framebuffer2D::MakeTexture(
    Framebuffer::FramebufferAttachment attachment,
    int width,
    int height,
    unsigned char *bytes
)
{
    auto attributes = Framebuffer::default_texture_attributes[attachment];

    auto texture = std::make_shared<Texture2D>(width, height, bytes);
    texture->SetInternalFormat(attributes.internal_format);
    texture->SetFormat(attributes.format);
    texture->SetFilter(attributes.mag_filter, attributes.min_filter);
    texture->SetWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    return texture;
}

Framebuffer2D::Framebuffer2D(
    int width,
    int height,
    bool has_color_texture,
    bool has_depth_texture,
    bool has_normal_texture,
    bool has_position_texture,
    bool has_data_texture
) : Framebuffer(width, height)
{
    has_color_texture && (m_attachments[FRAMEBUFFER_ATTACHMENT_COLOR] = MakeTexture(FRAMEBUFFER_ATTACHMENT_COLOR, width, height));
    has_depth_texture && (m_attachments[FRAMEBUFFER_ATTACHMENT_DEPTH] = MakeTexture(FRAMEBUFFER_ATTACHMENT_DEPTH, width, height));
    has_normal_texture && (m_attachments[FRAMEBUFFER_ATTACHMENT_NORMALS] = MakeTexture(FRAMEBUFFER_ATTACHMENT_NORMALS, width, height));
    has_position_texture && (m_attachments[FRAMEBUFFER_ATTACHMENT_POSITIONS] = MakeTexture(FRAMEBUFFER_ATTACHMENT_POSITIONS, width, height));
    has_data_texture && (m_attachments[FRAMEBUFFER_ATTACHMENT_USERDATA] = MakeTexture(FRAMEBUFFER_ATTACHMENT_USERDATA, width, height));
}

Framebuffer2D::~Framebuffer2D()
{
    // deleted in parent destructor
}

void Framebuffer2D::Use()
{
    if (!is_created) {
        glGenFramebuffers(1, &id);
        CatchGLErrors("Failed to generate framebuffer.");

        is_created = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, id);
    CatchGLErrors("Failed to bind framebuffer.", false);

    glViewport(0, 0, width, height);

    if (!is_uploaded) {
        unsigned int draw_buffers[FRAMEBUFFER_ATTACHMENT_MAX - 1] = { GL_NONE }; // -1 for depth
        int draw_buffer_index = 0;

        for (int i = 0; i < sizeof(draw_buffers) / sizeof(draw_buffers[0]); i++) {
            if (m_attachments[i] == nullptr) {
                continue;
            }

            m_attachments[i]->Use();
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0 + i,
                GL_TEXTURE_2D,
                m_attachments[i]->GetId(),
                0
            );
            CatchGLErrors("Failed to attach color attachment to framebuffer.", false);
            m_attachments[i]->End();

            draw_buffers[draw_buffer_index++] = GL_COLOR_ATTACHMENT0 + i;
        }

        if (m_attachments[FRAMEBUFFER_ATTACHMENT_DEPTH] != nullptr) {
            m_attachments[FRAMEBUFFER_ATTACHMENT_DEPTH]->Use();
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D,
                m_attachments[FRAMEBUFFER_ATTACHMENT_DEPTH]->GetId(),
                0
            );
            CatchGLErrors("Failed to attach depth texture to framebuffer.", false);
            m_attachments[FRAMEBUFFER_ATTACHMENT_DEPTH]->End();
        }

        glDrawBuffers(draw_buffer_index, draw_buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Could not create framebuffer");
        }

        is_uploaded = true;
    }

}

void Framebuffer2D::Store(FramebufferAttachment attachment, std::shared_ptr<Texture> &texture)
{
    if (m_attachments[attachment] == nullptr) {
        return;
    }

    // What happens for depth tex?
    
    glReadBuffer(GL_COLOR_ATTACHMENT0 + int(attachment));
    CatchGLErrors("Failed to set read buffer");

    //m_attachments[attachment]->Use();
    //texture->Use();

    // TEMP
    /*if (!texture->id) {
        // use without creating data
        glGenTextures(1, &texture->id);
        CatchGLErrors("Failed to generate texture.", true);
    }

    glBindTexture(GL_TEXTURE_2D, texture->GetId());
    CatchGLErrors("Failed to use texture");

    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER, texture->GetMagFilter());
    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER, texture->GetMinFilter());
    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S, texture->GetWrapS());
    glTexParameteri(GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T, texture->GetWrapT());

    // glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, m_attachments[attachment]->GetFormat(), 0, 0, width, height, 0);
    CatchGLErrors("Failed to copy subimage");

    texture->is_uploaded = true;
    texture->is_created = true;*/

    texture->Use(false);
    texture->CopyData(m_attachments[attachment].get());
    texture->End();

    //m_attachments[attachment]->End();
}

} // namespace apex
