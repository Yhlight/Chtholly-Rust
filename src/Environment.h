#pragma once

#include "Object.h"
#include <string>
#include <unordered_map>
#include <memory>

class Environment {
public:
    Environment(std::shared_ptr<Environment> enclosing = nullptr);

    void define(const std::string& name, const Object& value);
    Object get(const std::string& name);
    void assign(const std::string& name, const Object& value);

private:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, Object> values;
};
