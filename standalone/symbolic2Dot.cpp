/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolic2Dot.cpp
* Creation Date : 23-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include <iostream>
#include <string>
#include <cstdlib>
#include "../src/misc/symbolicAst.h"
#include "../src/misc/ast_visitor/dotVisitor.h"
#include "../src/misc/ast_visitor/uniqueTokenizerVisitor.h"

extern template struct symbolic::SymbolicAst< std::string::const_iterator >;
extern template struct symbolic::ast::DotVisitor< &std::cout, true >;
extern template struct symbolic::ast::DotVisitor< &std::cout, false >;


int main( int argc, char *argv[] )
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
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
        if ( argc != 2 )
        {
            symbolic::ast::DotVisitor< &std::cout, true > visitor( tmp );
            boost::apply_visitor( visitor, emp );
        }

        if ( argc == 2 )
        {
            if ( std::string( argv[1] ) == "1" )
            {
                symbolic::ast::DotVisitor< &std::cout, true > visitor( tmp );
                boost::apply_visitor( visitor, emp );
            }
            else if ( std::string( argv[1] ) == "2" || std::string( argv[1] ) == "0" )
            {
                symbolic::ast::DotVisitor< &std::cout, false > visitor( tmp );
                boost::apply_visitor( visitor, emp );
            }
            else
            {
                std::cerr << "No correct input for argv[1]" << std::endl;
                std::exit( EXIT_FAILURE );
            }
        }
    }

    std::exit( EXIT_SUCCESS );
}
