#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>

enum class BuiltinType {
    I32,
    Unknown
};

class Type {
public:
    Type(BuiltinType type) : type(type) {}

    BuiltinType getBuiltinType() const { return type; }

private:
    BuiltinType type;
};

#endif //CHTHOLLY_TYPE_H
