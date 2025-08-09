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
    auto operator==(const integer& _rhs) const -> bool = default;
    auto operator!=(const integer& _rhs) const -> bool = default;
    template <std::integral _T> auto operator==(const _T& _rhs) const -> bool { return _i == _rhs; }
    template <std::integral _T> auto operator!=(const _T& _rhs) const -> bool { return !operator==(_rhs); }
    template <std::integral _T> friend auto operator==(const _T& _lhs, const integer& _rhs) -> bool;
    template <std::integral _T> friend auto operator!=(const _T& _lhs, const integer& _rhs) -> bool;
private:
    int _i;
};
template <std::integral _T> auto operator==(const _T& _lhs, const integer& _rhs) -> bool { return _lhs == _rhs._i; }
template <std::integral _T> auto operator!=(const _T& _lhs, const integer& _rhs) -> bool { return _lhs != _rhs._i; }
struct json_exception : public std::exception {
public:
    explicit json_exception(const std::string& _msg) : std::exception(), _msg(_msg) {}
    explicit json_exception(const char* _msg) : std::exception(), _msg(_msg) {}
    json_exception(const json_exception&) noexcept = default;
    json_exception(json_exception&&) noexcept = default;
    json_exception& operator=(const json_exception&) noexcept = default;
    json_exception& operator=(json_exception&&) noexcept = default;
    virtual ~json_exception() noexcept = default;
    virtual const char* what() const noexcept { return _msg.c_str(); }
public:
    static auto throw_with(const std::string& _msg) { throw json_exception(_msg); }
    static auto throw_with(const char* _msg) { throw json_exception(_msg); }
private:
    std::string _msg;
};

const int a = 3, b = 4;
const bool f = false;
const std::string s = "hello";
const integer i(7);
const std::vector<int> v(3, 0);

ICY_CASE("success") {
    ICY_SEQCASE("UNARY") {
        EXPECT_TRUE(!f);
        EXPECT_FALSE(f);
        EXPECT_TRUE(true);
        EXPECT_FALSE(false);
        EXPECT_TRUE(TRUE{});
        EXPECT_FALSE(FALSE{});
    }
    ICY_SEQCASE("BINARY") {
        EXPECT_LT(a, b);
        EXPECT_EQ("hello", s);
        EXPECT_EQ(s[1], 'e');
        EXPECT_EQ(i, 7);
        EXPECT_NE(a, b);
        EXPECT_GT(b, a);
    }
    ICY_SEQCASE("SEQUENCE") {
        EXPECT_INCR(1,1,2.0,5u,6);
        EXPECT_STRICT_INCR(1,2,3.0,5,6u);
        EXPECT_DECR(7,5.0,4,4u,1);
        EXPECT_STRICT_DECR(7,5.0,4,3u,1);
        EXPECT_EQUALS(integer(3), 3u, 3l, integer(3), integer(3));
    }
    ICY_SEQCASE("THROW") {
        EXPECT_THROW(std::out_of_range, void(v.at(5) == 1));
    }
    ICY_SEQCASE("THROW_WITH") {
        EXPECT_THROW_WITH(json_exception, "VALUE EXPECTED",
            json_exception::throw_with("VALUE EXPECTED")
        );
    }
    ICY_SEQCASE("NOTHROW") {
        EXPECT_NOTHROW(void(v.at(2) == 1));
    }
    REQUIRE_EQ(test::result::errors(), 0);
}
ICY_CASE("failed") {
    ICY_SEQCASE("UNARY") {
        EXPECT_EQ(a, b);
        EXPECT_EQ("WORLD", s);
        EXPECT_EQ(s[2], 'e');
        EXPECT_EQ(i, 5);
        EXPECT_LT(b, a);
    }
    ICY_SEQCASE("BINARY") {
        EXPECT_TRUE(false);
        EXPECT_TRUE(f);
        EXPECT_TRUE(FALSE{});
        EXPECT_FALSE(true);
        EXPECT_FALSE(!f);
        EXPECT_FALSE(TRUE{});
    }
    ICY_SEQCASE("SEQUENCE") {
        EXPECT_INCR(1,-1,2.0,5u,4);
        EXPECT_STRICT_INCR(1,1,3.0,5,5u);
        EXPECT_DECR(7,5.0,3,4u,1);
        EXPECT_STRICT_DECR(4,5.0,4,3u,4);
        EXPECT_EQUALS(integer(3), 2u, 3l, integer(3), integer(2));
    }
    ICY_SEQCASE("THROW") {
        EXPECT_THROW(std::out_of_range, void(v.at(2) == 1));
    }
    ICY_SEQCASE("THROW_WITH") {
        EXPECT_THROW_WITH(json_exception, "COMMA EXPECTED",
            json_exception::throw_with("VALUE EXPECTED")
        );
    }
    ICY_SEQCASE("NOTHROW") {
        EXPECT_NOTHROW(void(v.at(5) == 1));
    }
    REQUIRE_EQ(test::result::errors(), 24);
    test::result::reset_errors();
}