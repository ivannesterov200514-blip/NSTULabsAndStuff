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

class UnitClass {
public:
    template <typename T>
    void UnitTestModule(T t, const string& str) {
        try {
            t();
            cout << str << " is fine ;)\n";
        } catch (runtime_error& re) {
            cerr << re.what() << " in " << str << endl;
            bTest++;
        }
    }

    ~UnitClass() {
        cerr << "Number of bugs: " << bTest << endl;
        if (bTest > 0) exit(1);
    }

private:
    int bTest = 0;
};

void UnitTests() {
    UnitClass unitClass;
    unitClass.UnitTestModule(TestPlus, "Block 1");
    unitClass.UnitTestModule(TestNeg, "Block 2");
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
    cout << "soska" << endl;
    return 0;
}