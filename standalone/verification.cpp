/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : verification.cpp
* Creation Date : 14-11-2016
* Last Modified : Mo 14 Nov 2016 16:36:55 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/misc/CLI11.hpp"
#include "../src/misc/findSimulationType.h"
#include "../src/version/version.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include <boost/scoped_ptr.hpp>
#include <iostream>

void printVersion( int /* count */ )
{
    printf( "Version: %s\n", VERSION_NUMBER );
    throw CLI::Success();
}

int main( int argc, char *argv[] )
{
    CLI::App app( "ISEA-Framework Verification Standalone" );

    std::string xmlFilename;
    bool printNumCells = false;
    bool printObserverSize = false;
    bool noPrintType = false;
    app.add_flag_function( "-v,--version", printVersion, "Display version information and exit" )->short_circuit();
    app.add_option( "xml-file", xmlFilename, "XML configuration file" )->required();
    app.add_flag( "--num-cells", printNumCells, "Print the number of cells in the configuration" );
    app.add_flag( "--observed-twoport-size", printObserverSize,
                  "Print the number of twoports observed by the electrical observer" );
    app.add_flag( "--no-type", noPrintType, "Don't print the simulation type" );
    CLI11_PARSE( app, argc, argv );

    // Parameter 1
    boost::scoped_ptr< xmlparser::XmlParser > parser;
    try
    {
        parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromFile( xmlFilename );
    }
    catch ( std::exception &e )
    {
        std::cout << "At 1. parameter: xml-file\n";
        std::cout << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        std::cout << "At 1. parameter: xml-file\n";
        std::cout << "Unidentified error\n";
        return EXIT_FAILURE;
    }

    boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser->GetRoot();

    simulation::CreatedSimulations simulations;
    simulation::SimulationType type = simulation::FindSimulationType( rootXmlNode, &simulations );

    if ( !noPrintType )
    {
        switch ( type )
        {
            case simulation::ELECTRICAL:
                std::cout << "electrical";
                break;
            case simulation::THERMAL:
                std::cout << "thermal";
                break;
            case simulation::THERMAL_ELECTRICAL:
                std::cout << "thermal-electrical";
                break;
            case simulation::AGING:
                std::cout << "aging";
                break;
            case simulation::NONE:
                std::cout << "none";
        }
        std::cout << "\n";
    }

    if ( printNumCells )
    {
        if ( simulations.electricalSimulation )
        {
            std::cout << simulations.electricalSimulation->mCellElements.size() << "\n";
        }
        else if ( simulations.thermalSimulation )
        {
            std::cout << simulations.thermalSimulation->mThermalStates.size() << "\n";
        }
    }
    if ( printObserverSize && simulations.electricalSimulation )
    {
        std::cout << simulations.electricalSimulation->mObserver->GetObservedTwoPorts().size() << "\n";
    }

    return EXIT_SUCCESS;
}