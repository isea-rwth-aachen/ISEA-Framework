#ifndef _TESTRAINFLOW_
#define _TESTRAINFLOW_

#include <cxxtest/TestSuite.h>
#include "../../misc/rainflow.h"

class TestRainflow : public CxxTest::TestSuite
{
    public:
    void TestCycleCounting();

    private:
    void AssertCorrectResult( const std::vector< double > &values, const std::vector< rainflow::Cycle > &expectedCycles );
    static bool CyclesAreEqual( const rainflow::Cycle &left, const rainflow::Cycle &right, double epsilon = 0.0001 );
};

#endif
