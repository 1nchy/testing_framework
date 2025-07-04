#include "test.hpp"

struct A {
    operator bool() const { return false; }
};

ICY_CASE("assertion") {
    int a = 3, b = 4;
    bool f = false;
    std::string s = "hello";
    EXPECT_EQ(a, b);
    EXPECT_EQ(s[2], 'e');
    EXPECT_TRUE(f);
    std::vector<int> v(3, 0);
    EXPECT_LT(b, a);
    EXPECT_THROW(std::out_of_range, v.at(2) = 1);
    EXPECT_THROW(std::out_of_range, v.at(5) = 1);
    EXPECT_NOTHROW(v.at(2) = 1);
    EXPECT_NOTHROW(v.at(5) = 1);
}