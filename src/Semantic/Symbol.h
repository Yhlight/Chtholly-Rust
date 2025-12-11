#ifndef CHTHOLLY_SYMBOL_H
#define CHTHOLLY_SYMBOL_H

#include <string>

class Symbol {
public:
    Symbol(const std::string& name, bool isMutable)
        : name(name), isMutable(isMutable) {}

    const std::string& getName() const { return name; }
    bool getIsMutable() const { return isMutable; }

private:
    std::string name;
    bool isMutable;
};

#endif //CHTHOLLY_SYMBOL_H
