#ifndef _TESTAGINGSTATE_
#define _TESTAGINGSTATE_

#include "../../state/aging_state.h"
#include <cxxtest/TestSuite.h>

class TestAgingState : public CxxTest::TestSuite
{
    public:
    void testCreation();
    void testValueChange();
};

#endif
