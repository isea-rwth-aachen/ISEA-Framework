#ifndef _TESTCOOLINGFUNCTIONS_
#define _TESTCOOLINGFUNCTIONS_

#include <cxxtest/TestSuite.h>
#include <boost/algorithm/cxx11/is_permutation.hpp>
#include <cstring>
#include "../../thermal/boundaryConditions/default_convection.h"
#include "../../thermal/boundaryConditions/cooling_block.h"
#include "../../thermal/boundaryConditions/cooling.h"



class TestCoolingFunctions: public CxxTest::TestSuite
{
    public:
        void TestConvectionByFormula();
        void TestConvectionByLookUp();
        void TestDirichletBoundaryCondition();
        void TestRadiation();
        void TestCoolingByLookUp();
        void TestCoolingByConstantValue();
        void TestCoolingHorizontalPlane();
        void TestCoolingVerticalPlane();
        void TestCoolingPrismatic();

    private:
    protected:
};

#endif
