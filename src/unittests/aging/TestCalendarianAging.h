#ifndef _TESTCALENDARIANAGING_
#define _TESTCALENDARIANAGING_

#include <cxxtest/TestSuite.h>

class TestCalendarianAging : public CxxTest::TestSuite
{
    public:
    void testNoAging();
    void testAgingCalculation();
    void testReset();
    void testFormulaVariables();
};

#endif
