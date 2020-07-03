/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : ..symbolic2Tex.cpp
* Creation Date : 11-01-2017
* Last Modified : Mi 01 Feb 2017 16:42:52 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/export/exportStructs/texExport.h"
#include "../src/export/symbolicExport.h"
#include "../src/misc/symbolicParser.h"
#include "standalone/standalone.h"
#include <cstdlib>
#include <iostream>
#include <string>
/*
extern template struct symbolic::SymbolicParser< std::string::const_iterator >;
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::TexExportStruct >;
*/

int main( int argc, char *argv[] )
{
    standalone::Standalone app( "ISEA-Framework Symbolic To Tex Standalone" );
    if ( !app.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    long timeout = 5;
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::string input;
    std::cin >> input;

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;
    typedef SymbolicExport< iterator_type, symbolic::TexExportStruct > se;
    std::string result;

    size_t i = 0;
    while ( true )
    {
        result = "";
        sp g;
        std::string str = input;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse( iter, end, g, space, result );
        if ( !r || iter != end )
        {
            std::cerr << "Error parsing string: " << std::endl;
            for ( auto b = iter; b != end; ++b )
            {
                std::cout << *b;
            }
            std::cout << std::endl;

            std::exit( EXIT_FAILURE );
        }
        std::cout << "Round: " << i << std::endl << "Result: \n" << result << std::endl;
        if ( input == result )
            break;
        else
        {
            input = result;
        }
        ++i;
    }
    std::cout << "Generating Tex output:" << std::endl;

    se g;
    std::string str = result;
    std::string output = "";
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = phrase_parse( iter, end, g, space, output );
    if ( !r || iter != end )
    {
        std::cerr << "Error transforming string to tex" << std::endl;
        std::exit( EXIT_FAILURE );
    }
    std::cout << "Final results: \n" << output << std::endl;
    std::exit( EXIT_SUCCESS );
}
