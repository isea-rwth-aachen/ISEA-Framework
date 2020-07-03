#include "../src/misc/matrixInclude.h"
#include "../src/thermal/simulation_data_outline.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "standalone/standalone.h"
#include <cstring>

int main( int argc, char *argv[] )
{
    std::string xmlfile;
    standalone::Standalone app( "ISEA-Framework Quick Visualizer" );
    app.mApp.add_option( "xml-file", xmlfile, "XML configuration file" )->required();
    if ( !app.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

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

    // Run program
    try
    {
        simulation::QuickVisualizeThermalModel< double >( parser->GetRoot() );
    }
    catch ( std::exception &e )
    {
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
