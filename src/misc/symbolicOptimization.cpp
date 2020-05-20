/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolicOptimization.cpp
* Creation Date : 05-03-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "symbolicOptimization.h"
#include "../exceptions/error_function.h"
#include "ast_visitor/optimizationVisitor.h"
#include "ast_visitor/symbolicVisitor.h"
#include "ast_visitor/uniqueTokenizerVisitor.h"
#include "symbolicAst.h"
//#include "../exceptions/error_proto.h"

namespace symbolic
{
bool SymbolicOptimization::Optimize( std::string &input, std::string &returnStr )
{
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef SymbolicAst< iterator_type > sp;

    sp g;

    std::string str = input;

    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    symbolic::ast::Expression emp;
    bool r = phrase_parse( iter, end, g, space, emp );
    if ( !r || iter != end )
    {
        std::string error_string = "Error parsing string: \n";
        for ( auto b = iter; b != end; ++b )
        {
            error_string += *b;
        }
        ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough", error_string.c_str() );
        return false;
    }

    std::stringstream firstRun;
    symbolic::ast::SymbolicVisitor strV( &firstRun );
    boost::apply_visitor( strV, emp );

    std::string lastValue = "";
    std::string currentValue = firstRun.str();

    symbolic::ast::Expression exp_memory = emp;
    while ( lastValue != currentValue )
    {
        lastValue = currentValue;
        symbolic::ast::OptimizationVisitor x;
        exp_memory = boost::apply_visitor( x, exp_memory );

        std::stringstream currentRun;
        symbolic::ast::SymbolicVisitor cur_V( &currentRun );
        boost::apply_visitor( cur_V, exp_memory );
        currentValue = currentRun.str();
    }
    returnStr = currentValue;
    return true;
}
}    // namespace symbolic
