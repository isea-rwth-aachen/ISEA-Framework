/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestRmphn.h
* Creation Date : 29-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTRMN_
#define _TESTRMN_
#include <cxxtest/TestSuite.h>

/// DESCRIPTION
class TestRmphn : public CxxTest::TestSuite
{
    public:
    void testRmphnCreation();
    void testRmphnRCDifference();
    void testRmphnFactoryCreation();
    void testRmphnFactoryCreationSimplification();
    void testRmphnFactoryPower();
};

#endif /* _TESTRMN_ */
