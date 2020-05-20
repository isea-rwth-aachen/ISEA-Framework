#include "TestCinterface_ThEl.h"
#include "../../cinterface/cinterface_ThEl.h"

#include <boost/scoped_ptr.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>

static const double sDelta = 0.000001;
static const double eulerDelta = 0.01;
static const double eulerDelta2 = 0.1;

void TestCinterface_ThEl::TestCreation()
{
    // Creating
    std::string line, file;
    std::ifstream xmlFile( "TestPouchBag.xml" );
    if ( !xmlFile.is_open() )
    {
        TS_FAIL( "File not found" );
        return;
    }
    while ( std::getline( xmlFile, line ) )
        file += line + "\n";
    const char *config = file.c_str();

    size_t pointerStructureAddress = 0;
    InitializeSystem_ThEl( config, &pointerStructureAddress );
    TS_ASSERT_DIFFERS( pointerStructureAddress, 0 );
    DeleteSystem_ThEl( &pointerStructureAddress );
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );

    // Reset
    InitializeSystem_ThEl( config, &pointerStructureAddress );
    ResetSystem_ThEl( config, &pointerStructureAddress );
    TS_ASSERT_DIFFERS( pointerStructureAddress, 0 );
    DeleteSystem_ThEl( &pointerStructureAddress );
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );

    // GetSize
    size_t realStatesize, realObserversize, realNumberOfCells, definedStatesize, definedObserversize, definedNumberOfCells, probeSize;
    InitializeSystem_ThEl( config, &pointerStructureAddress );
    GetRealSizes_ThEl( config, 0, &realStatesize, &realObserversize, &realNumberOfCells, &probeSize );
    TS_ASSERT_EQUALS( realStatesize, 67 );
    TS_ASSERT_EQUALS( realObserversize, 2 );
    TS_ASSERT_EQUALS( realNumberOfCells, 2 );
    TS_ASSERT_EQUALS( probeSize, 1 );
    GetRealSizes_ThEl( config, &pointerStructureAddress, &realStatesize, &realObserversize, &realNumberOfCells, &probeSize );
    TS_ASSERT_EQUALS( realStatesize, 67 );
    TS_ASSERT_EQUALS( realObserversize, 2 );
    TS_ASSERT_EQUALS( realNumberOfCells, 2 );
    TS_ASSERT_EQUALS( probeSize, 1 );
    DeleteSystem_ThEl( &pointerStructureAddress );
    GetDefinedSizes( config, &definedStatesize, &definedObserversize, &definedNumberOfCells, &probeSize );
    TS_ASSERT_EQUALS( definedStatesize, 2 );
    TS_ASSERT_EQUALS( definedObserversize, 3 );
    TS_ASSERT_EQUALS( definedNumberOfCells, 4 );
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );
    TS_ASSERT_EQUALS( probeSize, 1 );

    // SetCurrent
    InitializeSystem_ThEl( config, &pointerStructureAddress );
    SetCurrent_ThEl( &pointerStructureAddress, 0 );
    SetCurrent_ThEl( &pointerStructureAddress, 2 );
    SetCurrent_ThEl( &pointerStructureAddress, -2 );
    DeleteSystem_ThEl( &pointerStructureAddress );
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );

    // Run system
    const double dt = 0.001;
    const double time = 0.0;
    InitializeSystem_ThEl( config, &pointerStructureAddress );
    GetRealSizes_ThEl( config, &pointerStructureAddress, &realStatesize, &realObserversize, &realNumberOfCells, &probeSize );

    std::vector< real_T > x( realStatesize );
    std::vector< real_T > dxdt( realStatesize );
    std::vector< real_T > voltageOutputVec( realStatesize );
    std::vector< real_T > currentOutputVec( realStatesize );
    std::vector< real_T > powerOutputVec( realStatesize );
    std::vector< real_T > socOutputVec( realObserversize );
    std::vector< real_T > surfaceSocOutputVec( realObserversize );
    std::vector< real_T > probeVector( probeSize + 1 );    // + 1 needed as the probeSize itself is zero

    SetOutputVectors_ThEl( &pointerStructureAddress, &voltageOutputVec.at( 0 ), &currentOutputVec.at( 0 ),
                           &powerOutputVec.at( 0 ), &socOutputVec.at( 0 ), &surfaceSocOutputVec.at( 0 ) );

    SetCurrent_ThEl( &pointerStructureAddress, 2.05 );

    RunAlgebraicElectricalSystemPart_ThEl( &pointerStructureAddress, &x.at( 0 ), dt );

    TriggerObserverAndCellTemperatureOutput_ThEl( &pointerStructureAddress, time, 0, &probeVector.at( 0 ) );

    RunSystem_ThEl( &pointerStructureAddress, &x.at( 0 ), &dxdt.at( 0 ) );

    TriggerObserverAndCellTemperatureOutput_ThEl( &pointerStructureAddress, time, 0, &probeVector.at( 0 ) );

    DeleteSystem_ThEl( &pointerStructureAddress );    //< Free the memory that is used by the DAE
}

void TestCinterface_ThEl::TestScenario()
{
    boost::scoped_ptr< size_t > pointerStructureAddress( new size_t( 0 ) );
    size_t stateSize = 0;
    size_t vcpSize = 0;
    size_t numberOfCells = 0;
    size_t numberOfProbes = 0;
    std::string line, file;
    std::ifstream xmlFile( "TestPouchBag.xml" );

    if ( !xmlFile.is_open() )
    {
        TS_FAIL( "File not found" );
        return;
    }

    while ( std::getline( xmlFile, line ) )
        file += line + "\n";
    const char *config = file.c_str();

    // mdlInitializeSizes
    InitializeSystem_ThEl( config, pointerStructureAddress.get(), 0 );
    GetRealSizes_ThEl( config, pointerStructureAddress.get(), &stateSize, &vcpSize, &numberOfCells, &numberOfProbes );
    DeleteSystem_ThEl( pointerStructureAddress.get() );
    // mdlInitializeSizes

    double savedTimeFromLastRun = 0.0;
    double current = 0.0;
    std::vector< const real_T * > balancingCurrentVector;

    std::vector< real_T > simulinkStates( stateSize, 0 );
    std::vector< real_T > simulinkDxdt( stateSize, 0 );
    std::vector< real_T > simulinkV( vcpSize, 0 );
    std::vector< real_T > simulinkC( vcpSize, 0 );
    std::vector< real_T > simulinkP( vcpSize, 0 );
    std::vector< real_T > simulinkSOC( vcpSize, 0 );
    std::vector< real_T > simulinkSurfaceSOC( vcpSize, 0 );
    std::vector< real_T > simulinkT( numberOfCells, 0 );
    std::vector< real_T > simulinkTotalV( 1, 0 );
    std::vector< real_T > simulinkTotalC( 1, 0 );
    std::vector< real_T > simulinkTotalP( 1, 0 );
    std::vector< real_T > totalNumberOfBatteries( 1, 0 );
    std::vector< real_T > probeOutput( numberOfProbes + 1, 0 );    // + 1 needed as the probeSize itself is zero

    real_T *stateVector = &simulinkStates.at( 0 );
    real_T *dxdtVector = &simulinkDxdt.at( 0 );
    real_T *OutputPortRealSignalV = &simulinkV.at( 0 );
    real_T *OutputPortRealSignalC = &simulinkC.at( 0 );
    real_T *OutputPortRealSignalP = &simulinkP.at( 0 );
    real_T *OutputPortRealSignalSOC = &simulinkSOC.at( 0 );
    real_T *OutputPortRealSignalSurfaceSOC = &simulinkSurfaceSOC.at( 0 );
    real_T *OutputPortRealSignalT = &simulinkT.at( 0 );
    real_T *OutputPortRealSignalTotalV = &simulinkTotalV.at( 0 );
    real_T *OutputPortRealSignalTotalC = &simulinkTotalC.at( 0 );
    real_T *OutputPortRealSignalTotalP = &simulinkTotalP.at( 0 );
    real_T *OutputPortRealSignalNumberBatteries = &totalNumberOfBatteries.at( 0 );


    // mdlStart
    InitializeSystem_ThEl( config, pointerStructureAddress.get(), stateVector );
    const size_t knownElectricalSize = 11;
    const size_t knownThermalSize = 56;

    for ( size_t i = 0; i < simulinkDxdt.size(); ++i )
    {
        TS_ASSERT_DELTA( simulinkDxdt.at( i ), 0, 0.0000001 );
    }

    for ( size_t i = 0; i < knownElectricalSize; ++i )
    {
        TS_ASSERT_DELTA( simulinkStates.at( i ), 0, 0.0000001 );
    }
    for ( size_t i = 0; i < knownThermalSize; ++i )
    {
        TS_ASSERT_DELTA( simulinkStates.at( knownElectricalSize + i ), 27, 0.0000001 );
    }
    TS_ASSERT_EQUALS( simulinkStates.size(), knownElectricalSize + knownThermalSize );


    // Skipped GetRealSizes
    SetOutputVectors_ThEl( pointerStructureAddress.get(), OutputPortRealSignalV, OutputPortRealSignalC,
                           OutputPortRealSignalP, OutputPortRealSignalSOC, OutputPortRealSignalSurfaceSOC );
    OutputPortRealSignalNumberBatteries[0] = numberOfCells;
    TS_ASSERT_EQUALS( totalNumberOfBatteries.at( 0 ), 2 );
    // mdlStart

    unsigned int currentSwitchCounter = 0;
    // double previousCurrent[2] = {0.0};
    double previousVoltage[2] = {0.0, 0.0};
    double previousSoc[2] = {0.0, 0.0};
    // double previousHeatGeneration[2] = {0.0};
    double thisCurrent[2] = {0.0, 0.0};
    double thisVoltage[2] = {0.0, 0.0};
    double thisSoc[2] = {0.0, 0.0};
    double thisHeatGeneration[2] = {0.0, 0.0};
    double thisTemperature[2] = {0.0, 0.0};
    double lastHeatBeforeCurrentChange[2] = {0.0, 0.0};

    // Emulates Simulink engine
    double taskTime = 0.0;
    const double simulinkDt = 0.1;

    // mdlOutputs
    // mdlOutputs

    while ( taskTime < 100.0 )
    {
        // mdlOutputs
        // mdlOutputs

        // Test assertions
        for ( size_t i = 0; i < 2; ++i )
        {
            // previousCurrent[i] = thisCurrent[i];
            previousVoltage[i] = thisVoltage[i];
            previousSoc[i] = thisSoc[i];
            // previousHeatGeneration[i] = thisHeatGeneration[i];
            thisCurrent[i] = OutputPortRealSignalC[i];
            thisVoltage[i] = OutputPortRealSignalV[i];
            thisSoc[i] = OutputPortRealSignalSOC[i];
            thisHeatGeneration[i] = OutputPortRealSignalP[i];
            thisTemperature[i] = OutputPortRealSignalT[i];
            // std::cout << i << ", " << taskTime << ", " << thisCurrent[i] << ", " << thisVoltage[i] << ", " <<
            // thisSoc[i] << ", " << thisHeatGeneration[i] << ", " << thisTemperature[i] << std::endl;
            if ( currentSwitchCounter == 2 )
            {
                if ( i == 1 )
                {
                    TS_ASSERT_DELTA( thisCurrent[i], 10.0 / 2.0 - 0.1, 0.2 );
                    TS_ASSERT_DELTA( thisCurrent[i] + thisCurrent[0], 10.0, sDelta );
                }
                else
                {
                    TS_ASSERT_DELTA( thisCurrent[i], 10.0 / 2.0 + 0.1, 0.2 );
                }
                TS_ASSERT_LESS_THAN( previousVoltage[i], thisVoltage[i] );
                TS_ASSERT_LESS_THAN( previousSoc[i], thisSoc[i] );
                TS_ASSERT_LESS_THAN( thisTemperature[i], 27.04 );
                TS_ASSERT_LESS_THAN( 26.95, thisTemperature[i] );
            }
            else if ( currentSwitchCounter == 4 )
            {
                if ( i == 1 )
                {
                    TS_ASSERT_DELTA( thisCurrent[i], -8.0 / 2.0 - 0.1, 0.2 );
                    TS_ASSERT_DELTA( thisCurrent[i] + thisCurrent[0], -8.0, sDelta );
                }
                else
                {
                    TS_ASSERT_DELTA( thisCurrent[i], -8.0 / 2.0 + 0.1, 0.2 );
                }
                TS_ASSERT_LESS_THAN( thisVoltage[i], previousVoltage[i] );
                TS_ASSERT_LESS_THAN( thisSoc[i], previousSoc[i] );
                TS_ASSERT_LESS_THAN( thisHeatGeneration[i], lastHeatBeforeCurrentChange[i] );
                TS_ASSERT_LESS_THAN( thisTemperature[i], 27.04 );
                TS_ASSERT_LESS_THAN( 26.95, thisTemperature[i] );
            }
            else if ( currentSwitchCounter == 6 )
            {
                if ( i == 1 )
                {
                    TS_ASSERT_DELTA( thisCurrent[i], 0.0 - 0.1, 0.2 );
                    TS_ASSERT_DELTA( thisCurrent[i] + thisCurrent[0], 0.0, sDelta );
                }
                else
                {
                    TS_ASSERT_DELTA( thisCurrent[i], 0.0 + 0.1, 0.2 );
                }
                TS_ASSERT_LESS_THAN( previousVoltage[i], thisVoltage[i] );
                TS_ASSERT_DELTA( previousSoc[i], thisSoc[i], 0.2 );
                TS_ASSERT_LESS_THAN( thisHeatGeneration[i], lastHeatBeforeCurrentChange[i] );
                TS_ASSERT_LESS_THAN( thisTemperature[i], 27.04 );
                TS_ASSERT_LESS_THAN( 26.95, thisTemperature[i] );
            }
        }

        switch ( currentSwitchCounter )
        {
            case 0:
            case 1:
            case 3:
            case 5:
                ++currentSwitchCounter;
                break;
            case 2:
                if ( taskTime >= 30.0 )
                {
                    lastHeatBeforeCurrentChange[0] = thisHeatGeneration[0];
                    lastHeatBeforeCurrentChange[1] = thisHeatGeneration[1];
                    ++currentSwitchCounter;
                }
                break;
            case 4:
                if ( taskTime >= 60.0 )
                {
                    lastHeatBeforeCurrentChange[0] = thisHeatGeneration[0];
                    lastHeatBeforeCurrentChange[1] = thisHeatGeneration[1];
                    ++currentSwitchCounter;
                }
                break;
            default:
                break;
        }
        // Test assertions

        // Set current
        if ( taskTime < 30.0 )
            current = 10.0;
        else if ( taskTime < 60.0 )
            current = -8.0;
        else
            current = 0.0;
        // Set current

        // Set simulink time
        taskTime += simulinkDt;
        // Set simulink time

        // mdlOutput
        // Calculate last dt
        double dt = taskTime - savedTimeFromLastRun;
        savedTimeFromLastRun = taskTime;

        // Get input and set current
        SetCurrent_ThEl( pointerStructureAddress.get(), current );    //< Sets the new current
        if ( !balancingCurrentVector.empty() )
            SetBalancingCurrent_ThEl( pointerStructureAddress.get(), &balancingCurrentVector.at( 0 ) );

        // Run System
        RunAlgebraicElectricalSystemPart_ThEl( pointerStructureAddress.get(), stateVector, dt );


        // Set output vectors
        OutputRootElementData_ThEl( pointerStructureAddress.get(), OutputPortRealSignalTotalV,
                                    OutputPortRealSignalTotalC, OutputPortRealSignalTotalP );
        TriggerObserverAndCellTemperatureOutput_ThEl( pointerStructureAddress.get(), taskTime, OutputPortRealSignalT,
                                                      &probeOutput.at( 0 ) );
        // mdlDerivatives
        RunSystem_ThEl( pointerStructureAddress.get(), stateVector, dxdtVector );

        OutputRootElementData_ThEl( pointerStructureAddress.get(), OutputPortRealSignalTotalV,
                                    OutputPortRealSignalTotalC, OutputPortRealSignalTotalP );
        TriggerObserverAndCellTemperatureOutput_ThEl( pointerStructureAddress.get(), taskTime, OutputPortRealSignalT,
                                                      &probeOutput.at( 0 ) );

        // Solve with implicit Euler
        for ( size_t i = 0; i < stateSize; ++i )
            stateVector[i] += dxdtVector[i] * simulinkDt;
    }

    // mdlTerminate
    DeleteSystem_ThEl( pointerStructureAddress.get() );
    // mdlTerminate
}

void TestCinterface_ThEl::TestScenarioDiscreteSimuDGL()
{
    boost::scoped_ptr< size_t > pointerStructureAddress( new size_t( 0 ) );
    size_t realStateSize = 0;
    size_t realOutputSize = 0;
    size_t realNumberOfCells = 0;
    size_t definedStateSize = 0;
    size_t definedOutputSize = 0;
    size_t definedNumberOfCells = 0;
    size_t numberOfProbes = 0;
    std::string line, file;
    std::ifstream xmlFile( "TestSimpleBlockRCBattery.xml" );
    if ( !xmlFile.is_open() )
    {
        TS_FAIL( "File not found" );
        return;
    }
    while ( std::getline( xmlFile, line ) )
        file += line + "\n";
    const char *configStr = file.c_str();

    // mdlInitializeSizes
    GetRealSizes_ThEl( configStr, 0, &realStateSize, &realOutputSize, &realNumberOfCells, &numberOfProbes );
    GetDefinedSizes( configStr, &definedStateSize, &definedOutputSize, &definedNumberOfCells, &numberOfProbes );

    const size_t initialNumberOfCells = PickLargerSize( definedNumberOfCells, realNumberOfCells );
    const size_t initialOutputSize = PickLargerSize( definedOutputSize, realOutputSize );
    const size_t initialStateSize = PickLargerSize( definedStateSize, realStateSize );

    // simu_dgl now deletes configStr

    // Initilizes the input ports
    double current = 0.0;
    std::vector< const real_T * > balancingCurrentVector;    // empty vector, balancing is not tested here
    // no Air temperature
    // no reset port

    TS_ASSERT_EQUALS( initialNumberOfCells, 2 );
    TS_ASSERT_EQUALS( initialOutputSize, 2 );
    TS_ASSERT_EQUALS( initialStateSize, 4 );

    // Initilize the output ports, discret states vector and dxdt vector
    std::vector< real_T > simulinkDiscStates( initialStateSize, 0 );
    std::vector< real_T > simulinkDxdt( initialStateSize, 0 );
    std::vector< real_T > simulinkV( initialOutputSize, 0 );
    std::vector< real_T > simulinkC( initialOutputSize, 0 );
    std::vector< real_T > simulinkP( initialOutputSize, 0 );
    std::vector< real_T > simulinkSOC( initialOutputSize, 0 );
    std::vector< real_T > simulinkSurfaceSOC( initialOutputSize, 0 );
    std::vector< real_T > simulinkT( initialNumberOfCells, 0 );
    std::vector< real_T > simulinkTotalV( 1, 0 );
    std::vector< real_T > simulinkTotalC( 1, 0 );
    std::vector< real_T > simulinkTotalP( 1, 0 );
    std::vector< real_T > totalNumberOfBatteries( 1, 0 );
    std::vector< real_T > probeOutput( numberOfProbes, 0 );

    real_T *discStateVector = &simulinkDiscStates.at( 0 );
    real_T *dxdtVector = &simulinkDxdt.at( 0 );
    real_T *OutputPortRealSignalV = &simulinkV.at( 0 );
    real_T *OutputPortRealSignalC = &simulinkC.at( 0 );
    real_T *OutputPortRealSignalP = &simulinkP.at( 0 );
    real_T *OutputPortRealSignalSOC = &simulinkSOC.at( 0 );
    real_T *OutputPortRealSignalSurfaceSOC = &simulinkSurfaceSOC.at( 0 );
    real_T *OutputPortRealSignalT = &simulinkT.at( 0 );
    real_T *OutputPortRealSignalTotalV = &simulinkTotalV.at( 0 );
    real_T *OutputPortRealSignalTotalC = &simulinkTotalC.at( 0 );
    real_T *OutputPortRealSignalTotalP = &simulinkTotalP.at( 0 );
    real_T *OutputPortRealSignalNumberBatteries = &totalNumberOfBatteries.at( 0 );
    // mdlInitializeSizes

    // mdlInitializeSampleTimes
    // nothing do test here
    // mdlInitializeSampleTimes

    // mdlStart
    InitializeSystem_ThEl( configStr, pointerStructureAddress.get(), discStateVector );
    // Skipped GetRealSizes
    // Assign the output vectors to the observer
    SetOutputVectors_ThEl( pointerStructureAddress.get(), OutputPortRealSignalV, OutputPortRealSignalC,
                           OutputPortRealSignalP, OutputPortRealSignalSOC, OutputPortRealSignalSurfaceSOC );
    OutputPortRealSignalNumberBatteries[0] = initialNumberOfCells;

    TS_ASSERT_EQUALS( totalNumberOfBatteries.at( 0 ), 2 );

    RunAlgebraicElectricalSystemPartDiscret_ThEl( pointerStructureAddress.get(), 0.0 );
    // mdlStart

    // Emulates Simulink engine
    double simulinkTaskTime = 0.0;
    double lastTime = 0.0;
    const double simulinkDt = 0.001;
    double dt = 0.0;


    // Values equal to xml-configuration on top. Used to compute analytical solution for comparison
    // analytic solution
    const double rOhm = 2.0;
    const double tau = 2.0;
    const double rC = 1.0;
    const double nbrBat = 2.0;
    const double rho = 1.0;
    const double cp = 1000.0;
    const double vol = 0.000008;
    const double maxCapacity = 1.0;

    double factor = 0.0;
    double exponent = 0.0;
    double tStart = 0.0;
    double tStartOld = -1.0;
    double uC = 0.0;
    double duCdt = 0.0;
    double prevDuCdt = 0.0;
    double uOhm = 0.0;
    double uTotal = 0.0;
    double pCell = 0.0;
    double prevPCell = 0.0;
    double pTotal = 0.0;
    double socRef = 0.0;
    double Tref = 20.0;
    // analytic solution


    while ( simulinkTaskTime < 30.0 )
    {
        // mdlOutputs
        dt = simulinkTaskTime - lastTime;
        lastTime = simulinkTaskTime;

        // Set current
        if ( simulinkTaskTime < 30.0 )
        {
            current = 10.0;
            tStart = 0.0;
        }
        else if ( simulinkTaskTime < 60.0 )
        {
            current = -8.0;
            tStart = 30.0;
        }
        else
        {
            current = 0.0;
            tStart = 60.0;
        }
        // Set current

        // analytic solution
        if ( tStartOld < tStart )
        {
            tStartOld = tStart;
            factor = -current * rC + uC;
        }
        exponent = -( simulinkTaskTime - tStart ) / tau;
        uC = current * rC + factor * exp( exponent );
        duCdt = -factor / tau * exp( exponent );
        uOhm = current * rOhm;
        uTotal = nbrBat * ( uOhm + uC );
        pCell = current * current * rOhm + uC * uC / rC;
        pTotal = nbrBat * pCell;
        socRef += current * dt / ( maxCapacity * 3600.0 ) * 100.0;
        Tref += pCell / ( rho * cp * vol ) * dt;
        // analytic solution


        SetCurrent_ThEl( pointerStructureAddress.get(), current );

        IntegrateOneDiscreteStep( pointerStructureAddress.get(), dt, discStateVector, dxdtVector );

        OutputRootElementData_ThEl( pointerStructureAddress.get(), OutputPortRealSignalTotalV,
                                    OutputPortRealSignalTotalC, OutputPortRealSignalTotalP );
        TriggerObserverAndCellTemperatureOutput_ThEl( pointerStructureAddress.get(), simulinkTaskTime,
                                                      OutputPortRealSignalT, &probeOutput.at( 0 ) );
        // mdlOutputs

        // Test assertions

        // totalVoltage
        TS_ASSERT_DELTA( *OutputPortRealSignalTotalV, uTotal, eulerDelta );
        // totalCurrent
        TS_ASSERT_DELTA( *OutputPortRealSignalTotalC, current, sDelta );
        // totalPower
        TS_ASSERT_DELTA( *OutputPortRealSignalTotalP, pTotal, eulerDelta2 );

        // voltage
        TS_ASSERT_DELTA( OutputPortRealSignalV[0], OutputPortRealSignalV[1],
                         sDelta );    // two equal batteries serial connected
        TS_ASSERT_DELTA( OutputPortRealSignalV[0], uC + uOhm, eulerDelta );

        // current
        TS_ASSERT_DELTA( OutputPortRealSignalC[0], OutputPortRealSignalC[1],
                         sDelta );    // two equal batteries serial connected
        TS_ASSERT_DELTA( OutputPortRealSignalC[0], current, sDelta );

        // power
        TS_ASSERT_DELTA( OutputPortRealSignalP[0], OutputPortRealSignalP[1],
                         sDelta );    // two equal batteries serial connected
        TS_ASSERT_DELTA( OutputPortRealSignalP[0], pCell, eulerDelta2 );

        // SOC
        TS_ASSERT_DELTA( OutputPortRealSignalSOC[0], OutputPortRealSignalSOC[1],
                         sDelta );    // two equal batteries serial connected
        TS_ASSERT_DELTA( OutputPortRealSignalSOC[0], socRef, sDelta );

        // Temperature
        TS_ASSERT_DELTA( OutputPortRealSignalT[0], OutputPortRealSignalT[1],
                         sDelta );    // equal thermal behaviour and init state
        TS_ASSERT_DELTA( OutputPortRealSignalT[0], Tref, Tref * eulerDelta );

        // state vector
        TS_ASSERT_DELTA( discStateVector[0], discStateVector[1], sDelta );
        TS_ASSERT_DELTA( discStateVector[0], uC, eulerDelta );
        TS_ASSERT_DELTA( discStateVector[2], discStateVector[3], sDelta );
        TS_ASSERT_DELTA( discStateVector[2], Tref, Tref * eulerDelta );

        // dxdtVector
        TS_ASSERT_DELTA( dxdtVector[0], dxdtVector[1], sDelta );
        TS_ASSERT_DELTA( dxdtVector[0], prevDuCdt, eulerDelta );
        prevDuCdt = duCdt;    // the internal integration, integrates from the previous time step to the actual time
                              // step, therefore dxdt is shifted
        TS_ASSERT_DELTA( dxdtVector[2], dxdtVector[3], sDelta );
        TS_ASSERT_DELTA( dxdtVector[2], prevPCell / ( rho * vol * cp ), prevPCell / ( rho * vol * cp ) * eulerDelta );
        prevPCell = pCell;    // the internal integration, integrates from the previous time step to the actual time
                              // step, therefore dxdt is shifted

        // Test assertions

        // Set simulink time
        simulinkTaskTime += simulinkDt;
        // Set simulink time
    }

    // mdlTerminate
    DeleteSystem_ThEl( pointerStructureAddress.get() );
    // mdlTerminate
}
