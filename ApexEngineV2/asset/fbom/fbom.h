#ifndef FBOM_H
#define FBOM_H

#include "../loadable.h"
#include "../asset_loader.h"
#include "../../math/math_util.h"

#include <vector>
#include <string>
#include <map>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <memory>
#include <map>

#define FBOM_DATA_MAX_SIZE size_t(64)

namespace apex {
class ByteReader;
namespace fbom {

using FBOMRawData_t = unsigned char[FBOM_DATA_MAX_SIZE];

enum FBOMResult {
    FBOM_OK = 0,
    FBOM_ERR = 1
};

struct FBOMData {
    FBOMData()
        : data_size(0),
          next(nullptr)
    {
    }

    FBOMData(const FBOMData &other)
        : data_size(other.data_size)
    {
        memcpy(raw_data, other.raw_data, data_size);

        if (other.next) {
            next = new FBOMData(other.next);
        }
    }

    ~FBOMData()
    {
        if (next != nullptr) {
            delete next;
        }
    }

    size_t TotalSize() const
    {
        const FBOMData *tip = this;
        size_t sz = 0;

        while (tip) {
            sz += data_size;
            tip = tip->next;
        }

        return sz;
    }

    void ReadBytes(size_t n, unsigned char *out) const
    {
        const FBOMData *tip = this;

        while (n && tip) {
            size_t to_read = MathUtil::Min(n, MathUtil::Min(FBOM_DATA_MAX_SIZE, data_size));
            memcpy(out, raw_data, to_read);

            n -= to_read;
            out += to_read;
            tip = tip->next;
        }
    }

    void SetBytes(size_t n, const unsigned char *data)
    {
        if (next) {
            delete next;
            next = nullptr;
        }

        size_t to_copy = MathUtil::Min(n, FBOM_DATA_MAX_SIZE);
        memcpy(raw_data, data, to_copy);
        data_size = to_copy;

        data += to_copy;
        n -= to_copy;

        if (n) {
            next = new FBOMData();
            next->SetBytes(n, data);
        }
    }

    inline std::string ToString() const
    {
        std::stringstream stream;
        stream << "FBOM[size: ";
        stream << std::to_string(data_size);
        stream << ", data: { ";

        for (size_t i = 0; i < data_size; i++) {
            stream << std::hex << int(raw_data[i]) << " ";
        }
        
        stream << " }, next: ";

        if (next != nullptr) {
            stream << next->ToString();
        }

        stream << "]";

        return stream.str();
    }

private:
    size_t data_size;
    FBOMRawData_t raw_data;
    FBOMData *next;

    FBOMData &operator=(const FBOMData &other) = delete;
};

struct FBOMObject {
public:
    std::string decl_type;
    std::vector<std::unique_ptr<FBOMObject>> nodes;
    std::map<std::string, FBOMData> properties;
    FBOMObject *parent = nullptr;

    template <typename T>
    inline void SetProperty(const std::string &key, const T &value)
    {
        static_assert(std::is_pod<T>::value, "T must be POD");

        unsigned char *bytes = (unsigned char*)&value;

        FBOMData data;
        data.SetBytes(sizeof(T), bytes);
        properties[key] = data;
    }

    FBOMObject *AddChild()
    {
        std::unique_ptr<FBOMObject> child_node = std::make_unique<FBOMObject>(object);
        child_node->parent = this;

        nodes.push_back(child_node);

        return child_node.get();
    }
};

class FBOMLoadable : public Loadable {
public:
    virtual FBOMResult Serialize(FBOMObject &out) const = 0;
    virtual FBOMResult Deserialize(const FBOMObject &in) = 0;

    virtual std::shared_ptr<Loadable> Clone() = 0;
};

enum FBOMCommand {
    FBOM_NONE = 0,
    FBOM_OBJECT_START,
    FBOM_OBJECT_END,
    FBOM_NODE_START,
    FBOM_NODE_END,
    FBOM_ADD_PROPERTY
};

class FBOMLoader : public AssetLoader {
public:
    FBOMLoader();
    virtual ~FBOMLoader();

    virtual std::shared_ptr<Loadable> LoadFromFile(const std::string &) override;

    // template <typename T>
    // void RegisterLoader(const std::string &key)
    // {
    //     static_assert(std::is_base_of<FBOMLoader, T>::value, 
    //         "Must be a derived class of FBOMLoader");
    //     std::string key_lower;
    //     key_lower.resize(ley.length());
    //     std::transform(ley.begin(), ley.end(), 
    //         key_lower.begin(), ::tolower);
    //     loaders[key_lower] = std::make_unique<T>();
    // }

private:
    FBOMObject *root;
    FBOMObject *last;
    // std::map<std::string, std::unique_ptr<FBOMLoader>> loaders;

    void Handle(ByteReader *, FBOMCommand);
};

} // namespace fbom
} // namespace apex

#endif
