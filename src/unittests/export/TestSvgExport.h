#ifndef _TESTSVGEXPORT_
#define _TESTSVGEXPORT_

#include <boost/shared_ptr.hpp>
#include <cxxtest/TestSuite.h>
#include <string>

#include "../../electrical/twoport_withchild.h"

class TestSvgExport : public CxxTest::TestSuite
{
    public:
    TestSvgExport();

    void testSvgExportFailTwoPort();
    void testSvgExportFailStream();
    void testSvgExportStopEvaluateDepth();
    void testSvgExportGetHeightOfTwoPort();
    void testPreamble();
    void testBody();
    void testEnding();

    private:
    std::stringstream mDummyOutstream;

    boost::shared_ptr< electrical::TwoPortWithChild< myMatrixType > > mTestDummy;
};

#endif /* _TESTSVGEXPORT_ */
