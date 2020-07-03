/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolic2Dot.cpp
* Creation Date : 23-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/misc/ast_visitor/dotVisitor.h"
#include "../src/misc/ast_visitor/uniqueTokenizerVisitor.h"
#include "../src/misc/symbolicAst.h"
#include <cstdlib>
#include <iostream>
#include <string>

extern template struct symbolic::SymbolicAst< std::string::const_iterator >;
extern template struct symbolic::ast::DotVisitor< &std::cout, true >;
extern template struct symbolic::ast::DotVisitor< &std::cout, false >;


int main( int argc, char *argv[] )
{
    using namespace symbolic;
    using boost::spirit::ascii::space;

    bool maxDepth = true;
    standalone::Standalone app( "ISEA-Framework Symbolic To Dot Standalone" );
    app.mApp.add_option( "max-depth", maxDepth, "MaxDepth option for the dot visitor. Default = true" );
    if ( !app.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    std::string input;
    std::cin >> input;

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicAst< iterator_type > sp;
    symbolic::ast::Expression emp;

    sp g;
    std::string str = input;
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = phrase_parse( iter, end, g, space, emp );
    if ( !r || iter != end )
    {
        std::cerr << "Error transforming string to dot" << std::endl;
        std::exit( EXIT_FAILURE );
    }

    {
        std::map< std::string, std::vector< symbolic::ast::Expression * > > tmp;
        {
            symbolic::ast::UniqueVisitor visitor_U;
            boost::apply_visitor( visitor_U, emp );
            tmp = visitor_U.mUniqueTokenz;
        }

        symbolic::ast::DotVisitor< &std::cout, maxDepth > visitor( tmp );
        boost::apply_visitor( visitor, emp );
    }

    std::exit( EXIT_SUCCESS );
}
