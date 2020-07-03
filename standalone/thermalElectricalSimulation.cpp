#include "standalone/thermal_electrical_standalone.h"

int main( int argc, char *argv[] )
{
    standalone::ThermalElectricalStandalone thElStandalone( "ISEA-Framework Thermal-Electrical Simulation Standalone" );
    if ( !thElStandalone.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    thElStandalone.ReadXmlOptions();

    if ( !thElStandalone.CreateElectricalSimulation() )
        return EXIT_FAILURE;
    if ( !thElStandalone.CreateThermalSimulation() )
        return EXIT_FAILURE;

    thElStandalone.InitializeSimulation();

    for ( size_t actualCycleNumber = 0; actualCycleNumber < thElStandalone.mCycles; ++actualCycleNumber )
    {
        if ( actualCycleNumber > 0 )
        {
            thElStandalone.StartNewCycle();
        }

        while ( !thElStandalone.HasElectricalSimulationEnded() )
        {
            thElStandalone.DoElectricalStep();
            thElStandalone.DoThermalStep();
        }
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
