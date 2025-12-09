#include "Object.h"
#include <sstream>

std::string objectToString(const Object& obj) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            ss << "null";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else {
            ss << arg;
        }
    }, obj);
    return ss.str();
}
