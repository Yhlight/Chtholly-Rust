#include <gtest/gtest.h>
#include "../src/Semantic/SymbolTable.h"

TEST(SymbolTableTest, AddAndLookupSymbol) {
    SymbolTable st;
    Symbol s("x", false);

    EXPECT_TRUE(st.add(s));
    auto lookedUp = st.lookup("x");
    ASSERT_TRUE(lookedUp.has_value());
    EXPECT_EQ(lookedUp->getName(), "x");
    EXPECT_FALSE(lookedUp->getIsMutable());
}

TEST(SymbolTableTest, LookupNonExistentSymbol) {
    SymbolTable st;
    auto lookedUp = st.lookup("y");
    ASSERT_FALSE(lookedUp.has_value());
}

TEST(SymbolTableTest, AddDuplicateSymbol) {
    SymbolTable st;
    Symbol s1("x", false);
    Symbol s2("x", true);

    EXPECT_TRUE(st.add(s1));
    EXPECT_FALSE(st.add(s2)); // Should fail to add duplicate
}
