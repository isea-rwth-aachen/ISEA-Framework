#include "../src/misc/matrixInclude.h"
#include "../src/thermal/simulation_data_outline.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include <cstring>


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
    catch ( ... )
    {
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
