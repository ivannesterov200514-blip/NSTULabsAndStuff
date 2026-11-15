#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

class CoutCapture {
public:
    CoutCapture() : old_(std::cout.rdbuf(buf_.rdbuf())) {}
    ~CoutCapture() { std::cout.rdbuf(old_); }
    CoutCapture(const CoutCapture&) = delete;
    CoutCapture& operator=(const CoutCapture&) = delete;

    std::string str() const { return buf_.str(); }

private:
    std::ostringstream buf_;
    std::streambuf* old_;
};

template <class F>
std::string capture_output(F&& f) {
    CoutCapture c;
    std::forward<F>(f)();
    return c.str();
}
