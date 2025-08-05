#include "test.hpp"

#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <utility>
#include <tuple>

struct A {};
template <typename _T> struct B {};
template <typename _T1, typename _T2> struct C {};
template <typename... _Ts> struct D {};

template <typename _T> struct E {
public:
    E(std::initializer_list<_T> _il) : _data(_il) {}
    auto begin() const { return _data.begin(); }
    auto end() const { return _data.end(); }
private:
    std::vector<_T> _data;
};
template <typename _T> struct F {
public:
    F(std::initializer_list<_T> _il) : _data(_il) {}
    auto begin() const { return _data.begin(); }
    auto end() const { return _data.end(); }
    auto rbegin() const { return _data.rbegin(); }
    auto rend() const { return _data.rend(); }
private:
    std::vector<_T> _data;
};

ICY_CASE("basic") {
    EXPECT_EQ(test::to_string(0), "0");
    EXPECT_EQ(test::to_string(1), "1");
    EXPECT_EQ(test::to_string(-7), "-7");
    EXPECT_TRUE(test::to_string(3.14).starts_with("3.14"));
    EXPECT_TRUE(test::to_string(-2.71).starts_with("-2.71"));
    EXPECT_EQ(test::to_string(true), "true");
    EXPECT_EQ(test::to_string(false), "false");
    EXPECT_EQ(test::to_string(nullptr), "nullptr");
    const char* _s = "hello";
    const std::string _str = "world";
    const std::string _empty = "";
    EXPECT_EQ(test::to_string(_s), "\"hello\"");
    EXPECT_EQ(test::to_string(_str), "\"world\"");
    EXPECT_EQ(test::to_string(_empty), "\"\"");
}
ICY_CASE("pair/tuple") {
    const std::pair<std::string, unsigned> _p {"string", 6};
    EXPECT_EQ(test::to_string(_p), "(\"string\",6)");
    const std::tuple<char, unsigned, std::string> _t {'A', 2, "three"};
    EXPECT_EQ(test::to_string(_t), "(\'A\',2,\"three\")");
    const std::pair<std::tuple<bool, unsigned, char>, int> _pt {{true, 2, 'c'}, 4};
    EXPECT_EQ(test::to_string(_pt), "((true,2,\'c\'),4)");
    const std::tuple<std::pair<bool, unsigned>, int, char> _tp {{false, 2}, 3, 'd'};
    EXPECT_EQ(test::to_string(_tp), "((false,2),3,\'d\')");
}
ICY_CASE("bidirectional") {
    const unsigned _u[] = {2,3,5,7};
    EXPECT_EQ(test::to_string(_u), "[2,3,5,7]");
    const std::array<int, 4> _a {1,-2,3,-4};
    EXPECT_EQ(test::to_string(_a), "[1,-2,3,-4]");
    const std::vector<unsigned> _v {0,2,4,6,8};
    EXPECT_EQ(test::to_string(_v), "[0,2,4,6,8]");
    const std::list<char> _l {'x','y','z'};
    EXPECT_EQ(test::to_string(_l), "[\'x\',\'y\',\'z\']");
    const std::vector<char> _vc {};
    EXPECT_EQ(test::to_string(_vc), "[]");
    const std::set<unsigned> _s = {2,5,3,7};
    EXPECT_EQ(test::to_string(_s), "[2,3,5,7]");
    const std::map<unsigned, char> _m = {{2,'b'},{5,'e'},{3,'c'},{7,'g'}};
    EXPECT_EQ(test::to_string(_m), "[(2,'b'),(3,'c'),(5,'e'),(7,'g')]");
}
ICY_CASE("forward") {
    auto only_substr = [](const std::string& _src, const std::string& _x) -> bool {
        const auto _first = _src.find(_x);
        const auto _last = _src.rfind(_x);
        return (_first != std::string::npos) && (_first == _last);
    };
    const std::unordered_set<unsigned> _s = {2,3,5,7};
    const std::string _ss = test::to_string(_s);
    EXPECT_TRUE(_ss.starts_with("{") && _ss.ends_with("}"));
    EXPECT_TRUE(only_substr(_ss, "2"));
    EXPECT_TRUE(only_substr(_ss, "3"));
    EXPECT_TRUE(only_substr(_ss, "5"));
    EXPECT_TRUE(only_substr(_ss, "7"));
    const std::unordered_map<unsigned, char> _m = {{2,'b'},{5,'e'},{3,'c'},{7,'g'}};
    const std::string _ms = test::to_string(_m);
    EXPECT_TRUE(_ms.starts_with("{") && _ms.ends_with("}"));
    EXPECT_TRUE(only_substr(_ms, "(2,'b')"));
    EXPECT_TRUE(only_substr(_ms, "(3,'c')"));
    EXPECT_TRUE(only_substr(_ms, "(5,'e')"));
    EXPECT_TRUE(only_substr(_ms, "(7,'g')"));
}
ICY_CASE("custom") {
    EXPECT_EQ(test::to_string(A()), "A(...)");
    EXPECT_EQ(test::to_string(B<int>()), "B<int>(...)");
    const auto _c = C<char, void>();
    EXPECT_EQ(test::to_string(_c), "C<char, void>(...)");
    const auto _d = D<int,float,A>();
    EXPECT_EQ(test::to_string(_d), "D<int, float, A>(...)");
    const auto _e = E<unsigned>({2,3,5,7});
    EXPECT_EQ(test::to_string(_e), "{2,3,5,7}");
    const auto _f = F<unsigned>({2,3,5,7});
    EXPECT_EQ(test::to_string(_f), "[2,3,5,7]");
}
ICY_CASE("arguments") {
    const auto _x = std::vector<int>{1,2,3};
    const auto _y = std::string("hello");
    const auto _z = std::make_tuple(0u, 0);
    EXPECT_EQ(test::to_string<test::NONE>(_x, _y, _z), "[1,2,3],\"hello\",(0,0)");
}
ICY_CASE("combination") {
    const auto _x = std::vector<int>{1,2,3};
    const auto _y = std::string("hello");
    const auto _z = std::make_tuple(0u, 0);
    EXPECT_EQ(test::to_string(std::make_tuple(_x, _y, _z)), "([1,2,3],\"hello\",(0,0))");
    EXPECT_EQ(test::to_string(std::vector<std::remove_const<decltype(_x)>::type>{_x,_x}), "[[1,2,3],[1,2,3]]");
    EXPECT_EQ(test::to_string(std::vector<std::remove_const<decltype(_z)>::type>{_z,_z}), "[(0,0),(0,0)]");
}