/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestParser.h
* Creation Date : 20-01-2015
* Last Modified : Do 26 Mär 2015 11:55:35 CET
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTPARSER_
#define _TESTPARSER_
#include <cxxtest/TestSuite.h>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include "../../xmlparser/xmlparameter.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

/// Test the Parser implementation of GetElement....
class TestParser : public CxxTest::TestSuite
{
public:
    void TestGetElementUnsignedIntVecValue();
    void TestGetElementDoubleVecValue();
    void TestGetElementDoubleVecVecValue();
};
#endif /* _TESTPARSER_ */
