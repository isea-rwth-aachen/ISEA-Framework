#ifndef _TESTTHERMAL_ELEMENT_
#define _TESTTHERMAL_ELEMENT_

#include <cstring>
#include <cxxtest/TestSuite.h>
#include <boost/scoped_ptr.hpp>
#include "../../thermal/blocks/elements/thermal_element.h"
#include "../../thermal/blocks/elements/rectangular_element.h"
#include "../../thermal/blocks/elements/cylindric_element.h"
#include "../../thermal/blocks/elements/cutcyl_element.h"
#include "../../thermal/blocks/elements/triangular_element.h"


class TestThermalElements: public CxxTest::TestSuite
{
    public:
        void TestThermalElement();
        void TestRectangularElement();
        void TestCylindricElement();
        void TestCutCylElement();
        void TestTriangularElement();

    private:
    protected:
};

#endif
