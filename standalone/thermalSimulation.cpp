/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : thermalSimulation.cpp
* Creation Date : 14-08-2015
* Last Modified : Di 23 Feb 2016 16:19:54 CET
* Created By : Manoel Brunnen
_._._._._._._._._._._._._._._._._._._._._.*/

#include "standalone/thermal_standalone.h"

int main( int argc, char *argv[] )
{
    standalone::ThermalStandalone thermalStandalone( "ISEA-Framework Thermal Simulation Standalone" );
    if ( !thermalStandalone.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    thermalStandalone.ReadXmlOptions();

    if ( !thermalStandalone.CreateThermalSimulation() )
        return EXIT_FAILURE;

    thermalStandalone.FreeXml();
    thermalStandalone.InitializeSimulation();

    while ( !thermalStandalone.HasThermalSimulationEnded() )
    {
        thermalStandalone.DoThermalStep();
    }

    // Sucessful exit
    std::cout << "Run was successful" << std::endl;
    return EXIT_SUCCESS;
}
