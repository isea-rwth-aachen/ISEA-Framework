#ifndef _TESTSTATE_
#define _TESTSTATE_

#include <cstring>
#include <cxxtest/TestSuite.h>
#include <set>
#include "../../states/thermal_state.h"



class TestState: public CxxTest::TestSuite
{
    public:
        void TestThermalState();

    private:
    protected:
};

#endif
