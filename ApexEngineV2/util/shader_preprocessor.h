#ifndef SHADER_PREPROCESSOR_H
#define SHADER_PREPROCESSOR_H

#include <string>
#include <sstream>
#include <map>

namespace apex {

class ShaderPreprocessor {
public:
    static std::string ProcessShader(const std::string &code, 
        std::map<std::string, float> defines,
        const std::string &path = "");

private:
    static std::string ProcessInner(std::istringstream &is, 
        std::streampos &pos, 
        std::map<std::string, float> &defines,
        const std::string &local_path);
};

} // namespace apex

#endif
