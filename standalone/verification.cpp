/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : verification.cpp
* Creation Date : 14-11-2016
* Last Modified : Mo 14 Nov 2016 16:36:55 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "../src/misc/checksum.h"
#include "../src/misc/findSimulationType.h"
#include "../src/version/version.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "standalone/standalone.h"

#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <string>

namespace verification
{

xmlparser::tinyxml2::XmlParserImpl mParser;
simulation::SimulationType mType;
simulation::CreatedSimulations mSimulations;

void CheckHash( int )
{
    const std::string &originalHash = misc::GetOriginalXMLChecksum( mParser );
    const std::string &actualHash = misc::GetXMLChecksum( mParser );
    if ( originalHash == actualHash )
    {
        printf( "Hash is correct\n" );
        throw CLI::Success();
    }
    else
    {
        printf( "Hash is incorrect\n" );
        exit( EXIT_FAILURE );
    }
}

void PrintHash( int )
{
    const std::string &hash = misc::GetXMLChecksum( mParser );
    printf( "%s\n", hash.c_str() );
    throw CLI::Success();
}

void AddHash( int )
{
    const std::string &hash = misc::AddXMLChecksum( mParser );
    mParser.SaveFile();
    printf( "%s\n", hash.c_str() );
    throw CLI::Success();
}

void PrintNumberOfCells( int )
{
    if ( mSimulations.electricalSimulation )
    {
        std::cout << mSimulations.electricalSimulation->mCellElements.size() << "\n";
    }
    else if ( mSimulations.thermalSimulation )
    {
        std::cout << mSimulations.thermalSimulation->mThermalStates.size() << "\n";
    }
}

void PrintSimulationType( int )
{
    switch ( mType )
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

void PrintObserverSize( int )
{
    if ( mSimulations.electricalSimulation )
    {
        std::cout << mSimulations.electricalSimulation->mObserver->GetObservedTwoPorts().size() << "\n";
    }
}

void CreateSimulation( const std::string &filename )
{
    mParser.ReadFromFile( filename );
    boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = mParser.GetRoot();
    mType = simulation::FindSimulationType( rootXmlNode, &mSimulations );
}

}    // namespace verification

int main( int argc, char *argv[] )
{
    standalone::Standalone app( "ISEA-Framework Verification Standalone" );
    std::string xmlFilename;
    bool printObserverSize = false;
    app.mApp.add_option_function< std::string >( "xml-file", verification::CreateSimulation, "XML configuration file" )->required();
    app.mApp.add_flag_function( "--num-cells", verification::PrintNumberOfCells,
                                "Print the number of cells in the configuration" );
    app.mApp.add_flag_function( "--observed-twoport-size", verification::PrintObserverSize,
                                "Print the number of twoports observed by the electrical observer" );
    app.mApp.add_flag_function( "--type", verification::PrintSimulationType, "Print the simulation type" );
    app.mApp.add_flag_function( "--check-hash", verification::CheckHash,
                                "Check if the hash in the XML file is correct" );
    app.mApp.add_flag_function( "--print-hash", verification::PrintHash,
                                "Calculate the hash of the XML file and print it" );
    app.mApp.add_flag_function( "--add-hash", verification::AddHash,
                                "Add hash to the XML file and print the hash value" );
    if ( !app.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}