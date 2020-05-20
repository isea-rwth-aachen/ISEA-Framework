/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestCaseInseStringCmp.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:39:59 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestCaseInseStringCmp.h"

#include <cstring>
#include "../../misc/charArrayCmp.h"

void TestCaseInseStringCmp::TestStringCompare()
{
    const char* testString = "Das Ist EIn Brot";
    const char* testStringCorrect = "DAS IST EIN BROT";
    const char* testABig = "Ausw";
    const char* testASmall = "ausw";
    const char* testFBig = "Fusw";
    const char* testFSmall = "fusw";

    TS_ASSERT_EQUALS(misc::CaseInsensitiveStringCompare(testString, testStringCorrect), 0 );

    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testABig) > 0 );
    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testASmall) > 0);

    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testFBig) < 0 );
    TS_ASSERT(misc::CaseInsensitiveStringCompare(testString, testFSmall) < 0);
}

