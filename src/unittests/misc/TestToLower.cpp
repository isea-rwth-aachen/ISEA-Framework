/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestToLower.cpp
* Creation Date : 21-02-2014
* Last Modified : Do 27 Feb 2014 15:11:36 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestToLower.h"
#include "../../misc/toLower.h"
#include <cstring>

void TestToLower::TestToLow()
{
    const char *testCharArray = "DAS_IST_EIN_TEST";
    TS_ASSERT( !strcmp( misc::ToLower(testCharArray).get(), "das_ist_ein_test" ) );
}


