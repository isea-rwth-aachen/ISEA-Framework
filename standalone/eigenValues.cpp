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
#include "../src/electrical/electrical_simulation.h"
#include "../src/export/numericExport.h"
#include "../src/misc/StrCont.h"
#include "../src/misc/matrixInclude.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "standalone/standalone.h"

int main( int argc, char *argv[] )
{
    std::string xmlfile;
    standalone::Standalone app( "ISEA-Framework Eigen Values Standalone" );
    app.mApp.add_option( "xml-file", xmlfile, "XML configuration file" )->required();
    if ( !app.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    // Parameter 1
    boost::scoped_ptr< xmlparser::XmlParser > parser;
    try
    {
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromFile( xmlfile );
    }
    catch ( std::exception &e )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }

    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double, false > > electricalSimulation;

    try
    {

        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double, false >( rootXmlNode, 0.01, 0.01, 0.1, &cells ) );
        parser.reset();
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the equation systems\n" );
        printf( "%s", message );
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
