#include "TestExpressionObj.h"
#include "../../object/expression_obj.h"
#include "../../state/valueStateWrapper.h"
#include <boost/make_shared.hpp>

void TestExpressionObj::testExpressionObj()
{
    double x = 10.0;
    auto state = boost::make_shared< state::ValueStateWrapper< double > >( &x );
    std::vector< object::ExpressionObject< double >::Parameter > parameters{{"x", state}};
    object::ExpressionObject< double > obj( "x + 3", parameters );
    TS_ASSERT_DELTA( obj.GetValue(), x + 3.0, 1e-6 );
    TS_ASSERT_DELTA( obj(), x + 3.0, 1e-6 );
    x = 2.0;
    TS_ASSERT_DELTA( obj.GetValue(), x + 3.0, 1e-6 );
    TS_ASSERT_DELTA( obj(), x + 3.0, 1e-6 );
}

void TestExpressionObj::testRescale()
{
    double x = 2.0;
    auto state = boost::make_shared< state::ValueStateWrapper< double > >( &x );
    object::ExpressionObject< double >::RescaleRange xRescale( {2.0, 5.0}, {9.5, 15.0} );
    std::vector< object::ExpressionObject< double >::Parameter > parameters{{"x", state, xRescale}};
    object::ExpressionObject< double > obj( "x + 3", parameters );
    TS_ASSERT_DELTA( obj.GetValue(), 12.5, 1e-6 );
    x = 5.0;
    TS_ASSERT_DELTA( obj.GetValue(), 18.0, 1e-6 );
    x = 0.0;
    TS_ASSERT_DELTA( obj.GetValue(), ( -2.0 ) * 5.5 / 3.0 + 12.5, 1e-6 );

    object::ExpressionObject< double >::RescaleRange outputRescale( {12.5, 18.0}, {2.0, 5.0} );
    obj = object::ExpressionObject< double >( "x + 3", parameters, outputRescale );
    for ( x = 0; x < 10; ++x )
    {
        TS_ASSERT_DELTA( obj.GetValue(), x, 1e-6 );
    }
}