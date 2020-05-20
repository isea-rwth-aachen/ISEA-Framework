#ifndef _TESTSYMBOLIC_
#define _TESTSYMBOLIC_

#include <cxxtest/TestSuite.h>


class TestSymbolic : public CxxTest::TestSuite
{
    public:
    void TestSymbolicUnit();
    void TestSymbolicMatrix();
    void TestInvert();
    void TestSolve();

    private:
    protected:
};

#endif
