#ifndef SKYBOX_H
#define SKYBOX_H

#include "../../control.h"
#include "../cubemap.h"
#include "../mesh.h"
#include "../texture.h"
#include "../camera/camera.h"
#include "../../math/vector4.h"

#include <memory>

namespace apex {
class SkyboxControl : public EntityControl {
public:
    SkyboxControl(Camera *camera, const std::shared_ptr<Cubemap> &cubemap);
    virtual ~SkyboxControl() = default;

    void OnAdded();
    void OnRemoved();
    void OnUpdate(double dt);

private:
    std::shared_ptr<Entity> m_cube;
    std::shared_ptr<Cubemap> m_cubemap;
    Camera *m_camera;
};
} // namespace apex

#endif
