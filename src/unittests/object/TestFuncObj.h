/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestFuncObj.h
* Creation Date : 14-05-2013
* Last Modified : Fr 21 Feb 2014 12:00:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTFUNCOBJ_
#define _TESTFUNCOBJ_
#include <cxxtest/TestSuite.h>
const double pi =  3.14159265;
class TestFunctObj: public CxxTest::TestSuite
{
    public:
        void testFuncObjectCreate();
        void testFuncObjectEvaluation();
};
#endif /* _TESTFUNCOBJ_ */
