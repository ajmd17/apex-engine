#include "shader_preprocessor.h"
#include "../rendering/shader.h"
#include "../asset/asset_manager.h"
#include "../asset/text_loader.h"
#include "string_util.h"

#include <iostream>

namespace apex {

std::string ShaderPreprocessor::ProcessShader(const std::string &code, 
    const ShaderProperties &shader_properties, 
    const std::string &path,
    int *line_num_ptr)
{
    std::istringstream ss(code);
    std::ostringstream os;
    std::streampos pos;
    std::string line;

    std::string local_path(path.substr(0, path.find_last_of("\\/")));

    if (!(StringUtil::Contains(local_path, "/") ||
        StringUtil::Contains(local_path, "\\"))) {
        local_path.clear();
    }

    int line_num = 0;

    if (line_num_ptr == nullptr) {
        line_num_ptr = &line_num;
    }

    return FileHeader(path) + ProcessInner(ss, pos, shader_properties, local_path, line_num_ptr);
}

std::string ShaderPreprocessor::ProcessInner(std::istringstream &ss, 
    std::streampos &pos,
    const ShaderProperties &shader_properties,
    const std::string &local_path,
    int *line_num_ptr)
{
    ShaderProperties defines(shader_properties);

    std::string res;

    std::string line;

    while (std::getline(ss, line)) {
        (*line_num_ptr)++;

        line = StringUtil::Trim(line);

        std::string new_line = "/* ";
        new_line += std::to_string(*line_num_ptr);
        new_line += " */ ";

        if (StringUtil::StartsWith(line, "#define $")) {
            std::string sub = line.substr(9);

            const std::vector<std::string> components = StringUtil::Split(sub, ' ');

            std::vector<std::string> new_components;
            new_components.reserve(components.size());

            for (const auto &item : components) {
                const auto trimmed = StringUtil::Trim(item);

                if (!trimmed.empty()) {
                    new_components.push_back(trimmed);
                }
            }

            if (new_components.size() >= 2) {
                std::string key = new_components[0];
                std::string value = line.substr(9 + key.size());

                ShaderProperties::ShaderProperty::Value result;

                if (new_components.size() == 2) {
                    if (StringUtil::ParseNumber<float>(value, &result.float_value)) {
                        if (MathUtil::Round(result.float_value) == result.float_value) {
                            defines.Define(key, int(result.float_value));
                        } else {
                            defines.Define(key, result.float_value);
                        }
                    } else if (StringUtil::ParseNumber<int>(value, &result.int_value)) {
                        defines.Define(key, result.int_value);
                    } else if (StringUtil::ParseNumber<bool>(value, &result.bool_value)) {
                        defines.Define(key, result.bool_value);
                    } else {
                        defines.Define(key, value);
                    }
                } else {
                    defines.Define(key, value);
                }
            } else {
                new_line += "#error \"The `#define $` directive must be defined in the format: `#define $NAME value`\"";
            }
        } else if (StringUtil::StartsWith(line, "#if !")) {
            std::string key = line.substr(5);
            std::string inner = ProcessInner(ss, pos, defines, local_path, line_num_ptr);

            if (!defines.GetValue(key).IsTruthy()) {
                new_line += inner;
            }
        } else if (StringUtil::StartsWith(line, "#if ")) {
            std::string key = line.substr(4);
            std::string inner = ProcessInner(ss, pos, defines, local_path, line_num_ptr);

            if (defines.GetValue(key).IsTruthy()) {
                new_line += inner;
            }
        } else if (StringUtil::StartsWith(line, "#endif")) {
            break; // exit current recursion
        } else if (StringUtil::StartsWith(line, "#include ")) {
            std::string val = line.substr(9);
            std::string path;

            if (val[0] == '\"') {
                int idx = 1;
                while (idx < val.size() && val[idx] != '\"') {
                    path += val[idx];
                    ++idx;
                }
            }

            const std::string include_path = local_path + "/" + path;

            if (auto loaded = AssetManager::GetInstance()->
                LoadFromFile<TextLoader::LoadedText>(include_path)) {

                std::string relative_path(loaded->GetFilePath().substr(0,
                    loaded->GetFilePath().find_last_of("\\/")));

                if (!(StringUtil::Contains(relative_path, "/") ||
                    StringUtil::Contains(relative_path, "\\"))) {
                    relative_path.clear();
                }

                new_line += ProcessShader(loaded->GetText(), defines, relative_path, line_num_ptr) + "\n";
                new_line += "\n";
                new_line += FileHeader(local_path);
            } else {
                new_line += "#error \"The include could not be found at: ";
                new_line += include_path;
                new_line += "\"";
            }
        } else {
            new_line += line;
        }

        res += new_line + "\n";
    }

    std::string res_processed;
    res_processed.reserve(res.length());

    for (size_t i = 0; i < res.length(); i++) {
        char ch = res[i];
        if (ch == '$') {
            size_t start_pos = i;
            std::string str = "";

            while (i + 1 < res.length() && (std::isalnum(res[i + 1]) || res[i + 1] == '_')) {
                ch = res[++i];
                str += ch;
            }

            if (defines.HasValue(str)) {
                auto value = defines.GetValue(str);
                res_processed += value.raw_value;
            } else {
                res_processed += "__UNDEFINED__";
            }
        } else {
            res_processed += ch;
        }
    }

    return res_processed;
}

std::string ShaderPreprocessor::FileHeader(const std::string &path)
{
    std::string header = "/* ===== ";
    header += path;
    header += " ===== */\n";
    return header;
}

} // namespace apex
