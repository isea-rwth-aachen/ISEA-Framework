
#ifndef _TESTAGINGTWOPORT_
#define _TESTAGINGTWOPORT_

#include "../../state/aging_state.h"
#include <cxxtest/TestSuite.h>

class TestAgingTwoPort : public CxxTest::TestSuite
{
    public:
    void testFactorCalculation();

    private:
    void assertAllVariables( const boost::shared_ptr< state::AgingState > &agingState, double capacityFactor,
                             double socOffset, double resistanceFactor );

    public:
};

#endif
