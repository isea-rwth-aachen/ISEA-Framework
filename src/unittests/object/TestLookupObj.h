/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestLookupObj.h
* Creation Date : 31-10-2012
* Last Modified : Fr 21 Feb 2014 12:03:07 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTLOOKUPOBJ_
#define _TESTLOOKUPOBJ_
#include <cxxtest/TestSuite.h>

std::vector< double > Createm1();
std::vector< double > Createf1();
std::vector< double > CreateSoc();
std::vector< double > CreateTemp();
std::vector< double > CreateSOC();
std::vector< std::vector< double > > CreateCd();

class TestLookupObj : public CxxTest::TestSuite
{
    public:
    void testLookupObj1dCreate();
    void testLookupObj1dOperator_linearInterpolation();
    void testLookupObj1dOperator_nearestNeighbour();
    void testLookupObj1dOperator_constantUntilThreshold();
    void testLookupObj2dCreate();
    void testLookupObj2dOperator_linearInterpolation();
    void testLookupObj2dOperator_nearestNeighbour();
    void testLookupObj2dOperator_constantUntilThreshold();
    void testLookupObj1WithStatedCreate();
    void testLookupObj1WithStatedOperations();
    void testLookupObj2WithStatedCreate();
};
#endif /* _TESTLOOKUPOBJ_ */
