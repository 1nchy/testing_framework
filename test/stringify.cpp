#include "test.hpp"

#include <array>
#include <vector>
#include <list>

struct A {};
template <typename _T> struct B {};
template <typename _T1, typename _T2> struct C {};
template <typename... _Ts> struct D {};

ICY_CASE("basic") {
    EXPECT_EQ(ICY_STRINGIFY(0), "0");
    EXPECT_EQ(ICY_STRINGIFY(1), "1");
    EXPECT_EQ(ICY_STRINGIFY(-7), "-7");
    EXPECT_TRUE(ICY_STRINGIFY(3.14).starts_with("3.14"));
    EXPECT_TRUE(ICY_STRINGIFY(-2.71).starts_with("-2.71"));
    EXPECT_EQ(ICY_STRINGIFY(true), "true");
    EXPECT_EQ(ICY_STRINGIFY(false), "false");
    EXPECT_EQ(ICY_STRINGIFY(nullptr), "nullptr");
    const char* _s = "hello";
    const std::string _str = "world";
    EXPECT_EQ(ICY_STRINGIFY(_s), "\"hello\"");
    EXPECT_EQ(ICY_STRINGIFY(_str), "\"world\"");
}
ICY_CASE("pair/tuple") {
    const std::pair<std::string, unsigned> _p {"string", 6};
    EXPECT_EQ(ICY_STRINGIFY(_p), "(\"string\",6)");
    const std::tuple<char, unsigned, std::string> _t {'A', 2, "three"};
    EXPECT_EQ(ICY_STRINGIFY(_t), "(\'A\',2,\"three\")");
    const std::pair<std::tuple<bool, unsigned, char>, int> _pt {{true, 2, 'c'}, 4};
    EXPECT_EQ(ICY_STRINGIFY(_pt), "((true,2,\'c\'),4)");
    const std::tuple<std::pair<bool, unsigned>, int, char> _tp {{false, 2}, 3, 'd'};
    EXPECT_EQ(ICY_STRINGIFY(_tp), "((false,2),3,\'d\')");
}
ICY_CASE("range") {
    const std::array<int, 4> _a {1,-2,3,-4};
    EXPECT_EQ(ICY_STRINGIFY(_a), "{1,-2,3,-4}");
    const std::vector<unsigned> _v {0,2,4,6,8};
    EXPECT_EQ(ICY_STRINGIFY(_v), "{0,2,4,6,8}");
    const std::list<char> _l {'x','y','z'};
    EXPECT_EQ(ICY_STRINGIFY(_l), "{\'x\',\'y\',\'z\'}");
}
ICY_CASE("custom") {
    EXPECT_EQ(ICY_STRINGIFY(A()), "A");
    EXPECT_EQ(ICY_STRINGIFY(B<int>()), "B<int>");
    const auto _c = C<char, void>();
    EXPECT_EQ(ICY_STRINGIFY(_c), "C<char, void>");
    const auto _d = D<short,int,long,long long>();
    EXPECT_EQ(ICY_STRINGIFY(_d), "D<short int, int, long int, long long int>");
}