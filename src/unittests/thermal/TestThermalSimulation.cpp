/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestThermalSimulation.cpp
* Creation Date : 05-03-2015
* Last Modified : Di 24 Mai 2016 18:25:14 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestThermalSimulation.h"
#include "../../thermal/thermal_simulation.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"


void TestThermalSimulation::TestThermalSimulationRun()
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromFile( "./TestRectangularblock.xml" );
    boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();
    simulation::ThermalSimulation< myMatrixType, double, true > sim( rootXmlNode, 0.001, 0.5, 5, 0, 0, 0 );

    TS_ASSERT_EQUALS( sim.mThermalProbes->size(), 8 );

    for ( size_t i = 0; i < sim.mThermalProbes->size(); ++i )
    {
        TS_ASSERT_DELTA( sim.mThermalProbes->at( i ).GetCorrespondingThermalElement()->GetTemperature(), 100, 0.0001 );
        TS_ASSERT( sim.mThermalProbes->at( i ).GetCorrespondingThermalElement()->GetGridVertex().Distance(
                    sim.mThermalProbes->at( i ).GetCoordinates() ) < 0.0001 );
    }
}
