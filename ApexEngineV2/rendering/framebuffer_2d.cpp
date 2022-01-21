#include "./framebuffer_2d.h"
#include "../core_engine.h"
#include "../gl_util.h"

namespace apex {

Framebuffer2D::Framebuffer2D(
    int width,
    int height,
    bool has_color_texture,
    bool has_depth_texture,
    bool has_normal_texture,
    bool has_position_texture,
    bool has_data_texture
)
    : Framebuffer(width, height),
      m_has_color_texture(has_color_texture),
      m_has_depth_texture(has_depth_texture),
      m_has_normal_texture(has_normal_texture),
      m_has_position_texture(has_position_texture),
      m_has_data_texture(has_data_texture)
{
    if (m_has_color_texture) {
        m_color_texture = std::make_shared<Texture2D>(width, height, (unsigned char*)nullptr);
        m_color_texture->SetInternalFormat(CoreEngine::GLEnums::RGB32F);
        m_color_texture->SetFormat(CoreEngine::GLEnums::RGB);
        m_color_texture->SetFilter(CoreEngine::GLEnums::NEAREST, CoreEngine::GLEnums::NEAREST);
        m_color_texture->SetWrapMode(CoreEngine::GLEnums::CLAMP_TO_EDGE, CoreEngine::GLEnums::CLAMP_TO_EDGE);
    }

    if (m_has_normal_texture) {
        m_normal_texture = std::make_shared<Texture2D>(width, height, (unsigned char*)nullptr);
        m_normal_texture->SetInternalFormat(CoreEngine::GLEnums::RGBA32F);
        m_normal_texture->SetFormat(CoreEngine::GLEnums::RGBA);
        m_normal_texture->SetFilter(CoreEngine::GLEnums::NEAREST, CoreEngine::GLEnums::NEAREST);
        m_normal_texture->SetWrapMode(CoreEngine::GLEnums::CLAMP_TO_EDGE, CoreEngine::GLEnums::CLAMP_TO_EDGE);
    }

    if (m_has_position_texture) {
        m_position_texture = std::make_shared<Texture2D>(width, height, (unsigned char*)nullptr);
        m_position_texture->SetInternalFormat(CoreEngine::GLEnums::RGBA32F);
        m_position_texture->SetFormat(CoreEngine::GLEnums::RGBA);
        m_position_texture->SetFilter(CoreEngine::GLEnums::NEAREST, CoreEngine::GLEnums::NEAREST);
        m_position_texture->SetWrapMode(CoreEngine::GLEnums::CLAMP_TO_EDGE, CoreEngine::GLEnums::CLAMP_TO_EDGE);
    }

    if (m_has_depth_texture) {
        m_depth_texture = std::make_shared<Texture2D>(width, height, (unsigned char*)nullptr);
        m_depth_texture->SetInternalFormat(CoreEngine::GLEnums::DEPTH_COMPONENT32F);
        m_depth_texture->SetFormat(CoreEngine::GLEnums::DEPTH_COMPONENT);
        m_depth_texture->SetFilter(CoreEngine::GLEnums::NEAREST, CoreEngine::GLEnums::NEAREST);
        m_depth_texture->SetWrapMode(CoreEngine::GLEnums::CLAMP_TO_EDGE, CoreEngine::GLEnums::CLAMP_TO_EDGE);
    }

    if (m_has_data_texture) {
        m_data_texture = std::make_shared<Texture2D>(width, height, (unsigned char*)nullptr);
        m_data_texture->SetInternalFormat(CoreEngine::GLEnums::RGBA8);
        m_data_texture->SetFormat(CoreEngine::GLEnums::RGBA);
        m_data_texture->SetFilter(CoreEngine::GLEnums::NEAREST, CoreEngine::GLEnums::NEAREST);
        m_data_texture->SetWrapMode(CoreEngine::GLEnums::CLAMP_TO_EDGE, CoreEngine::GLEnums::CLAMP_TO_EDGE);
    }
}

Framebuffer2D::~Framebuffer2D()
{
    // deleted in parent destructor
}

const std::shared_ptr<Texture> Framebuffer2D::GetColorTexture() const { return m_color_texture; }
const std::shared_ptr<Texture> Framebuffer2D::GetNormalTexture() const { return m_normal_texture; }
const std::shared_ptr<Texture> Framebuffer2D::GetPositionTexture() const { return m_position_texture; }
const std::shared_ptr<Texture> Framebuffer2D::GetDepthTexture() const { return m_depth_texture; }
const std::shared_ptr<Texture> Framebuffer2D::GetDataTexture() const { return m_data_texture; }

void Framebuffer2D::Use()
{
    if (!is_created) {
        CoreEngine::GetInstance()->GenFramebuffers(1, &id);
        CatchGLErrors("Failed to generate framebuffer.");

        is_created = true;
    }

    CoreEngine::GetInstance()->BindFramebuffer(CoreEngine::GLEnums::FRAMEBUFFER, id);
    CatchGLErrors("Failed to bind framebuffer.", false);

    CoreEngine::GetInstance()->Viewport(0, 0, width, height);

    if (!is_uploaded) {
        unsigned int draw_buffers[4] = { GL_NONE };
        int draw_buffer_index = 0;

        if (m_has_color_texture) {
            m_color_texture->Begin();
            CoreEngine::GetInstance()->FramebufferTexture(CoreEngine::GLEnums::FRAMEBUFFER,
                CoreEngine::GLEnums::COLOR_ATTACHMENT0, CoreEngine::GLEnums::TEXTURE_2D, m_color_texture->GetId(), 0);
            CatchGLErrors("Failed to attach color attachment 0 to framebuffer.", false);
            m_color_texture->End();

            draw_buffers[draw_buffer_index++] = CoreEngine::GLEnums::COLOR_ATTACHMENT0;
        }

        if (m_has_normal_texture) {
            m_normal_texture->Begin();
            CoreEngine::GetInstance()->FramebufferTexture(CoreEngine::GLEnums::FRAMEBUFFER,
                CoreEngine::GLEnums::COLOR_ATTACHMENT0 + 1, CoreEngine::GLEnums::TEXTURE_2D, m_normal_texture->GetId(), 0);
            CatchGLErrors("Failed to attach color attachment 1 to framebuffer.", false);
            m_normal_texture->End();

            draw_buffers[draw_buffer_index++] = CoreEngine::GLEnums::COLOR_ATTACHMENT0 + 1;
        }

        if (m_has_position_texture) {
            m_position_texture->Begin();
            CoreEngine::GetInstance()->FramebufferTexture(CoreEngine::GLEnums::FRAMEBUFFER,
                CoreEngine::GLEnums::COLOR_ATTACHMENT0 + 2, CoreEngine::GLEnums::TEXTURE_2D, m_position_texture->GetId(), 0);
            CatchGLErrors("Failed to attach color attachment 2 to framebuffer.", false);
            m_position_texture->End();

            draw_buffers[draw_buffer_index++] = CoreEngine::GLEnums::COLOR_ATTACHMENT0 + 2;
        }

        if (m_has_data_texture) {
            m_data_texture->Begin();
            CoreEngine::GetInstance()->FramebufferTexture(CoreEngine::GLEnums::FRAMEBUFFER,
                CoreEngine::GLEnums::COLOR_ATTACHMENT0 + 3, CoreEngine::GLEnums::TEXTURE_2D, m_data_texture->GetId(), 0);
            CatchGLErrors("Failed to attach color attachment 3 to framebuffer.", false);
            m_data_texture->End();

            draw_buffers[draw_buffer_index++] = CoreEngine::GLEnums::COLOR_ATTACHMENT0 + 3;
        }

        if (m_has_depth_texture) {
            m_depth_texture->Begin();
            CoreEngine::GetInstance()->FramebufferTexture(CoreEngine::GLEnums::FRAMEBUFFER,
                CoreEngine::GLEnums::DEPTH_ATTACHMENT, CoreEngine::GLEnums::TEXTURE_2D, m_depth_texture->GetId(), 0);
            CatchGLErrors("Failed to attach depth texture to framebuffer.", false);
            m_depth_texture->End();
        }

        CoreEngine::GetInstance()->DrawBuffers(draw_buffer_index, draw_buffers);

        if (CoreEngine::GetInstance()->CheckFramebufferStatus(CoreEngine::GLEnums::FRAMEBUFFER) != CoreEngine::GLEnums::FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("Could not create framebuffer");
        }

        is_uploaded = true;
    }

}

void Framebuffer2D::StoreColor()
{
    if (!m_has_color_texture) {
        return;
    }

    m_color_texture->Begin();

    CoreEngine::GetInstance()->CopyTexSubImage2D(CoreEngine::GLEnums::TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

    m_color_texture->End();
}

void Framebuffer2D::StoreDepth()
{
    if (!m_has_depth_texture) {
        return;
    }

    m_depth_texture->Begin();

    CoreEngine::GetInstance()->CopyTexSubImage2D(CoreEngine::GLEnums::TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

    m_depth_texture->End();
}

} // namespace apex
