#include <iostream>
#include <sstream>
#include <assert.h>

using namespace std;

int sum(int x, int y) {
    return x + y * 2;
}

template <typename T, typename N>
void Check(const T& t, const N& n, const string& funcName) {
    if (t != n) {
        ostringstream ss;
        ss  << "Assertion failed " << t << " != " << n << " Where: " << funcName;
        //cout << "Assertion failed! " << t << "!=" << n;

        throw runtime_error(ss.str());
    }
}

void TestPlus() {
    Check(sum(2,2), 4, "from TestPlus");
}

void TestNeg() {
    Check(sum(-2,-2), -4, "from TestNeg");
}

template <typename T>
void UnitTestModule(T t, const string& str) {
    try {
        t();
    } catch (runtime_error& re) {
        cerr << re.what() << " in " << str << endl;
    }
}

void UnitTests() {
    UnitTestModule(TestPlus, "Block 1");
    UnitTestModule(TestNeg, "Block 2");
}

int main() {
    // assert(sum(2, 2) == 4);
    // try {
    //     TestPlus();
    // } catch (runtime_error& re) {
    //     cerr << re.what() << endl;
    // }
    
    // try {
    //     TestNeg();
    // } catch (runtime_error& re) {
    //     cerr << re.what() << endl;
    // }

    UnitTests();

    return 0;
}