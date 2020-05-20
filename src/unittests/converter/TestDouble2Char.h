/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestDouble2Char.h
* Creation Date : 19-08-2014
* Last Modified : Di 19 Aug 2014 10:39:16 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTDOUBLE2AR_
#define _TESTDOUBLE2AR_

#include <cxxtest/TestSuite.h>

/// This class tests the conversion from double to char
class TestDouble2Char: public CxxTest::TestSuite
{
    public:
        void testDouble2CharImplementation();
		void testDouble2CharFailImplementation();
    private:
    protected:
};

#endif /* _TESTDOUBLE2AR_ */
