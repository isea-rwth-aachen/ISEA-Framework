#ifndef _TESTHERMALSTRUCTS_
#define _TESTHERMALSTRUCTS_

#include <cxxtest/TestSuite.h>
#include "../../thermal/thermal_structs.h"
#include "../../thermal/jagged_array.h"



class TestThermalStructs: public CxxTest::TestSuite
{
    public:
        void TestStructs();
        void TestJaggedArray();

    private:
    protected:
};

#endif
