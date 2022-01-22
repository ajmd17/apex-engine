#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>

namespace apex {

#define ex_assert_msg(cond, msg) if (!(cond)) { throw std::runtime_error("Invalid argument: " msg "\n\tCondition: " #cond " evaluated to FALSE in file " __FILE__ " on line " __LINE__ "."); }
#define ex_assert(cond) { if (!(cond)) { const char *s = "Invalid argument.\n\tCondition: " #cond " evaluated to FALSE in file " __FILE__; throw std::runtime_error(s); } }
#define soft_assert(cond) if (!(cond)) { puts("Soft assertion failed in " __FILE__ " on line " __LINE__ ": " #cond); return; }

} // namespace apex

#endif
