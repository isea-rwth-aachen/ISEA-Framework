#ifndef _TESTBATTERY_
#define _TESTBATTERY_

#include <cxxtest/TestSuite.h>
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/cxx11/is_permutation.hpp>
#include <cstring>
#include "../../geometry/area.h"
#include "../../thermal/thermal_model.h"
#include "../../thermal/blocks/rectangular_block.h"
#include "../../thermal/boundaryConditions/cooling.h"
#include "../../thermal/boundaryConditions/cooling_block.h"
#include "../../thermal/thermal_structs.h"



class TestThermalModels: public CxxTest::TestSuite
{
    public:
        void TestThermalModel();
        void TestCharacteristicLengthCalculation();
        void TestCharacteristicLengthCalculationAdvancedAggregation();

    private:
    protected:
};

#endif
