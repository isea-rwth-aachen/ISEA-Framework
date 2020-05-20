/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestDouble2Char.cpp
* Creation Date : 19-08-2014
* Last Modified : Di 24 Mai 2016 18:25:01 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestDouble2Char.h"

// STD
#include <cstring>

// ETC
#include "../../convert/double2char.h"

void TestDouble2Char::testDouble2CharImplementation()
{
    double array[] = {84,  104, 105, 115, 32,  105, 115, 32,  97,  32, 115, 116, 114, 105, 110,
                      103, 44,  32,  112, 115, 115, 115, 115, 116, 46, 46,  46,  33,  0};
    Double2Char convertedString( array, sizeof( array ) / sizeof( double ) );
    TS_ASSERT_EQUALS( strcmp( convertedString.mReturnArray.get(), "This is a string, psssst...!" ), 0 );
    TS_ASSERT_DIFFERS( strcmp( convertedString.mReturnArray.get(), "This is not a string, psssst...!" ), 0 );
}

void TestDouble2Char::testDouble2CharFailImplementation()
{
    double array[] = {256, 84,  104, 105, 115, 32,  105, 115, 32,  97,  32, 115, 116, 114, 105,
                      110, 103, 44,  32,  112, 115, 115, 115, 115, 116, 46, 46,  46,  33,  0};
    Double2Char convertedString( array, sizeof( array ) / sizeof( double ) );
    TS_ASSERT_EQUALS( strcmp( convertedString.mReturnArray.get(), "\x01This is a string, psssst...!" ), 0 );
    TS_ASSERT_DIFFERS( strcmp( convertedString.mReturnArray.get(), "This is not a string, psssst...!" ), 0 );
}
