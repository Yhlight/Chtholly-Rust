#ifndef CHTHOLLY_SYMBOL_H
#define CHTHOLLY_SYMBOL_H

#include "../Type/Type.h"
#include <string>

class Symbol {
public:
    Symbol(const std::string& name, bool isMutable, Type type)
        : name(name), isMutable(isMutable), type(type) {}

    const std::string& getName() const { return name; }
    bool getIsMutable() const { return isMutable; }
    Type getType() const { return type; }

private:
    std::string name;
    bool isMutable;
    Type type;
};

#endif //CHTHOLLY_SYMBOL_H
