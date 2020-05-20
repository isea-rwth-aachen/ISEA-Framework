#ifndef _TESTANODEOVERHANG_
#define _TESTANODEOVERHANG_

#include <cxxtest/TestSuite.h>

class TestAnodeOverhang : public CxxTest::TestSuite
{
    public:
    void testNoAging();
    void testInitialOffset();
    void testAgingCalculation();
    void testReset();
};

#endif
