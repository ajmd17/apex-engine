#include "ui_manager.h"

namespace apex {
UIManager::UIManager(InputManager *input_manager)
    : m_input_manager(input_manager),
      m_input_event(new InputEvent),
      m_mouse_pressed(false)
{
    m_input_event->SetHandler([&](bool pressed) {
        m_mouse_pressed = pressed;
        // process click event on all ui objects
        HandleMouseEvent();
    });
    m_input_manager->RegisterClickEvent(MouseButton::MOUSE_BTN_LEFT, *m_input_event);
}

UIManager::~UIManager()
{
    delete m_input_event;
}

void UIManager::Update(double dt)
{
    HandleMouseEvent();
}

void UIManager::HandleMouseEvent()
{
    Vector2 mouse_vector(m_input_manager->GetMouseX(), m_input_manager->GetMouseY());
    mouse_vector *= 0.5;
    mouse_vector += 0.5;

    // mouse_vector *= 

    for (auto &object : m_ui_objects) {
        // std::cout << "update " << object->GetName() << "\n";
        if (object->IsMouseOver(m_input_manager->GetMouseX(), m_input_manager->GetMouseY())) {
            std::cout << "mouseover " << object->GetName() << "\n";
            object->GetClickEvent().Trigger(m_mouse_pressed);
        }
    }
}
} // namespace apex
