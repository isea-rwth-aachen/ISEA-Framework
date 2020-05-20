/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSlice.h
* Creation Date : 08-04-2015
* Last Modified : Mi 15 Apr 2015 11:34:17 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSLICE_
#define _TESTSLICE_
#include <cxxtest/TestSuite.h>

/// DESCRIPTION
class TestSlice : public CxxTest::TestSuite
{
    public:
    void TestSliceCreation();
    void TestSliceEditing();
};
#endif /* _TESTSLICE_ */
