#include "fbom.h"
#include "../byte_reader.h"
#include "../../entity.h"

namespace apex {
namespace fbom {

FBOMLoader::FBOMLoader()
    : root(new FBOMObject())
{
    last = root;
}

FBOMLoader::~FBOMLoader()
{
    if (root) {
        delete root;
    }
}

std::shared_ptr<Loadable> FBOMLoader::LoadFromFile(const std::string &path)
{
    ByteReader *reader = new FileByteReader(path);

    // TODO: file header

    // expect first FBOMObject defined

    while (reader->Position() < reader->Max()) {
        int32_t ins;
        reader->Read(&ins, sizeof(int32_t));
        Handle(reader, (FBOMCommand)ins);
    }

    // auto it = loaders.find(root->decl_type);

    // if (it == loaders.end()) {
    //     // no registered loader
    //     throw std::runtime_error("No registered handler for declaration type");
    // }

    hard_assert(last == root);

    std::shared_ptr<apex::Entity> entity = std::make_shared<apex::Entity>();

    entity->Deserialize(*root);

    return entity;
}

void FBOMLoader::Handle(ByteReader *reader, FBOMCommand command)
{
    switch (command) {
    case FBOM_OBJECT_START:
        hard_assert(last != nullptr);

        last = last->AddChild();

        // TODO read decl type and shit

        break;
    case FBOM_OBJECT_END:
        hard_assert(last != nullptr);
        hard_assert(last->parent != nullptr);

        last = last->parent;

        break;
    case FBOM_NODE_START:
        hard_assert(last != nullptr);

        last->

        break;
    }
}

} // namespace fbom
} // namespace apex
