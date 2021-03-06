#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>

namespace apex {

class StringUtil {
public:
    static inline bool StartsWith(const std::string &text, const std::string &token)
    {
        if (text.length() < token.length()) {
            return false;
        }
        return (text.compare(0, token.length(), token) == 0);
    }

    static inline bool EndsWith(const std::string &text, const std::string &token)
    {
        if (text.length() < token.length()) {
            return false;
        }
        return std::equal(text.begin() + text.size() - token.size(),
            text.end(), token.begin());
    }

    static inline bool Contains(const std::string &text, const std::string &token)
    {
        return text.find(token) != std::string::npos;
    }

    static inline std::vector<std::string> Split(const std::string &text, const char sep)
    {
        std::vector<std::string> tokens;
        size_t start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos) {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }

    static inline std::vector<std::string> RemoveEmpty(const std::vector<std::string> &strings)
    {
        std::vector<std::string> res;
        for (auto &&str : strings) {
            if (!str.empty()) {
                res.push_back(str);
            }
        }
        return res;
    }

    static inline std::string TrimLeft(const std::string &s)
    {
        std::string res(s);
        res.erase(res.begin(), std::find_if(res.begin(), res.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
        return res;
    }

    static inline std::string TrimRight(const std::string &s)
    {
        std::string res(s);
        res.erase(std::find_if(res.rbegin(), res.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), res.end());
        return res;
    }

    static inline std::string Trim(const std::string &s)
    {
        return TrimLeft(TrimRight(s));
    }

    static inline std::string ReplaceAll(const std::string &text,
        const std::string &from, const std::string &to)
    {
        std::string result(text);
        if (from.empty()) {
            return result;
        }
        size_t start_pos = 0;
        while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
            result.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return result;
    }
};

} // namespace apex

#endif
