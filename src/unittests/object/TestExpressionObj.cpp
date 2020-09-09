#include "TestExpressionObj.h"
#include "../../object/expression_obj.h"
#include "../../state/valueStateWrapper.h"
#include <boost/make_shared.hpp>

void TestExpressionObj::testExpressionObj()
{
    double x = 10.0;
    auto state = boost::make_shared< state::ValueStateWrapper< double > >( &x );
    std::vector< object::ExpressionObject< double >::ParameterT > parameters{{"x", state}};
    object::ExpressionObject< double > obj( "x + 3", parameters );
    TS_ASSERT_DELTA( obj.GetValue(), x + 3.0, 1e-6 );
    TS_ASSERT_DELTA( obj(), x + 3.0, 1e-6 );
    x = 2.0;
    TS_ASSERT_DELTA( obj.GetValue(), x + 3.0, 1e-6 );
    TS_ASSERT_DELTA( obj(), x + 3.0, 1e-6 );
}