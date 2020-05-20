/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestBoolVectorOper.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:37:52 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestBoolVectorOper.h"
#include <vector>

#include "../../misc/boolVecOper.h"

void TestBoolVectorOperation::TestOperation()
{
    {
        std::vector<bool> test;
        test.push_back(true);
        test.push_back(true);
        test.push_back(true);
        TS_ASSERT_EQUALS(Any(test), true);
        TS_ASSERT_EQUALS(All(test), true);
    }

    {
        std::vector<bool> test;
        test.push_back(false);
        test.push_back(true);
        test.push_back(true);
        TS_ASSERT_EQUALS(Any(test), true);
        TS_ASSERT_EQUALS(All(test), false);
    }

    {
        std::vector<bool> test;
        test.push_back(false);
        test.push_back(false);
        test.push_back(false);
        TS_ASSERT_EQUALS(Any(test), false);
        TS_ASSERT_EQUALS(All(test), false);

    }
}


