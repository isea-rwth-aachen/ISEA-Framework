#include "TestFindSimulationType.h"
#include "../../misc/findSimulationType.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"
#include <boost/scoped_ptr.hpp>

void TestFindSimulationType::TestSimulationType()
{
    simulation::SimulationType type;
    boost::scoped_ptr< xmlparser::XmlParser > parser;
    boost::shared_ptr< xmlparser::XmlParameter > rootNode;

    parser.reset( new xmlparser::tinyxml2::XmlParserImpl() );

    try
    {
        parser->ReadFromFile( "./TestFindSimulationTypeElectrical.xml" );
        rootNode = parser->GetRoot();
        type = simulation::FindSimulationType( rootNode );
        TS_ASSERT_EQUALS( type, simulation::ELECTRICAL );

        parser->ReadFromFile( "./TestFindSimulationTypeThermal.xml" );
        rootNode = parser->GetRoot();
        type = simulation::FindSimulationType( rootNode );
        TS_ASSERT_EQUALS( type, simulation::THERMAL );

        parser->ReadFromFile( "./TestFindSimulationTypeThEl.xml" );
        rootNode = parser->GetRoot();
        type = simulation::FindSimulationType( rootNode );
        TS_ASSERT_EQUALS( type, simulation::THERMAL_ELECTRICAL );

#ifdef BUILD_AGING_SIMULATION
        parser->ReadFromFile( "./TestFindSimulationTypeAging.xml" );
        rootNode = parser->GetRoot();
        type = simulation::FindSimulationType( rootNode );
        TS_ASSERT_EQUALS( type, simulation::AGING );
#endif
    }
    catch ( std::exception &e )
    {
        TS_FAIL( e.what() );
    }
    catch ( ... )
    {
        TS_FAIL( "Unittest failed because an unknown exception occured." );
    }
}
