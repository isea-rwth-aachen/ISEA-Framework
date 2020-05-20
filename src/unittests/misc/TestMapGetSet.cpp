/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMapGetSet.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:54:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestMapGetSet.h"

#include <vector>
#include "../../misc/mapGetSet.h"

void TestMapGetSet::TestCreate()
{
    MapGetSet<double, double > test;
}

void TestMapGetSet::TestAddGet()
{
    MapGetSet<double, double > test;
    test.Add(4.0, 123.0);
    test.Add(5.0, 66.0);
    test.Add(5.0, 77.0);
    TS_ASSERT_EQUALS(test.Get(4.0)[0], 123.0);
    TS_ASSERT_EQUALS(test.Get(5.0)[0], 66.0);
    TS_ASSERT_EQUALS(test.Get(5.0)[1], 77.0);

    TS_ASSERT_EQUALS(test.Get(0).empty(), true);
}


