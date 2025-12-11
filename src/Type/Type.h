#ifndef CHTHOLLY_TYPE_H
#define CHTHOLLY_TYPE_H

#include <string>

enum class BuiltinType {
    I32,
    F64,
    Unknown
};

class Type {
public:
    Type(BuiltinType type) : type(type) {}

    BuiltinType getBuiltinType() const { return type; }

    bool operator==(const Type& other) const {
        return type == other.type;
    }

private:
    BuiltinType type;
};

#endif //CHTHOLLY_TYPE_H
