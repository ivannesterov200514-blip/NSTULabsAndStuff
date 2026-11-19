#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

using namespace std;

class CoutCapture {
public:
    CoutCapture() : old_(cout.rdbuf(buf_.rdbuf())) {}
    ~CoutCapture() { cout.rdbuf(old_); }
    CoutCapture(const CoutCapture&) = delete;
    CoutCapture& operator=(const CoutCapture&) = delete;

    string str() const { return buf_.str(); }

private:
    ostringstream buf_;
    streambuf* old_;
};

template <class F>
string capture_output(F&& f) {
    CoutCapture c;
    forward<F>(f)();
    return c.str();
}