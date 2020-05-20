#ifndef _TESTCYCLICALAGING_
#define _TESTCYCLICALAGING_

#include <cxxtest/TestSuite.h>

class TestCyclicalAging : public CxxTest::TestSuite
{
    public:
    void testNoAging();
    void testAgingCalculation();
    void testReset();
};

#endif
