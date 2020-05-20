/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestFuncObj.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Nov 2014 18:57:44 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestFuncObj.h"

// STD
#include <cmath>

// ETC
#include "../../object/function_obj1d.h"

void TestFunctObj::testFuncObjectCreate()
{
    object::FunctionObj< double, ::sin > obj;
    TS_ASSERT_EQUALS( strcmp( obj.GetName(), "FunctionObj" ), 0 );
}

void TestFunctObj::testFuncObjectEvaluation()
{
    object::FunctionObj< double, ::sin > obj;
    double value( 0 );
    for ( size_t i = 0; i < 360; ++i )    // Walk in a circle
    {
        value = static_cast< double >( i ) / 360 * 2 * pi;
        TS_ASSERT_EQUALS( obj( value ), ::sin( value ) );
        TS_ASSERT_EQUALS( obj(), ::sin( value ) );
    }

    double factor = 2.0;
    obj.SetFactor( factor );
    TS_ASSERT_EQUALS( obj( pi / 4 ), factor * ::sin( pi / 4 ) );
}
