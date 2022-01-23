#ifndef BYTE_WRITER_H
#define BYTE_WRITER_H

#include <fstream>

namespace apex {
class ByteWriter {
public:
    virtual ~ByteWriter() {}

    template <typename T>
    void Write(T *ptr, unsigned size = sizeof(T))
    {
        WriteBytes(reinterpret_cast<char*>(ptr), size);
    }

    virtual std::streampos Position() const = 0;
    virtual void Close() = 0;

protected:
    virtual void WriteBytes(char *ptr, unsigned size) = 0;
};

class FileByteWriter : public ByteWriter {
public:
    FileByteWriter(const std::string &filepath, std::streampos begin = 0)
    {
        file = new std::ofstream(filepath, std::ifstream::out | std::ifstream::binary);
    }

    ~FileByteWriter()
    {
        delete file;
    }

    std::streampos Position() const
    {
        return file->tellg();
    }

    void Close()
    {
        file->close();
    }

private:
    std::ostream *file;

    void WriteBytes(char *ptr, unsigned size)
    {
        file->write(ptr, size);
    }
};
}

#endif
