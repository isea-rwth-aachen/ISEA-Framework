/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestConstObj.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Nov 2014 18:58:05 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestConstObj.h"

// ETC
#include "../../object/const_obj.h"

void TestConstObj::testConstObjectCreate()
{
    object::ConstObj< double > test( 1.0 );
    TS_ASSERT_EQUALS( strcmp( test.GetName(), "ConstObject" ), 0 );
}

void TestConstObj::testConstObjectGetValue()
{
    float Z = 1.0;
    object::ConstObj< float > test( Z );
    TS_ASSERT_EQUALS( Z, test.GetValue() );

    float factor = 3.0;
    test.SetFactor( factor );
    TS_ASSERT_EQUALS( Z * factor, test.GetValue() );
}
