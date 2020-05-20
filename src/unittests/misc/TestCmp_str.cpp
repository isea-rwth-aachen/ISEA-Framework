/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestCmp_str.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:52:44 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "../../misc/cmp_str.h"
#include "TestCmp_str.h"

void TestCmp_Str::TestCmpStr()
{
    misc::cmp_str CmpStr;
    TS_ASSERT_EQUALS(CmpStr("EinString", "EinString"), false);
    TS_ASSERT(CmpStr("EinString", "EinSxring"));
    TS_ASSERT_EQUALS(CmpStr("EinString", "EinStrang"), false);

    misc::equal_str EqStr;
    TS_ASSERT(EqStr("EinString", "EinString"));
    TS_ASSERT(EqStr("EinString", "eINsTRING"));
    TS_ASSERT_EQUALS(EqStr("EinString", "INsTRIN"), false);
}


