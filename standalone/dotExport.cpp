/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : dotExport.cpp
* Creation Date : 22-07-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
// STD
#include <cstring>
#include <fstream>
#include <string>

// BOOST
#include <boost/numeric/odeint.hpp>

// ETC
#include "../src/container/matio_file.h"
#include "../src/export/esbVisualizer.h"
#include "../src/misc/StrCont.h"
#include "../src/misc/fast_copy_matrix.h"
#include "../src/misc/matrixInclude.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/time_series/eval_linear_interpolation.h"
#include "../src/time_series/time_series.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"


int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        printf( "Error, must be called with exactly 2 parameters:\nxml-file\ncurrent profile\n" );
        return EXIT_FAILURE;
    }

    // Parameter 1
    boost::scoped_ptr< xmlparser::XmlParser > parser;
    try
    {
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromFile( argv[1] );
    }
    catch ( std::exception &e )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
    }

    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double, false > > electricalSimulation;

    try
    {

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double, false >( rootXmlNode, 0.1, 0.1, 0.1, &cells ) );
        parser.reset();
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the equation systems\n" );
        printf( "%s", message );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "Unknown error while creating the equation systems\n" );
        return EXIT_FAILURE;
    }

    visualizer::EsbVisualizer< myMatrixType > testVisualizer( electricalSimulation->mRootTwoPort.get(), &std::cout );
    return EXIT_SUCCESS;
}
