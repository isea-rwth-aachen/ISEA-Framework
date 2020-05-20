/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : directSymbolic2Tex.cpp
* Creation Date : 13-01-2017
* Last Modified : Fr 13 Jan 2017 16:46:35 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include <iostream>
#include <string>
#include <cstdlib>
#include "../src/export/symbolicExport.h"
#include "../src/export/exportStructs/texExport.h"

extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::TexExportStruct >;


int main( int /*argc*/, char *[] /*argv*/ )
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::string input;
    std::cin >> input;

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicExport< iterator_type, symbolic::TexExportStruct > se;

    std::cout << "Generating Tex output(" << input.size() << "):" << std::endl;

    se g;
    std::string output;
    std::string::const_iterator iter = input.begin();
    std::string::const_iterator end = input.end();
    bool r = phrase_parse( iter, end, g, space, output );
    if ( !r || iter != end )
    {
        std::cerr << "Error transforming string to tex:" << std::endl;
        while ( iter != end )
        {
            std::cerr << *iter;
            ++iter;
        }
        std::cerr << std::endl;

        std::exit( EXIT_FAILURE );
    }
    std::cout << "Final results: \n" << output << std::endl;
    std::exit( EXIT_SUCCESS );
}
