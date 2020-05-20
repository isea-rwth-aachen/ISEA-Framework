/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolicOptimization.cpp
* Creation Date : 06-01-2017
* Last Modified : Fr 13 Jan 2017 10:25:57 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include <iostream>
#include <string>
#include <cstdlib>
#include "../src/misc/symbolicOptimization.h"
#include "../src/exceptions/error_proto.h"

int main( int /*argc*/, char *[] /*argv*/ )
{
    using namespace symbolic;
    std::string input, result;
    std::cin >> input;

    SymbolicOptimization optimzier;
    if ( !optimzier.Optimize( input, result ) )
        std::exit( EXIT_FAILURE );
    std::cout << result << std::endl;
    std::exit( EXIT_SUCCESS );
}
