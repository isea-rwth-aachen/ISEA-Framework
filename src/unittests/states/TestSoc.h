/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSoc.h
* Creation Date : 11-11-2012
* Last Modified : Fr 21 Feb 2014 12:14:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSOC_
#define _TESTSOC_
#include <cxxtest/TestSuite.h>
class TestSoc: public CxxTest::TestSuite
{
public:
    void testSocCreation();
    void testSocOperations();
    void testCapacityChange();
};
#endif /* _TESTSOC_ */
