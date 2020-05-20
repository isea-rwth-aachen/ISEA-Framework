/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestToLower.cpp
* Creation Date : 21-02-2014
* Last Modified : Do 27 Feb 2014 15:11:36 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestGetPositionInVector.h"
#include "../../misc/get_position_in_vector.h"



void TestGetPositionInVector::TestRun()
{
    std::vector<int> vec(3);
    vec.at(0) = - 4;
    vec.at(1) = 0;
    vec.at(2) = 5;

    TS_ASSERT_EQUALS(misc::GetPositionInVector(vec, - 4), 0);
    TS_ASSERT_EQUALS(misc::GetPositionInVector(vec, 0), 1);
    TS_ASSERT_EQUALS(misc::GetPositionInVector(vec, 5), 2);
    TS_ASSERT_EQUALS(misc::GetPositionInVector(vec, 4), 3);
    TS_ASSERT_EQUALS(misc::GetPositionInVector(vec, - 23), 3);
}


