#ifndef _TESTBLOCK_
#define _TESTBLOCK_

#include <cxxtest/TestSuite.h>
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/cxx11/is_permutation.hpp>
#include <cstring>
#include "../../geometry/area.h"
#include "../../thermal/blocks/rectangular_block.h"
#include "RectangularBlockTester.h"
#include "../../thermal/blocks/quadratic_cell_block.h"
#include "../../thermal/blocks/supercap.h"
#include "../../thermal/blocks/hexagonal_cell_block.h"
#include "../../thermal/blocks/triangular_prism_block.h"


class TestBlocks: public CxxTest::TestSuite
{
    public:
        void TestRectangularBlock();
        void TestQuadraticCellBlock();
        void TestQuadraticCellBlockPhiDivisionEight();
        void TestSupercap();
        void TestHexagonalCellBlock();
        void TestTriangularPrismBlock();
        void TestMultipleThermalStatesInThermalBlock();

    private:
    protected:
};

#endif
