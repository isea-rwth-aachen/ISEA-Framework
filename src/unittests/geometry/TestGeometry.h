#ifndef _TESTGEOMETRY_
#define _TESTGEOMETRY_

#include <cxxtest/TestSuite.h>
#include "../../geometry/angle.h"
#include "../../geometry/cartesian.h"
#include "../../geometry/cylindric.h"
#include "../../geometry/two_dim.h"
#include "../../geometry/unit_vector.h"
#include "../../geometry/tolerance.h"



class TestGeometry: public CxxTest::TestSuite
{
    public:
        void TestAngle();
        void TestCartesian();
        void TestUnitVector();
        void TestTwoDim();
        void TestCylindric();
        void TestTolerance();

    private:
    protected:
};

#endif
