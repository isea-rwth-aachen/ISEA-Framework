/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestLookupObj.h
* Creation Date : 31-10-2012
* Last Modified : Fr 21 Feb 2014 10:25:46 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMEX2VEC_
#define _TESTMEX2VEC_
#include <cxxtest/TestSuite.h>

//ETC
class TestConvertMex2Vec: public CxxTest::TestSuite
{
    public:
        void test2DMex2Vec();
        void test2DMex1Vec();
};
#endif /* TestConvertMex2Vec */
