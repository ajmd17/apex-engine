#ifndef BOUNDING_BOX_CONTROL_H
#define BOUNDING_BOX_CONTROL_H

#include <cstddef>
#include <vector>

#include "../control.h"
#include "../math/bounding_box.h"
#include "../entity.h"
#include "../rendering/renderers/bounding_box_renderer.h"

namespace apex {
class BoundingBoxControl : public EntityControl {
public:
    BoundingBoxControl();
    virtual ~BoundingBoxControl();

    virtual void OnAdded();
    virtual void OnRemoved();
    virtual void OnUpdate(double dt);

private:
    std::shared_ptr<Entity> m_entity;
    std::shared_ptr<BoundingBoxRenderer> m_bounding_box_renderer;
};
} // namespace apex

#endif
