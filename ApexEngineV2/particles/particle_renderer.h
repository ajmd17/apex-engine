#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include "particle.h"
#include "particle_construction_info.h"
#include "particle_shader.h"
#include "../rendering/renderable.h"
#include "../rendering/camera/camera.h"

#include <vector>
#include <memory>

namespace apex {
class ParticleRenderer : public Renderable {
    friend class ParticleEmitterControl;
public:
    ParticleRenderer(const ParticleConstructionInfo &info);
    ~ParticleRenderer();

    virtual void Render() override;

private:
    // pointer to particle vector (set by ParticleEmitterControl)
    std::vector<Particle> *m_particles;

    ParticleConstructionInfo m_info;

    bool m_is_created;

    unsigned int m_vertex_buffer;
    unsigned int m_position_buffer;
    unsigned int m_lifespan_buffer;
    unsigned int m_vao;
};
} // namespace apex

#endif
