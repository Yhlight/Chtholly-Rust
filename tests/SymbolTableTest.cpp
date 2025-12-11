#include <gtest/gtest.h>
#include "../src/Semantic/SymbolTable.h"
#include "../src/Type/Type.h"

TEST(SymbolTableTest, AddAndLookupSymbol) {
    SymbolTable st;
    Symbol s("x", false, Type(BuiltinType::I32));

    EXPECT_TRUE(st.add(s));
    auto lookedUp = st.lookup("x");
    ASSERT_TRUE(lookedUp.has_value());
    EXPECT_EQ(lookedUp->getName(), "x");
    EXPECT_FALSE(lookedUp->getIsMutable());
    EXPECT_EQ(lookedUp->getType().getBuiltinType(), BuiltinType::I32);
}

TEST(SymbolTableTest, LookupNonExistentSymbol) {
    SymbolTable st;
    auto lookedUp = st.lookup("y");
    ASSERT_FALSE(lookedUp.has_value());
}

TEST(SymbolTableTest, AddDuplicateSymbol) {
    SymbolTable st;
    Symbol s1("x", false, Type(BuiltinType::I32));
    Symbol s2("x", true, Type(BuiltinType::I32));

    EXPECT_TRUE(st.add(s1));
    EXPECT_FALSE(st.add(s2)); // Should fail to add duplicate
}
