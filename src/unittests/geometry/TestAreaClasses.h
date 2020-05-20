#ifndef _TESTAREACLASSES_
#define _TESTAREACLASSES_

#include <cxxtest/TestSuite.h>
#include "../../geometry/plane.h"
#include "../../geometry/geometry2D.h"
#include "../../geometry/area.h"
#include "../../geometry/innerArea.h"
#include "../../geometry/block_geometry.h"
#include "../../cinterface/Geometry2DTest.h"



class TestAreaClasses: public CxxTest::TestSuite
{
    public:
        void TestEasyPlane();
        void TestComplicatedPlane();
        void TestGeometry2D();
        void TestGeometry2DWithArcs();
        void TestArea();
        void TestAreaWithArcs();
        void TestInnerAreas();
        void TestBlockGeometry();
		void TestSimulinkUnitTest();

    private:
    protected:
};

#endif
