#pragma once

#include <string>
#include <variant>

// Using std::monostate to represent 'null' or uninitialized values
using Object = std::variant<std::monostate, int, double, std::string, bool>;

// Helper function to convert an Object to its string representation
std::string objectToString(const Object& obj);
