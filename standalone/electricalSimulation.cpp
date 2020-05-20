#include "standalone.h"

int main( int argc, char *argv[] )
{
    standalone::Standalone electricalStandalone( "ISEA-Framework Electrical Simulation Standalone" );
    if ( !electricalStandalone.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    electricalStandalone.ReadOptions();

    if ( !electricalStandalone.CreateElectricalSimulation() )
        return EXIT_FAILURE;

    electricalStandalone.FreeXml();
    electricalStandalone.InitializeSimulation();

    // Run simulation
    while ( !electricalStandalone.HasElectricalSimulationEnded() )
    {
        electricalStandalone.DoElectricalStep();
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
