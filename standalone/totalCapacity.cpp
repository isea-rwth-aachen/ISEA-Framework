// STD
#include <cstring>
#include <string>

// ETC
#include "../src/misc/StrCont.h"
#include "../src/misc/matrixInclude.h"
#include "../src/electrical/electrical_simulation.h"
#include "../src/version/version.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"
#include "standalone/standalone.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, true >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, false >;

int main( int argc, char *argv[] )
{
    std::string xmlFilename;
    standalone::Standalone app( "ISEA-Framework check total capacity" );
    app.mApp.add_option( "xml-file", xmlFilename, "XML configuration file" )->required();
    if ( !app.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

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

    // Sucessful exit
    return EXIT_SUCCESS;
}
