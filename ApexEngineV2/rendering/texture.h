#ifndef TEXTURE_H
#define TEXTURE_H

#include "../asset/loadable.h"

namespace apex {

class Texture : public Loadable {
    friend class TextureLoader;
public:
    Texture();
    Texture(int width, int height, unsigned char *bytes);
    virtual ~Texture();

    unsigned int GetId() const;

    void SetFormat(int type);
    inline int GetFormat() const { return fmt; }
    void SetInternalFormat(int type);
    inline int GetInternalFormat() const { return ifmt; }
    void SetFilter(int mag, int min);
    void SetWrapMode(int s, int t);

    virtual void CopyData(Texture * const other) = 0;

    inline int GetWidth() const { return width; }
    inline int GetHeight() const { return height; }

    inline int GetMagFilter() const { return mag_filter; }
    inline int GetMinFilter() const { return min_filter; }
    inline int GetWrapS() const { return wrap_s; }
    inline int GetWrapT() const { return wrap_t; }

    unsigned char * const GetBytes() const { return bytes; }

    static size_t NumComponents(int format);
    static void ActiveTexture(int i);

    virtual void Use(bool upload_data = true) = 0;
    virtual void End() = 0;

    unsigned int id;

    bool is_created, is_uploaded;

protected:
    int ifmt, fmt, width, height;
    unsigned char *bytes;

    int mag_filter, min_filter;
    int wrap_s, wrap_t;
};

} // namespace apex

#endif
