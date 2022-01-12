#include "ui_button.h"
#include "../shader_manager.h"
#include "../shaders/ui/ui_button_shader.h"
#include "../../util/mesh_factory.h"

#include <algorithm>

namespace apex {
namespace ui {
UIButton::UIButton(const std::string &name)
    : UIObject(name)
{
    m_renderable->SetShader(ShaderManager::GetInstance()->GetShader<UIButtonShader>({ }));
}
} // namespace ui
} // namespace apex
