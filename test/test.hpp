#ifndef _ICY_TESTING_FRAMEWORK_TEST_HPP_
#define _ICY_TESTING_FRAMEWORK_TEST_HPP_

#include <string>
#include <set>
#include <queue>
#include <stack>
#include <vector>
#include <cassert>
#include <iostream>

#define __ICY_CAT(x, y) x##y
#define ICY_CAT(x, y) __ICY_CAT(x, y)
// #ifdef __COUNTER__
// #define ICY_ANONYMOUS(x) ICY_CAT(x, __COUNTER__)
// #else // __COUNTER__
#define ICY_ANONYMOUS(x) ICY_CAT(x, __LINE__)
// #endif // __COUNTER__

#define ICY_ATTRIBUTE_CONSTRUCTOR __attribute__((constructor))
#define ICY_ATTRIBUTE_DESTRUCTOR __attribute__((destructor))

#define ICY_CONSTEXPR(...) \
    const int ICY_ANONYMOUS(ICY_ANONYMOUS_VAR_) = __VA_ARGS__

namespace {

template <typename _T> auto to_string() -> std::string {
    std::string _s = __PRETTY_FUNCTION__; // std::string {anonymous}::to_string() [with _T = A; std::string = std::__cxx11::basic_string<char>]
    std::string::size_type _begin = _s.find('=') + 2;
    std::string::size_type _end = _s.find(';');
    return _s.substr(_begin, _end - _begin);
}
template <> auto to_string<void>() -> std::string { return ""; }
auto to_string(bool _b) -> std::string { return _b ? "true" : "false"; }
auto to_string(char _c) -> std::string {
    return std::string{'\'', _c, '\''};
}
template <typename _T> auto to_string(_T) -> std::string;
template <> auto to_string<std::nullptr_t>(std::nullptr_t) -> std::string { return "nullptr"; }
template <> auto to_string<std::string>(std::string _s) -> std::string {
    _s.reserve(_s.size() + 2);
    _s.insert(0, 1, '\"');
    _s.push_back('\"');
    return _s;
}
template <> auto to_string<const char*>(const char* _s) -> std::string {
    return to_string(std::string(_s));
}
template <std::integral _T> auto to_string(_T _t) -> std::string { return std::to_string(_t); }
template <typename _T> auto to_string(_T) -> std::string {
    return to_string<_T>();
}

}

namespace test {

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
    testcase& operator=(const testcase&) = default;
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
template <binary_type _Ct, typename _L, typename _R> struct binary { auto operator()(const _L&, const _R&) const -> bool; };
template <typename _L, typename _R> struct binary<EQ, _L, _R> { auto operator()(const _L& _lhs, const _R& _rhs) const -> bool { return _lhs == _rhs; } };
template <typename _L, typename _R> struct binary<NE, _L, _R> { auto operator()(const _L& _lhs, const _R& _rhs) const -> bool { return _lhs != _rhs; } };
template <typename _L, typename _R> struct binary<GT, _L, _R> { auto operator()(const _L& _lhs, const _R& _rhs) const -> bool { return _lhs > _rhs; } };
template <typename _L, typename _R> struct binary<LT, _L, _R> { auto operator()(const _L& _lhs, const _R& _rhs) const -> bool { return _lhs < _rhs; } };
template <typename _L, typename _R> struct binary<GE, _L, _R> { auto operator()(const _L& _lhs, const _R& _rhs) const -> bool { return _lhs >= _rhs; } };
template <typename _L, typename _R> struct binary<LE, _L, _R> { auto operator()(const _L& _lhs, const _R& _rhs) const -> bool { return _lhs <= _rhs; } };
enum unary_type { TRUE, FALSE };
template <unary_type _Ut, typename _T> struct unary { auto operator()(const _T&) const -> bool; };
template <typename _T> struct unary<TRUE, _T> { auto operator()(const _T& _t) const -> bool { return _t; } };
template <typename _T> struct unary<FALSE, _T> { auto operator()(const _T& _t) const -> bool { return !_t; } };
}
struct result {
public:
    result(const char*, unsigned, const char*, const char*, const char*, const char*);
public:
    template <binary_type _Ct, typename _L, typename _R> auto binary_assert(const _L&, const _R&) -> result&;
    template <unary_type _Ut, typename _T> auto unary_assert(const _T&) -> result&;
    template <typename _Ex> auto exception_assert(const _Ex&) -> result&; // threw an exception
    auto exception_failed() -> result&; // threw an exception
    auto noexception_failed() -> result&; // not threw any exception
public:
    static auto errors() -> size_t { return _errors; }
private:
    // auto get_exception_type() const -> std::string;
    auto get_exception_message() const -> std::string;
private:
    std::string _file;
    unsigned _line;
    std::string _assertion;
    std::string _expr;
    bool _failed;
    bool _threw;
    std::string _exception_type;
    std::string _exception_message;
private:
    inline static size_t _errors = 0;
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

result::result(const char* _file, unsigned _line, const char* _assertion, const char* _expr, const char* _exception_type = "", const char* _exception_message = "") :
_file(_file), _line(_line), _assertion(_assertion), _expr(_expr), _exception_type(_exception_type), _exception_message(_exception_message) {}
template <binary_type _Ct, typename _L, typename _R> auto result::binary_assert(const _L& _lhs, const _R& _rhs) -> result& {
    if (!binary<_Ct, _L, _R>()(_lhs, _rhs)) {
        std::cout << _file << "(" << _line << ") FAILED!\n";
        std::cout << "  " << _assertion << "(" << _expr << ")\n";
        std::cout << "with expansion:\n";
        std::cout << "  " << _assertion << "(" << to_string(_lhs) << ", " << to_string(_rhs) << ")" << std::endl;
        ++_errors;
    }
    return *this;
}
template <unary_type _Ut, typename _T> auto result::unary_assert(const _T& _t) -> result& {
    if (!unary<_Ut, _T>()(_t)) {
        std::cout << _file << "(" << _line << ") FAILED!\n";
        std::cout << "  " << _assertion << "(" << _expr << ")\n";
        std::cout << "with expansion:\n";
        std::cout << "  " << _assertion << "(" << to_string(_t) << ")" << std::endl;
        ++_errors;
    }
    return *this;
}
template <typename _Ex> auto result::exception_assert(const _Ex& _e) -> result& {
    const std::string& _msg = get_exception_message();
    if (_exception_message != _msg) {
        std::cout << _file << "(" << _line << ") FAILED!\n";
        std::cout << "  " << _assertion << "(" << _expr << ")\n";
        std::cout << "with expansion:\n";
        std::cout << "  threw with (\"" << _msg << "\") expected (\"" << _exception_message << "\")" << std::endl;
        ++_errors;
    }
    return *this;
}
auto result::exception_failed() -> result& {
    std::cout << _file << "(" << _line << ") FAILED!\n";
    std::cout << "  " << _assertion << "(" << _expr << ")\n";
    std::cout << "with expansion:\n";
    std::cout << "  threw (\"" << get_exception_message() << "\") expected (" << _exception_type << ")" << std::endl;
    ++_errors;
    return *this;
}
auto result::noexception_failed() -> result& {
    std::cout << _file << "(" << _line << ") FAILED!\n";
    std::cout << "  " << _assertion << "(" << _expr << ")\n";
    std::cout << "with expansion:\n";
    std::cout << "  threw (" << ") expected (" << _exception_type << ")" << std::endl;
    ++_errors;
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

}

#define __ICY_REGISTER_CASE(f, label) \
    ICY_CONSTEXPR(test::testcase::enroll(&f, label, __FILE__, __LINE__))

#define __ICY_DECLARE_AND_REGISTER_CASE(f, label) \
    static void f(); \
    __ICY_REGISTER_CASE(f, label); \
    static void f()

#define __BEGIN do
#define __END while (false)

// assert unary expression, may fail
#define __UNARY_ASSERT(assertion, type, ...) \
    test::result(__FILE__, __LINE__, assertion, #__VA_ARGS__).unary_assert<type>(__VA_ARGS__)
// assert binary expression, may fail
#define __BINARY_ASSERT(assertion, type, ...) \
    test::result(__FILE__, __LINE__, assertion, #__VA_ARGS__).binary_assert<type>(__VA_ARGS__)
// assert exception expression, may fail
#define __EXCEPTION_ASSERT(assertion, exception, message, e, ...) \
    test::result(__FILE__, __LINE__, assertion, #__VA_ARGS__, #exception, message).exception_assert(e)

// failed, exception handler
#define __EXCEPTION_FAILED(assertion, exception, ...) \
    test::result(__FILE__, __LINE__, assertion, #__VA_ARGS__, #exception).exception_failed()
// failed, noexception handler
#define __NOEXCEPTION_FAILED(assertion, exception, ...) \
    test::result(__FILE__, __LINE__, assertion, #__VA_ARGS__, #exception).noexception_failed()

#define __EXPECT_UNARY(assertion, type, ...) \
    __BEGIN { __UNARY_ASSERT(assertion, type, __VA_ARGS__); } __END
#define __EXPECT_BINARY(assertion, type, ...) \
    __BEGIN { __BINARY_ASSERT(assertion, type, __VA_ARGS__); } __END
#define __EXPECT_NOTHROW(assertion, ...) \
    __BEGIN { \
        try { __VA_ARGS__; } \
        catch (...) { __EXCEPTION_FAILED(assertion, , __VA_ARGS__); } \
    } __END
#define __EXPECT_THROW(assertion, exception, ...) \
    __BEGIN { \
        try { __VA_ARGS__; } catch (const exception& e) { break; } \
        catch (...) { __EXCEPTION_FAILED(assertion, exception, __VA_ARGS__); } \
        __NOEXCEPTION_FAILED(assertion, exception, __VA_ARGS__); \
    } __END
#define __EXPECT_THROW_WITH(assertion, exception, message, ...) \
    __BEGIN { \
        try { __VA_ARGS__; } catch (const exception& e) { \
            __EXCEPTION_ASSERT(assertion, exception, message, e, __VA_ARGS__); break; \
        } catch (...) { __EXCEPTION_FAILED(assertion, exception, __VA_ARGS__); } \
        __NOEXCEPTION_FAILED(assertion, exception, __VA_ARGS__); \
    } __END

#define ICY_CASE(label) \
    __ICY_DECLARE_AND_REGISTER_CASE(ICY_ANONYMOUS(ICY_ANONYMOUS_CASE_), label)

#define ICY_SUBCASE(label) \
    if (const test::subcase& ICY_ANONYMOUS(ICY_ANONYMOUS_SUBCASE_) = test::subcase(label, __FILE__, __LINE__))

#define EXPECT_TRUE(expression) __EXPECT_UNARY("EXPECT_TRUE", test::TRUE, expression)
#define EXPECT_FALSE(expression) __EXPECT_UNARY("EXPECT_FALSE", test::FALSE, expression)
#define EXPECT_EQ(x, y) __EXPECT_BINARY("EXPECT_EQ", test::EQ, x, y)
#define EXPECT_NE(x, y) __EXPECT_BINARY("EXPECT_NE", test::NE, x, y)
#define EXPECT_GT(x, y) __EXPECT_BINARY("EXPECT_GT", test::GT, x, y)
#define EXPECT_LT(x, y) __EXPECT_BINARY("EXPECT_LT", test::LT, x, y)
#define EXPECT_GE(x, y) __EXPECT_BINARY("EXPECT_GE", test::GE, x, y)
#define EXPECT_LE(x, y) __EXPECT_BINARY("EXPECT_LE", test::LE, x, y)
#define EXPECT_NOTHROW(...) __EXPECT_NOTHROW("EXPECT_NOTHROW", __VA_ARGS__)
#define EXPECT_THROW(exception, ...) __EXPECT_THROW("EXPECT_THROW", exception, __VA_ARGS__)
#define EXPECT_THROW_WITH(exception, message, ...) __EXPECT_THROW_WITH("EXPECT_THROW_WITH", exception, message, __VA_ARGS__)

int main(int _argc, char** _argv) {
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

#endif // _ICY_TESTING_FRAMEWORK_TEST_HPP_