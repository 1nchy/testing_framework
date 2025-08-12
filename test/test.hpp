#ifndef _ICY_TESTING_FRAMEWORK_TEST_HPP_
#define _ICY_TESTING_FRAMEWORK_TEST_HPP_

#include <string>
#include <set>
#include <vector>
#include <concepts>
#include <utility>
#include <tuple>
#include <cassert>
#include <iostream>

#define __ICY_STR(...) #__VA_ARGS__
#define ICY_STR(...) __ICY_STR(__VA_ARGS__)
#define __ICY_CAT(x, y) x##y
#define ICY_CAT(x, y) __ICY_CAT(x, y)
// #ifdef __COUNTER__
// #define ICY_ANONYMOUS(x) ICY_CAT(x, __COUNTER__)
// #else // __COUNTER__
#define ICY_ANONYMOUS(x) ICY_CAT(x, __LINE__)
// #endif // __COUNTER__

#define ICY_ATTRIBUTE_CONSTRUCTOR __attribute__((constructor))
#define ICY_ATTRIBUTE_DESTRUCTOR __attribute__((destructor))

#define GLOBAL_EVAL(...) \
    const int ICY_ANONYMOUS(ICY_ANONYMOUS_VAR_) = __VA_ARGS__

namespace test {

enum bracket : unsigned { PARENTHESES, SQUARE, CURLY, ANGLE, NONE };
constexpr std::pair<char, char> brackets[bracket::NONE] = {{'(', ')'}, {'[', ']'}, {'{', '}'}, {'<', '>'}};
#if defined(_MSC_VER)
constexpr std::string keywords[] = {"class", "struct", "enum", "union"};
#endif
enum level : unsigned { INFO, WARNING, ERROR, FATAL };
struct fatal_exception {};

namespace __details__ {
struct arguments_stringify;
struct arguments_stringify {
    template <typename _T, typename... _Ts> static auto append(std::string&, _T&&, _Ts&&...) -> void;
    static auto append(std::string&) -> void;
};
}

namespace {
template <typename _Begin, typename _End> concept iterable_range = requires (_Begin _b, _End _e) {
    {++_b}; {*_b}; requires !std::is_void<decltype(*_b)>::value && std::is_same<_Begin, _End>::value; {_b != _e};
};
template <typename _T> concept forward_iterable = !std::is_array<_T>::value &&
requires (const _T& _obj) {
    {std::begin(_obj)}; {std::end(_obj)};
    requires iterable_range<decltype(std::begin(_obj)), decltype(std::end(_obj))>;
} && !requires (const _T& _obj) {
    {std::rbegin(_obj)}; {std::rend(_obj)};
    requires iterable_range<decltype(std::rbegin(_obj)), decltype(std::rend(_obj))>;
};
template <typename _T> concept bidirectional_iterable = std::is_array<_T>::value || (
    requires (const _T& _obj) {
        {std::begin(_obj)}; {std::end(_obj)};
        requires iterable_range<decltype(std::begin(_obj)), decltype(std::end(_obj))>;
    }
) && (
    requires (const _T& _obj) {
        {std::rbegin(_obj)}; {std::rend(_obj)};
        requires iterable_range<decltype(std::rbegin(_obj)), decltype(std::rend(_obj))>;
    }
);
}

template <typename _T = void> auto to_string() -> std::string {
    // GCC 9.2.0 // std::string test::to_string() [with _T = A; std::string = std::__cxx11::basic_string]
    // GCC 8.3.0 // std::__cxx11::string test::to_string() [with _T = A; std::__cxx11::string = std::__cxx11::basic_string]
    // CLANG 7.0.1 // std::string to_string() [_T = A]
#if defined(_MSC_VER)
    std::string _s = __FUNCSIG__;
#elif defined(__clang__) || defined(__GNUC__)
    std::string _s = __PRETTY_FUNCTION__;
#else
    static_assert(false, "unknown compiler");
#endif
#if defined(_MSC_VER) // class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > __cdecl to_string<class A>(void)
    std::string::size_type _begin = _s.find("to_string") + 10;
    std::string::size_type _end = _s.size() - 7;
    _s = _s.substr(_begin, _end - _begin);
    for (const auto& _word : keywords) {
        if (_s.starts_with(_word)) {
            _s = _s.substr(_word.size() + 1);
            break;
        }
    }
#elif defined(__clang__) // std::string to_string() [_T = A]
    std::string::size_type _begin = _s.find('=') + 2;
    std::string::size_type _end = _s.size() - 1;
    _s = _s.substr(_begin, _end - _begin);
#elif defined(__GNUC__) // std::string to_string() [with _T = A; std::string = std::__cxx11::basic_string]
    std::string::size_type _begin = _s.find('=') + 2;
    std::string::size_type _end = _s.find(';');
    _s = _s.substr(_begin, _end - _begin);
#else
    static_assert(false);
#endif
    return _s.append("(...)");
}
auto to_string(bool _b) -> std::string { return _b ? "true" : "false"; }
auto to_string(char _c) -> std::string { return std::string{'\'', _c, '\''}; }
auto to_string(const char* _s) -> std::string { return "\"" + std::string(_s) + "\""; }
auto to_string(std::nullptr_t) -> std::string { return "nullptr"; }
auto to_string(const std::string& _s) -> std::string { return "\"" + _s + "\""; }
template <> auto to_string<void>() -> std::string { return ""; }
template <std::integral _T> auto to_string(const _T& _t) -> std::string { return std::to_string(_t); }
template <std::floating_point _T> auto to_string(const _T& _t) -> std::string { return std::to_string(_t); }
template <bracket _B, typename... _Args> auto to_string(_Args&&... _args) -> std::string;
template <typename _K1, typename _K2> auto to_string(const std::pair<_K1, _K2>& _p) -> std::string;
template <typename... _Ts> auto to_string(const std::tuple<_Ts...>& _t) -> std::string;
template <bidirectional_iterable _T> auto to_string(const _T& _t) -> std::string;
template <forward_iterable _T> auto to_string(const _T& _t) -> std::string;
template <typename _T> auto to_string(const _T&) -> std::string;
template <bidirectional_iterable _T> auto to_string(const _T& _t) -> std::string {
    std::string _s("[");
    for (auto _i = std::begin(_t); _i != std::end(_t); _s.push_back(',')) {
        _s += to_string(*_i);
        if (++_i == std::end(_t)) { break; }
    }
    _s.append("]");
    return _s;
}
template <forward_iterable _T> auto to_string(const _T& _t) -> std::string {
    std::string _s("{");
    for (auto _i = std::begin(_t); _i != std::end(_t); _s.push_back(',')) {
        _s += to_string(*_i);
        if (++_i == std::end(_t)) { break; }
    }
    _s.append("}");
    return _s;
}
template <typename _K1, typename _K2> auto to_string(const std::pair<_K1, _K2>& _p) -> std::string {
    return test::to_string<PARENTHESES>(_p.first, _p.second);
}
namespace {
template <size_t... _N, typename... _Ts> auto tuple_to_string(std::index_sequence<_N...>, const std::tuple<_Ts...>& _t) -> std::string {
    return test::to_string<PARENTHESES>(std::get<_N>(_t)...);
};
}
template <typename... _Ts> auto to_string(const std::tuple<_Ts...>& _t) -> std::string {
    return tuple_to_string(std::make_index_sequence<sizeof...(_Ts)>{}, _t);
}
template <bracket _B, typename... _Args> auto to_string(_Args&&... _args) -> std::string {
    std::string _s;
    if constexpr (_B != bracket::NONE) { _s.push_back(brackets[_B].first); }
    __details__::arguments_stringify::append(_s, std::forward<_Args>(_args)...);
    if constexpr (_B != bracket::NONE) { _s.push_back(brackets[_B].second); }
    return _s;
};

template <typename _T> auto to_string(const _T& _t) -> std::string {
    return to_string<_T>();
}

namespace __details__ {
template <typename _T, typename... _Ts> auto arguments_stringify::append(std::string& _s, _T&& _t, _Ts&&... _ts) -> void {
    _s.append(to_string(std::forward<_T>(_t)));
    if (sizeof...(_Ts) != 0) { _s.push_back(','); }
    arguments_stringify::append(_s, std::forward<_Ts>(_ts)...);
}
auto arguments_stringify::append(std::string& _s) -> void {}
}

struct subcase;
struct subcase_signature;
struct testcase;
struct result;
using function_type = void(*)();

struct subcase_signature {
public:
    subcase_signature(const char* _label, const char* _file, unsigned _line);
    subcase_signature(const subcase_signature&) = default;
    subcase_signature(subcase_signature&&) = default;
    subcase_signature& operator=(const subcase_signature&) = default;
    subcase_signature& operator=(subcase_signature&&) = default;
    virtual ~subcase_signature() = default;
    auto operator==(const subcase_signature&) const -> bool = default;
    auto operator!=(const subcase_signature&) const -> bool = default;
private:
    std::string _label;
    std::string _file;
    unsigned _line;
};
struct subcase : public subcase_signature {
public:
    subcase(const char* _label, const char* _file, unsigned _line);
    subcase(const subcase&) = delete;
    subcase(subcase&&) = delete;
    subcase& operator=(const subcase&) = delete;
    subcase& operator=(subcase&&) = delete;
    virtual ~subcase();
    operator bool() const;
public:
    static auto& cases() { return _subcases; }
    static auto build() -> void;
    static auto done() { return _next_subcases.empty(); }
    static auto enter(const subcase_signature&) -> void;
    static auto leave(const subcase_signature&) -> void;
    static auto depth() -> unsigned { return _depth; }
    static auto begin() -> void;
    static auto end() -> void;
private:
    bool _entered;
    inline static std::vector<subcase_signature> _subcases;
    inline static std::vector<subcase_signature> _next_subcases;
    inline static unsigned _depth;
    inline static bool _choose_next;
};
struct testcase {
public:
    testcase(function_type, const char*, const char*, unsigned);
    testcase(const testcase&) = default;
    testcase(testcase&&) = delete;
    testcase& operator=(const testcase&) = delete;
    testcase& operator=(testcase&&) = delete;
    ~testcase() = default;
    auto operator<(const testcase&) const -> bool;
    auto operator()() const -> void;
public:
    static auto enroll(function_type, const char*, const char*, unsigned) -> int;
    static auto& cases() { return _cases; }
private:
    const function_type _function;
    const std::string _label;
    const std::string _file;
    const unsigned _line;
private:
    inline static std::set<testcase> _cases;
    inline static std::vector<subcase_signature> _subcases;
    inline static std::vector<subcase_signature> _next_subcases;
    inline static unsigned _depth; // = 0
    inline static bool _choose_next;
};
namespace {
enum binary_type { EQ, NE, GT, LT, GE, LE };
template <binary_type _Ct, typename _L, typename _R> struct binary {
    auto operator()(const _L&, const _R&) const -> bool;
};
#define STRUCT_BINARY(BT, OP) \
    template <typename _L, typename _R> struct binary<BT, _L, _R> { \
        auto operator()(const _L& _lhs, const _R& _rhs) const -> bool { return _lhs OP _rhs; } \
    }
STRUCT_BINARY(EQ, ==);
STRUCT_BINARY(NE, !=);
STRUCT_BINARY(GT, >);
STRUCT_BINARY(LT, <);
STRUCT_BINARY(GE, >=);
STRUCT_BINARY(LE, <=);
enum unary_type { TRUE, FALSE };
template <unary_type _Ut, typename _T> struct unary { auto operator()(const _T&) const -> bool; };
#define STRUCT_UNARY(UT, OP) \
    template <typename _T> struct unary<UT, _T> { \
        auto operator()(const _T& _t) const -> bool { return OP _t; } \
    }
STRUCT_UNARY(TRUE, );
STRUCT_UNARY(FALSE, !);
}
struct result {
public:
    result(const char*, unsigned, level, const char*, const char*, const char*, const char*);
public:
    template <unary_type _Ut, typename _T> auto unary_assert(const _T&) -> result&;
    template <binary_type _Bt, typename _L, typename _R> auto binary_assert(const _L&, const _R&) -> result&;
    template <binary_type _Bt, size_t _I, typename _L, typename _R>
    auto sequence_assert(const _L&, const _R&) -> result&;
    template <binary_type _Bt, size_t _I, typename _L, typename _R, typename... _Args>
    auto sequence_assert(const _L&, const _R&, _Args&&... _args) -> result&;
    template <typename _Ex> auto exception_assert(const _Ex&) -> result&; // threw an exception
    auto exception_failed() -> result&; // threw an exception
    auto noexception_failed() -> result&; // not threw any exception
public:
    static auto errors() -> size_t { return _errors; }
    static auto fatals() -> size_t { return _fatals; }
    static auto reset_errors() -> void { _errors = 0; }
private:
    // auto get_exception_type() const -> std::string;
    auto get_exception_message() const -> std::string;
    auto report() const -> void;
    auto count() -> void;
private:
    std::string _file;
    unsigned _line;
    level _level;
    std::string _assertion;
    std::string _expr;
    bool _failed;
    bool _threw;
    std::string _exception_type;
    std::string _exception_message;
private:
    inline static size_t _errors = 0;
    inline static size_t _fatals = 0;
};


subcase_signature::subcase_signature(const char* _label, const char* _file, unsigned _line) :
_label(_label), _file(_file), _line(_line) {}

subcase::subcase(const char* _label, const char* _file, unsigned _line) :
subcase_signature(_label, _file, _line) {
    enter(*this);
    _entered = depth() <= cases().size() && this->operator==(cases().at(depth() - 1));
}
subcase::~subcase() {
    leave(*this);
}
subcase::operator bool() const {
    return _entered;
}

auto subcase::build() -> void {
    _subcases.clear();
    // _subcases.emplace_back("", __FILE__, 0u);
}
auto subcase::enter(const subcase_signature& _sc) -> void {
    ++_depth; // depth of `_sc`
    // assert(_depth < _unfinished.size());
    if (_subcases.empty() && _depth > _next_subcases.size()) {
        _next_subcases.push_back(_sc);
    }
    if (_choose_next) {
        // _next_subcases.clear();
        _next_subcases.assign(_subcases.cbegin(), _subcases.cbegin() + _depth - 1);
        _next_subcases.push_back(_sc);
        _choose_next = false;
    }
    if (_depth <= _subcases.size() && _subcases.at(_depth - 1) == _sc) {
        _next_subcases.clear();
        _choose_next = true;
    }
}
auto subcase::leave(const subcase_signature& _sc) -> void {
    --_depth;
}
auto subcase::begin() -> void {
    _subcases.clear();
    _subcases.swap(_next_subcases);
    _choose_next = false;
}
auto subcase::end() -> void {}

testcase::testcase(function_type _function, const char* _label, const char* _file, unsigned _line) :
_function(_function), _label(_label), _file(_file), _line(_line) {}
auto testcase::operator<(const testcase& _rhs) const -> bool {
    return _line < _rhs._line;
}
auto testcase::operator()() const -> void {
    _function();
}
auto testcase::enroll(function_type _function, const char* _label, const char* _file, unsigned _line) -> int {
    _cases.emplace(_function, _label, _file, _line);
    return 0;
}

result::result(const char* _file, unsigned _line, level _level, const char* _assertion, const char* _expr,
               const char* _exception_type = "", const char* _exception_message = "") :
_file(_file), _line(_line), _level(_level), _assertion(_assertion), _expr(_expr),
_exception_type(_exception_type), _exception_message(_exception_message) {}
template <unary_type _Ut, typename _T> auto result::unary_assert(const _T& _t) -> result& {
    if (!unary<_Ut, _T>()(_t)) {
        report();
        std::cout << "  " << _assertion << test::to_string<PARENTHESES>(_t) << std::endl;
        count();
    }
    return *this;
}
template <binary_type _Bt, typename _L, typename _R> auto result::binary_assert(const _L& _lhs, const _R& _rhs) -> result& {
    if (!binary<_Bt, _L, _R>()(_lhs, _rhs)) {
        report();
        std::cout << "  " << _assertion << test::to_string<PARENTHESES>(_lhs, _rhs) << std::endl;
        count();
    }
    return *this;
}
template <binary_type _Bt, size_t _I, typename _L, typename _R> auto result::sequence_assert(const _L& _lhs, const _R& _rhs) -> result& {
    if (!binary<_Bt, _L, _R>()(_lhs, _rhs)) {
        report();
        std::cout << "  " << _assertion << test::to_string<ANGLE>(_I, _I + 1) << test::to_string<PARENTHESES>(_lhs, _rhs) << std::endl;
        count();
    }
    return *this;
}
template <binary_type _Bt, size_t _I, typename _L, typename _R, typename... _Args> auto
result::sequence_assert(const _L& _lhs, const _R& _rhs, _Args&&... _args) -> result& {
    if (!binary<_Bt, _L, _R>()(_lhs, _rhs)) {
        report();
        std::cout << "  " << _assertion << test::to_string<ANGLE>(_I, _I + 1) << test::to_string<PARENTHESES>(_lhs, _rhs) << std::endl;
        count();
    }
    return sequence_assert<_Bt, _I + 1>(_rhs, std::forward<_Args>(_args)...);
}
template <typename _Ex> auto result::exception_assert(const _Ex& _e) -> result& {
    const std::string& _msg = get_exception_message();
    if (_exception_message != _msg) {
        report();
        std::cout << "  threw with " << test::to_string<PARENTHESES>(_msg) << " expected " << test::to_string<PARENTHESES>(_exception_message) << std::endl;
        count();
    }
    return *this;
}
auto result::exception_failed() -> result& {
    report();
    std::cout << "  threw " << test::to_string<PARENTHESES>(get_exception_message()) << " expected " << test::to_string<PARENTHESES>(_exception_type) << std::endl;
    count();
    return *this;
}
auto result::noexception_failed() -> result& {
    report();
    std::cout << "  threw " << test::to_string<PARENTHESES>() << " expected " << test::to_string<PARENTHESES>(_exception_type) << std::endl;
    count();
    return *this;
}

// auto result::get_exception_type() const -> std::string {}
auto result::get_exception_message() const -> std::string {
    try { throw; }
    catch (const std::exception& _e) { return _e.what(); }
    catch (const std::string& _e) { return _e; }
    catch (const char* _e) { return _e; }
    catch (...) { return "unknown exception"; }
    return "";
}
auto result::report() const -> void {
    std::cout << _file << '(' << _line << ')' << ' ' << "FAILED!" << '\n';
    std::cout << "  " << _assertion << '(' << _expr << ')' << std::endl;
}
auto result::count() -> void {
    switch (_level) {
    case level::ERROR: { ++_errors; break; }
    case level::FATAL: { ++_fatals; break; }
    default: {}
    }
    if (_level == level::FATAL) {
        throw fatal_exception();
    }
}

}

#define __ICY_REGISTER_CASE(f, label) \
    GLOBAL_EVAL(test::testcase::enroll(&f, label, __FILE__, __LINE__))

#define __ICY_DECLARE_AND_REGISTER_CASE(f, label) \
    static void f(); \
    __ICY_REGISTER_CASE(f, label); \
    static void f()

#define __BEGIN do
#define __END while (false)

// assert unary expression, may fail
#define __UNARY_ASSERTION_IMPL(assertion, level, type, ...) \
    test::result(__FILE__, __LINE__, level, assertion, ICY_STR(__VA_ARGS__)).unary_assert<type>(__VA_ARGS__)
// assert binary expression, may fail
#define __BINARY_ASSERTION_IMPL(assertion, level, type, ...) \
    test::result(__FILE__, __LINE__, level, assertion, ICY_STR(__VA_ARGS__)).binary_assert<type>(__VA_ARGS__)
// assert sequence expression, may fail
#define __SEQUENTIAL_ASSERTION_IMPL(assertion, level, type, ...) \
    test::result(__FILE__, __LINE__, level, assertion, ICY_STR(__VA_ARGS__)).sequence_assert<type, 0>(__VA_ARGS__)
// assert exception expression, may fail
#define __EXCEPTION_ASSERTION_IMPL(assertion, level, exception, message, e, ...) \
    test::result(__FILE__, __LINE__, level, assertion, ICY_STR(__VA_ARGS__), ICY_STR(exception), message).exception_assert(e)

// failed, exception handler
#define __EXCEPTION_FAILURE_IMPL(assertion, level, exception, ...) \
    test::result(__FILE__, __LINE__, level, assertion, ICY_STR(__VA_ARGS__), ICY_STR(exception)).exception_failed()
// failed, noexception handler
#define __NOEXCEPTION_FAILURE_IMPL(assertion, level, exception, ...) \
    test::result(__FILE__, __LINE__, level, assertion, ICY_STR(__VA_ARGS__), ICY_STR(exception)).noexception_failed()

#define __UNARY_ASSERTION(assertion, level, type, ...) \
    __BEGIN { __UNARY_ASSERTION_IMPL(assertion, level, type, __VA_ARGS__); } __END
#define __BINARY_ASSERTION(assertion, level, type, ...) \
    __BEGIN { __BINARY_ASSERTION_IMPL(assertion, level, type, __VA_ARGS__); } __END
#define __SEQUENTIAL_ASSERTION(assertion, level, type, ...) \
    __BEGIN { __SEQUENTIAL_ASSERTION_IMPL(assertion, level, type, __VA_ARGS__); } __END
#define __NOTHROWN_ASSERTION(assertion, level, ...) \
    __BEGIN { \
        try { __VA_ARGS__; } \
        catch (...) { __EXCEPTION_FAILURE_IMPL(assertion, level, __VA_ARGS__); } \
    } __END
#define __THROWN_ASSERTION(assertion, level, exception, ...) \
    __BEGIN { \
        try { __VA_ARGS__; } catch (const exception& e) { break; } \
        catch (...) { __EXCEPTION_FAILURE_IMPL(assertion, level, exception, __VA_ARGS__); } \
        __NOEXCEPTION_FAILURE_IMPL(assertion, level, exception, __VA_ARGS__); \
    } __END
#define __THROWN2_ASSERTION(assertion, level, exception, message, ...) \
    __BEGIN { \
        try { __VA_ARGS__; } catch (const exception& e) { \
            __EXCEPTION_ASSERTION_IMPL(assertion, level, exception, message, e, __VA_ARGS__); break; \
        } catch (...) { __EXCEPTION_FAILURE_IMPL(assertion, level, exception, __VA_ARGS__); } \
        __NOEXCEPTION_FAILURE_IMPL(assertion, level, exception, __VA_ARGS__); \
    } __END

#define ICY_CASE(label) \
    __ICY_DECLARE_AND_REGISTER_CASE(ICY_ANONYMOUS(ICY_ANONYMOUS_CASE_), label)

#define ICY_SUBCASE(label) \
    if (const test::subcase& ICY_ANONYMOUS(ICY_ANONYMOUS_SUBCASE_) = test::subcase(label, __FILE__, __LINE__))

#define ICY_SEQCASE(label)

#define EXPECT_TRUE(expression) __UNARY_ASSERTION("EXPECT_TRUE", test::level::ERROR, test::TRUE, expression)
#define EXPECT_FALSE(expression) __UNARY_ASSERTION("EXPECT_FALSE", test::level::ERROR, test::FALSE, expression)
#define EXPECT_EQ(x, y) __BINARY_ASSERTION("EXPECT_EQ", test::level::ERROR, test::EQ, x, y)
#define EXPECT_NE(x, y) __BINARY_ASSERTION("EXPECT_NE", test::level::ERROR, test::NE, x, y)
#define EXPECT_GT(x, y) __BINARY_ASSERTION("EXPECT_GT", test::level::ERROR, test::GT, x, y)
#define EXPECT_LT(x, y) __BINARY_ASSERTION("EXPECT_LT", test::level::ERROR, test::LT, x, y)
#define EXPECT_GE(x, y) __BINARY_ASSERTION("EXPECT_GE", test::level::ERROR, test::GE, x, y)
#define EXPECT_LE(x, y) __BINARY_ASSERTION("EXPECT_LE", test::level::ERROR, test::LE, x, y)

#define EXPECT_INCR(...) __SEQUENTIAL_ASSERTION("EXPECT_INCR", test::level::ERROR, test::LE, __VA_ARGS__)
#define EXPECT_STRICT_INCR(...) __SEQUENTIAL_ASSERTION("EXPECT_STRICT_INCR", test::level::ERROR, test::LT, __VA_ARGS__)
#define EXPECT_DECR(...) __SEQUENTIAL_ASSERTION("EXPECT_DECR", test::level::ERROR, test::GE, __VA_ARGS__)
#define EXPECT_STRICT_DECR(...) __SEQUENTIAL_ASSERTION("EXPECT_STRICT_DECR", test::level::ERROR, test::GT, __VA_ARGS__)
#define EXPECT_EQUALS(...) __SEQUENTIAL_ASSERTION("EXPECT_EQUALS", test::level::ERROR, test::EQ, __VA_ARGS__)

#define EXPECT_NOTHROW(...) __NOTHROWN_ASSERTION("EXPECT_NOTHROW", test::level::ERROR, __VA_ARGS__)
#define EXPECT_THROW(exception, ...) __THROWN_ASSERTION("EXPECT_THROW", test::level::ERROR, exception, __VA_ARGS__)
#define EXPECT_THROW_WITH(exception, message, ...) __THROWN2_ASSERTION("EXPECT_THROW_WITH", test::level::ERROR, exception, message, __VA_ARGS__)

#define REQUIRE_TRUE(expression) __UNARY_ASSERTION("REQUIRE_TRUE", test::level::FATAL, test::TRUE, expression)
#define REQUIRE_FALSE(expression) __UNARY_ASSERTION("REQUIRE_FALSE", test::level::FATAL, test::FALSE, expression)
#define REQUIRE_EQ(x, y) __BINARY_ASSERTION("REQUIRE_EQ", test::level::FATAL, test::EQ, x, y)
#define REQUIRE_NE(x, y) __BINARY_ASSERTION("REQUIRE_NE", test::level::FATAL, test::NE, x, y)
#define REQUIRE_GT(x, y) __BINARY_ASSERTION("REQUIRE_GT", test::level::FATAL, test::GT, x, y)
#define REQUIRE_LT(x, y) __BINARY_ASSERTION("REQUIRE_LT", test::level::FATAL, test::LT, x, y)
#define REQUIRE_GE(x, y) __BINARY_ASSERTION("REQUIRE_GE", test::level::FATAL, test::GE, x, y)
#define REQUIRE_LE(x, y) __BINARY_ASSERTION("REQUIRE_LE", test::level::FATAL, test::LE, x, y)

#define REQUIRE_INCR(...) __SEQUENTIAL_ASSERTION("REQUIRE_INCR", test::level::FATAL, test::LE, __VA_ARGS__)
#define REQUIRE_STRICT_INCR(...) __SEQUENTIAL_ASSERTION("REQUIRE_STRICT_INCR", test::level::FATAL, test::LT, __VA_ARGS__)
#define REQUIRE_DECR(...) __SEQUENTIAL_ASSERTION("REQUIRE_DECR", test::level::FATAL, test::GE, __VA_ARGS__)
#define REQUIRE_STRICT_DECR(...) __SEQUENTIAL_ASSERTION("REQUIRE_STRICT_DECR", test::level::FATAL, test::GT, __VA_ARGS__)
#define REQUIRE_EQUALS(...) __SEQUENTIAL_ASSERTION("REQUIRE_EQUALS", test::level::FATAL, test::EQ, __VA_ARGS__)

#define REQUIRE_NOTHROW(...) __NOTHROWN_ASSERTION("REQUIRE_NOTHROW", test::level::FATAL, __VA_ARGS__)
#define REQUIRE_THROW(exception, ...) __THROWN_ASSERTION("REQUIRE_THROW", test::level::FATAL, exception, __VA_ARGS__)
#define REQUIRE_THROW_WITH(exception, message, ...) __THROWN2_ASSERTION("REQUIRE_THROW_WITH", test::level::FATAL, exception, message, __VA_ARGS__)

int main(int _argc, char** _argv) {
    try {
        for (const auto& _c : test::testcase::cases()) {
            test::subcase::build();
            do {
                test::subcase::begin();
                _c.operator()();
                test::subcase::end();
            } while (!test::subcase::done());
        }
        return test::result::errors();
    }
    catch (const test::fatal_exception& _e) {
        return test::result::fatals();
    }
    catch (const std::exception& _e) {
        std::cerr << _e.what() << std::endl;
    }
    catch (const std::string& _e) {
        std::cerr << _e << std::endl;
    }
    catch (const char* _e) {
        std::cerr << _e << std::endl;
    }
    catch (...) {
        std::cerr << "unknown exception" << std::endl;
    }
    return EXIT_FAILURE;
}

#endif // _ICY_TESTING_FRAMEWORK_TEST_HPP_