// STD
#include <cstring>
#include <string>

// ETC
#include "../src/misc/CLI11.hpp"
#include "../src/misc/StrCont.h"
#include "../src/misc/matrixInclude.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/version/version.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"

void printVersion( int /* count */ )
{
    printf( "Version: %s\n", VERSION_NUMBER );
    throw CLI::Success();
}

extern template class electrical::TwoPort< myMatrixType >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, true >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, false >;

int main( int argc, char *argv[] )
{
    CLI::App app( "ISEA-Framework check total capacity" );

    std::string xmlFilename;
    std::string currentFilename;
    app.add_flag_function( "-v,--version", printVersion, "Display version information and exit" )->short_circuit();
    app.add_option( "xml-file", xmlFilename, "XML configuration file" )->required();
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

        const double stepTime = 0.1;
        std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double, false >( rootXmlNode, stepTime, stepTime, 0.1, &cells ) );
        parser.reset();


        if ( electricalSimulation->mRootTwoPort )
        {
            electricalSimulation->mObserver->SetRoot( electricalSimulation->mRootTwoPort );
        }

        electricalSimulation->mObserver->AddFilter( new observer::StdoutFilterTwoPort< myMatrixType >() );
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

    // Sucessful exit
    return EXIT_SUCCESS;
}
