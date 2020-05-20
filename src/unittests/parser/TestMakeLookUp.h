/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMakeLookUp.h
* Creation Date : 18-05-2012
* Last Modified : Fr 21 Feb 2014 12:09:01 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMAKELOOKUP_
#define _TESTMAKELOOKUP_
#include <cxxtest/TestSuite.h>

class TestMakeLookUp : public CxxTest::TestSuite
{
public:
    void testCorrectLookup();
    void testErrorLookup();
    void testBadInput();
    void testForBadness();
};
#endif /* _TESTMAKELOOKUP_ */
