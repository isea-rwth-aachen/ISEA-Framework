/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestCartesian.h
* Creation Date : 04-03-2015
* Last Modified : Mi 04 Mär 2015 17:41:11 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTCARTESIAN_
#define _TESTCARTESIAN_

#include <cxxtest/TestSuite.h>
#include "../../geometry/cartesian.h"

class TestCartesian: public CxxTest::TestSuite
{
    public:
        void TestCartesianDistance();
};



#endif /* _TESTCARTESIAN_ */
