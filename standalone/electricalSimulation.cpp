#include "standalone/electrical_standalone.h"

int main( int argc, char *argv[] )
{
    standalone::ElectricalStandalone electricalStandalone( "ISEA-Framework Electrical Simulation Standalone" );
    if ( !electricalStandalone.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    electricalStandalone.ReadXmlOptions();

    if ( !electricalStandalone.CreateElectricalSimulation() )
        return EXIT_FAILURE;

    electricalStandalone.InitializeSimulation();
    electricalStandalone.FreeXml();

    // Run simulation
    while ( !electricalStandalone.HasElectricalSimulationEnded() )
    {
        electricalStandalone.DoElectricalStep();
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
