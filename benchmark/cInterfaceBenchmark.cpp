/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : main.cpp
* Creation Date : 31-10-2012
* Last Modified : Do 03 Mär 2016 15:58:30 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

// STD
#include <string>
#include <cstring>
#include <iterator>
#include <fstream>
#include <boost/scoped_array.hpp>

#include "../src/observer/observer.h"
#include "../src/factory/observer/createBenchmarkFilter.h"

#include "../src/electrical/twoport.h"
#include "../src/observer/twoPortObserver.h"
#include "../src/system/stateSystemGroup.h"
#include "../src/system/dae_sys.h"
#include "../src/time_series/time_series.h"

#include "../src/cinterface/cinterface.h"

#include "../src/thermal/electrical_simulation.h"

// BOOST
#include "boost/date_time.hpp"
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif

typedef myMatrixType myUnit;
typedef double myScalar;

struct PointerStructure
{
    boost::scoped_ptr< simulation::ElectricalSimulation< myUnit, myScalar, false > > mElectricalSimulation;
};


void DieDieDie()
{
#ifdef __EXCEPTIONS__
    throw( std::runtime_error( "Couldn't find file" ) );
#else
    std::cerr << "Couldn't find file" << std::endl;
    abort();
    return;
#endif
}

int main( int argc, char *argv[] )
{
    size_t pointerStructureAddress = 0;
    size_t statesize = 0;
    size_t observersize = 0;

    if ( argc < 3 )
    {
        printf( "Error, must be called with exactly 2 parameters:\nxml-file\ncurrent profile\n" );
        return EXIT_FAILURE;
    }


    // Parameter 1
    std::fstream filestream( argv[1], std::fstream::in );
    if ( !filestream.is_open() )
        DieDieDie();
    std::string xmlString( ( std::istreambuf_iterator< char >( filestream ) ), std::istreambuf_iterator< char >() );
    InitializeSystem( xmlString.c_str(), &pointerStructureAddress, 0, &statesize, &observersize );

    // Parameter 2
    boost::scoped_ptr< electrical::TimeSeries< double, electrical::EvalNoInterpolation > > currentProfile;
    try
    {
        std::string filename( argv[2] );

        if ( filename.rfind( ".mat" ) != std::string::npos )
        {
            matlab::MatFile matFile( filename );
            matlab::MatioData &matStruct = matFile["diga"]["daten"];
            std::vector< double > time = matStruct["Programmdauer"].operator()< matlab::MatioData::X_DIM >( 0 );
            std::vector< double > current = matStruct["Strom"].operator()< matlab::MatioData::X_DIM >( 0 );
            currentProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( time, current ) );
        }
        else
        {
            std::ifstream currentProfileFile( argv[2] );
            currentProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( currentProfileFile ) );
        }
    }
    catch ( std::exception &e )
    {
        printf( "At 2. parameter: current profile\n" );
        printf( "%s\n", e.what() );
        return EXIT_FAILURE;
    }
    catch ( ... )
    {
        printf( "At 2. parameter: current profile\n" );
        printf( "Unidentified error\n" );
        return EXIT_FAILURE;
    }

    statesize = GetRealStateSize( &pointerStructureAddress );
    observersize = GetRealOutputSize( &pointerStructureAddress );

    boost::scoped_array< double > x( new double[statesize] );
    boost::scoped_array< double > dxdt( new double[statesize] );
    double totalVoltageOutputVec[1];
    double totalCurrentOutputVec[1];
    double totalPowerOutputVec[1];

    boost::scoped_array< double > voltageOutputVec( new double[statesize] );
    boost::scoped_array< double > currentOutputVec( new double[statesize] );
    boost::scoped_array< double > powerOutputVec( new double[statesize] );
    boost::scoped_array< double > socOutputVec( new double[statesize] );
    boost::scoped_array< double > surfaceSocOutputVec( new double[statesize] );

    totalVoltageOutputVec[0] = 0;
    totalCurrentOutputVec[0] = 0;
    totalPowerOutputVec[0] = 0;

    memset( x.get(), 0, sizeof( double ) * statesize );
    memset( dxdt.get(), 0, sizeof( double ) * statesize );
    memset( voltageOutputVec.get(), 0, sizeof( double ) * statesize );
    memset( currentOutputVec.get(), 0, sizeof( double ) * statesize );
    memset( powerOutputVec.get(), 0, sizeof( double ) * statesize );
    memset( socOutputVec.get(), 0, sizeof( double ) * statesize );
    memset( surfaceSocOutputVec.get(), 0, sizeof( double ) * statesize );

    SetOutputVectors( &pointerStructureAddress, voltageOutputVec.get(), currentOutputVec.get(), powerOutputVec.get(),
                      socOutputVec.get(), surfaceSocOutputVec.get() );

    PointerStructure *pointerStructure = (PointerStructure *)pointerStructureAddress;
    auto observer_ptr = pointerStructure->mElectricalSimulation->mObserver.get();
    if ( observer_ptr )
        AddBenchmarkFilter( observer_ptr );


    // Time
    double t = 0.0;
    const double dt = 0.001;

    // Iteration
    int count = 5000;
    if ( argc >= 4 )
        count = atoi( argv[3] );

    SetCurrent( &pointerStructureAddress, 0.0 );

    for ( int i = 0; i < count; i++ )
    {
        currentProfile->SetTimeAndTriggerEvaluation( t );
        SetCurrent( &pointerStructureAddress, currentProfile->GetValue() );

        RunAlgebraicElectricalSystemPart( &pointerStructureAddress, x.get(), dt );
        OutputRootElementData( &pointerStructureAddress, totalVoltageOutputVec, totalCurrentOutputVec, totalPowerOutputVec );
        TriggerObserver( &pointerStructureAddress, t );

        RunSystem( &pointerStructureAddress, x.get(), dxdt.get() );
        for ( size_t j = 0; j < statesize; ++j )
            x[j] = x[j] + dt * dxdt[j];

        t += dt;
    }

    DeleteSystem( &pointerStructureAddress );    //< Free the memory that is used by the DAE

    return 0;
}
