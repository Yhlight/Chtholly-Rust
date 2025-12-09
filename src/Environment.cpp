#include "Environment.h"
#include <stdexcept>

Environment::Environment(std::shared_ptr<Environment> enclosing)
    : enclosing(std::move(enclosing)) {}

void Environment::define(const std::string& name, const Object& value) {
    values[name] = value;
}

Object Environment::get(const std::string& name) {
    if (values.count(name)) {
        return values.at(name);
    }

    if (enclosing != nullptr) {
        return enclosing->get(name);
    }

    throw std::runtime_error("Undefined variable '" + name + "'.");
}

void Environment::assign(const std::string& name, const Object& value) {
    if (values.count(name)) {
        values[name] = value;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }

    throw std::runtime_error("Undefined variable '" + name + "'.");
}
