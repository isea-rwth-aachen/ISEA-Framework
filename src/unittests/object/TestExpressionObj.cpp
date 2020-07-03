#include "TestExpressionObj.h"
#include "../../object/expression_obj.h"

void TestExpressionObj::testExpressionObj()
{
    double x = 10.0;
    exprtk::expression< double > expr;
    exprtk::symbol_table< double > symbol_table;
    symbol_table.add_variable( "x", x, false );
    expr.register_symbol_table( symbol_table );
    exprtk::parser< double > parser;
    parser.compile( "x + 3", expr );
    object::ExpressionObject< double > obj( expr );
    TS_ASSERT_DELTA( obj.GetValue(), x + 3.0, 1e-6 );
    TS_ASSERT_DELTA( obj(), x + 3.0, 1e-6 );
    x = 2.0;
    TS_ASSERT_DELTA( obj.GetValue(), x + 3.0, 1e-6 );
    TS_ASSERT_DELTA( obj(), x + 3.0, 1e-6 );
}