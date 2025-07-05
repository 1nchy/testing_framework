#include "test.hpp"

struct TRUE {
    operator bool() const { return true; }
};
struct FALSE {
    operator bool() const { return false; }
};
struct integer {
public:
    explicit integer(int _i) : _i(_i) {}
    void increase() { ++_i; }
    void decrease() { --_i; }
    auto operator==(const integer& _rhs) const -> bool { return _i == this->_i; }
    template <std::integral _T> auto operator==(const _T& _rhs) const -> bool { return _i == _rhs; }
private:
    int _i;
};

ICY_CASE("assertion") {
    int a = 3, b = 4;
    bool f = false;
    std::string s = "hello";
    std::vector<int> v(3, 0);
    integer i(7);
    ICY_SUBCASE("EXPECT_EQ/NE") {
        EXPECT_EQ(a, b);
        EXPECT_EQ("HELLO", s);
        EXPECT_EQ(s[2], 'e');
        EXPECT_EQ(i, 5);
        EXPECT_NE(a, b);
        EXPECT_LT(b, a);
    }
    ICY_SUBCASE("EXPECT_TRUE/FALSE") {
        EXPECT_TRUE(false);
        EXPECT_TRUE(f);
        EXPECT_TRUE(FALSE{});
        EXPECT_FALSE(true);
        EXPECT_FALSE(!f);
        EXPECT_FALSE(TRUE{});
    }
    ICY_SUBCASE("EXPECT_THROW") {
        EXPECT_THROW(std::out_of_range, v.at(2) = 1);
        EXPECT_THROW(std::out_of_range, v.at(5) = 1);
    }
    ICY_SUBCASE("EXPECT_NOTHROW") {
        EXPECT_NOTHROW(v.at(2) = 1);
        EXPECT_NOTHROW(v.at(5) = 1);
    }
}