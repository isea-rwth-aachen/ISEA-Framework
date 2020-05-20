/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestConstObj.h
* Creation Date : 31-10-2012
* Last Modified : Fr 21 Feb 2014 11:58:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTCONSTOBJ_
#define _TESTCONSTOBJ_
#include <cxxtest/TestSuite.h>

class TestConstObj: public CxxTest::TestSuite
{
    public:
        void testConstObjectCreate();
        void testConstObjectGetValue();
};
#endif /* _TESTCONSTOBJ_ */
