/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : eigenValues.cpp
* Creation Date : 08-09-2016
* Last Modified : Do 08 Sep 2016 17:01:12 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

// STD
#include <cstring>
#include <fstream>
#include <string>

// ETC
#include "../src/export/numericExport.h"
#include "../src/misc/StrCont.h"
#include "../src/misc/matrixInclude.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"


int main( int argc, char *argv[] )
{
    if ( argc != 2 )
    {
        printf( "Error, must be called with exactly 1 parameter:\nxml-file\n" );
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

    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, ScalarUnit, false > > electricalSimulation;

    try
    {

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, ScalarUnit, false >( rootXmlNode, 0.01, 0.01, 0.1, &cells ) );
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

    // Run simulation
    electricalSimulation->mRootTwoPort->SetCurrent( 1 );
    electricalSimulation->UpdateSystem();
    electricalSimulation->UpdateSystemValues();

    std::stringstream outstream;
    NumericExport<> exporter( electricalSimulation.get(), &std::cout );

    // Sucessful exit
    return EXIT_SUCCESS;
}
