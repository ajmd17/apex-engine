#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <array>
#include <vector>
#include <memory>

#include "./texture_2D.h"

#define CUBEMAP_NUM_MIPMAPS 5

namespace apex {

class Cubemap : public Texture {
public:

    Cubemap(const std::array<std::shared_ptr<Texture2D>, 6> &textures);
    virtual ~Cubemap();

    inline const std::array<std::shared_ptr<Texture2D>, 6> GetTextures() const
        { return m_textures; }

    virtual void CopyData(Texture * const other) override;

    void Use(bool upload_data = true);
    void End();

private:
    std::array<std::shared_ptr<Texture2D>, 6> m_textures;
};

} // namespace apex

#endif
