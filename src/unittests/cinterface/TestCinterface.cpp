/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestCinterface.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 28 Aug 2015 15:57:21 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestCinterface.h"
#include "../../cinterface/cinterface.h"

static const double sDelta = 0.000001;


const char *TestCinterface::ConfigWithCellelement()
{
    const char *xmlConfig =
     "<?xml version='1.0'?>\
        <Configuration>\
        <Options>\
            <MinStateSize>2</MinStateSize>\
            <MinOutputSize>3</MinOutputSize>\
            <SampleRate>100000000</SampleRate>\
        </Options>\
        <CustomDefinitions>\
          <CellElement class='CellElement' observable='TRUE'>\
            <ThermalState class='ThermalState' cache='True'>\
                <InitialTemperature>23</InitialTemperature>\
            </ThermalState>\
            <Soc class='Soc' cache='True'>\
                <InitialCapacity>2.05</InitialCapacity>\
                <InitialSoc>20</InitialSoc>\
                <MeasurementPoints>5,10,20,23,35,50,65,80,90</MeasurementPoints>\
            </Soc>\
            <Children>\
            <OhmicResistance class='OhmicResistance'>\
               <Object class='LookupObj2dWithState'>\
                  <RowState cacheref='ThermalState'/>\
                  <ColState cacheref='Soc'/>\
                  <LookupData desc='Rser'>\
                0.0288200000000001, 0.0273753907300395, 0.0264344729259093, 0.0135127660821939;\
                0.0288200000000001, 0.0273753907300395, 0.0270767085789135, 0.0275280515729565;\
                0.0290500142586083, 0.0277525443197526, 0.0275261486868454, 0.0276368633915343;\
                0.0288630838514731, 0.0278576890117617, 0.0275537618404411, 0.0277933657755191;\
                0.0286781206123875, 0.0277738617773271, 0.0275453220226692, 0.0259269199477185;\
                0.0285331061111646, 0.0277867915477187, 0.0268358840093433, 0.0267808653711796;\
                0.0286358289536196, 0.0277803384908296, 0.0257328031445100, 0.0272959924289106\
                  </LookupData>\
                  <MeasurementPointsRow desc='MeasureTemp'>0,10,23,50</MeasurementPointsRow>\
                  <MeasurementPointsColumn desc='StateOfCharge'>5,20,35,50,65,80,90</MeasurementPointsColumn>\
               </Object>\
            </OhmicResistance>\
            <VoltageSource class='VoltageSource'>\
               <Object class='LookupObj2dWithState'>\
                  <RowState cacheref='Soc'/>\
                  <ColState cacheref='ThermalState'/>\
                  <LookupData desc='OCV'>\
                3.47980625000000, 3.56020000000000, 3.61080000000000, 3.64033750000000, 3.67903750000000, 3.74791250000000, 3.84935000000000, 3.94420000000000, 4.05327500000000, 4.18230000000000;\
                3.48386250000000, 3.56396250000000, 3.61388750000000, 3.64350000000000, 3.68183750000000, 3.75231250000000, 3.85146875000000, 3.94563750000000, 4.05550000000000, 4.18410000000000;\
                3.48390000000000, 3.56456250000000, 3.61843125000000, 3.64930000000000, 3.68808750000000, 3.76500000000000, 3.85450000000000, 3.94680000000000, 4.05477500000000, 4.17770000000000;\
                3.47570000000000, 3.55060000000000, 3.60896250000000, 3.64975000000000, 3.68677500000000, 3.75358750000000, 3.84280000000000, 3.93626250000000, 4.04295000000000, 4.16480000000000 \
                  </LookupData>\
                  <MeasurementPointsRow desc='StateOfChargeOCV'>10,20,30,40,50,60,70,80,90,100</MeasurementPointsRow>\
                  <MeasurementPointsColumn desc='MeasureTemp'>0,10,23,50</MeasurementPointsColumn>\
               </Object>\
            </VoltageSource>\
            <ZarcElement class='ZarcElement'>\
                <LookupTau class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    1.12980227037822e-01, 1.71749789914134e-01, 4.27878577005388e-01, 4.70584769553997e-01;\
                    2.88029269499999e-01, 2.03069924403468e-01, 7.59434298204209e-02, 5.38248547348156e-02;\
                    1.80149147116020e-01, 1.12630469688383e-01, 3.52185854766679e-02, 2.98230802877780e-02;\
                    1.47909096582298e-01, 8.33921914682953e-02, 2.57060565921076e-02, 2.22501291807582e-02;\
                    1.32900242338032e-01, 7.22938740353130e-02, 2.05480235442034e-02, 9.95886997764575e-02;\
                    1.19852623435413e-01, 6.56657395160509e-02, 2.36971844841798e-02, 1.02914420936082e-01;\
                    1.18273000586515e-01, 6.60491071602289e-02, 3.25921294643322e-02, 1.09197268465146e-01 \
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupTau>\
                <LookupOhmicResistance class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788;\
                    0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630;\
                    0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295;\
                    0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453;\
                    0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745;\
                    0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180;\
                    0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupOhmicResistance>\
                <LookupPhi class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>0.574590000000000,0.516891694022437,0.408379079339744,0.110443124808620;0.574590000000000,0.516891694022437,0.568073405078093,0.566435038531618;0.616859078283881,0.579111165549142,0.662374262351899,0.620812008168125;0.625502091112064,0.610693210867254,0.697319797270455,0.671812046180353;0.621586176902636,0.620972340223097,0.721715387830978,0.323239015612757;0.620607929391110,0.630419920219851,0.671002168217511,0.371661290927877;0.620534462894923,0.628465004550853,0.585312180039373,0.410302314031108</LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupPhi>\
            </ZarcElement>\
            <ZarcElement class='ZarcElement'>\
                <LookupTau class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788;\
                    0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630;\
                    0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295;\
                    0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453;\
                    0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745;\
                    0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180;\
                    0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupTau>\
                <LookupOhmicResistance class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                        0.0422632965521782,0.0422632965521782,0.0422632965521782,0.0646154311756893;\
                        0.0122909999999999,0.00820305593966980,0.00584662021670523,0.00383743519303631;\
                        0.0322225070656886,0.0289438148360227,0.0225495908462913,0.00599634893454420;\
                        0.0257481760483287,0.0186632656230284,0.00993329376244036,0.00582987109816202;\
                        0.0157426548912461,0.0171684201770746,0.0134953332473813,0.00457332312382527;\
                        0.0218280618476754,0.0131225853688649,0.00741527720123879,0.00329020951414971;\
                        0.0153521701301020,0.00846967601820038,0.00563955480281332,0.00315825717209717\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupOhmicResistance>\
                <LookupPhi class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    1,0.957683980942010,0.762474540690386,0.810166837787303;\
                    1,0.957683980942010,0.837851589303327,0.826206919812180;\
                    0.806534226878276,0.755415801951905,0.630228467428149,0.686969202518359;\
                    0.708526377255388,0.661628608281274,0.630894213874104,0.675415879938948;\
                    0.791964147404381,0.697353484425094,0.628110619592649,0.801166605326404;\
                    0.759343341643463,0.720390317831926,0.755733232495091,0.938336421996388;\
                    0.790468798628622,0.797350100822233,0.852903199927494,0.986742576117900\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupPhi>\
            </ZarcElement>\
            </Children>\
          </CellElement>\
        </CustomDefinitions>\
        <Options>\
            <SampleRate> 100000 </SampleRate>\
        </Options>\
        <RootElement ref='CellElement'/>\
        </Configuration>";
    return xmlConfig;
}

const char *TestCinterface::ConfigWithParallelRC()
{
    const char *xmlConfig =
     "<?xml version='1.0'?>\
        <Configuration>\
        <CustomDefinitions>\
		<myParallelRC class=\"ParallelRC\">\
			<LookupOhmicResistance class=\"ConstObj\">\
                <Value>0.6</Value>\
			</LookupOhmicResistance>\
			<LookupTau class=\"ConstObj\">\
                <Value>0.3</Value>\
			</LookupTau>\
		</myParallelRC>\
        <MyCellElement1 class=\"CellElement\" observable=\"True\">\
            <ThermalState class=\"ThermalState\" cache= \"True\">\
                <InitialTemperature>25</InitialTemperature>\
            </ThermalState>\
            <Soc class=\"Soc\" cache=\"True\">\
                <InitialCapacity>12</InitialCapacity>\
                <InitialSoc>100</InitialSoc>\
                <MeasurementPoints>10, 20, 30, 40, 50, 60, 70, 80, 90, 100</MeasurementPoints>\
            </Soc>\
            <Children>\
                 <ParallelRC ref=\"myParallelRC\"/>\
            </Children>\
        </MyCellElement1>\
       </CustomDefinitions>\
        <Options>\
            <SampleRate> 100000 </SampleRate>\
        </Options>\
        <RootElement class='ParallelTwoPort'>\
            <Children>\
                <RC ref=\"MyCellElement1\"/>\
            </Children>\
        </RootElement>\
        </Configuration>";
    return xmlConfig;
}

void TestCinterface::TestCinterfaceCreating()
{
    size_t pointerStructureAddress = 0;
    const char *config = ConfigWithCellelement();
    InitializeSystem( config, &pointerStructureAddress );
    TS_ASSERT_DIFFERS( pointerStructureAddress, 0 );
    DeleteSystem( &pointerStructureAddress );    //< Free the memory that is used by the DAE
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );
}
void TestCinterface::TestCinterfaceReset()
{
    size_t pointerStructureAddress = 0;
    const char *config = ConfigWithCellelement();
    InitializeSystem( config, &pointerStructureAddress );
    ResetSystem( config, &pointerStructureAddress );
    TS_ASSERT_DIFFERS( pointerStructureAddress, 0 );
    DeleteSystem( &pointerStructureAddress );    //< Free the memory that is used by the DAE
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );
}
void TestCinterface::TestCinterfaceGetSize()
{
    size_t pointerStructureAddress = 0;

    size_t definedStateSize, definedOutputSize;
    InitializeSystem( ConfigWithCellelement(), &pointerStructureAddress, 0, &definedStateSize, &definedOutputSize );
    const size_t realStateSize = GetRealStateSize( &pointerStructureAddress );
    const size_t realObserverSize = GetRealOutputSize( &pointerStructureAddress );

    TS_ASSERT_EQUALS( realStateSize, 6 );
    TS_ASSERT_EQUALS( realObserverSize, 1 );
    TS_ASSERT_EQUALS( definedStateSize, 2 );
    TS_ASSERT_EQUALS( definedOutputSize, 3 );
    DeleteSystem( &pointerStructureAddress );    //< Free the memory that is used by the DAE
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );
}

void TestCinterface::TestCinterfaceSetCurrent()
{
    size_t pointerStructureAddress = 0;

    InitializeSystem( ConfigWithCellelement(), &pointerStructureAddress );
    SetCurrent( &pointerStructureAddress, 0 );
    SetCurrent( &pointerStructureAddress, 2 );
    SetCurrent( &pointerStructureAddress, -2 );
    DeleteSystem( &pointerStructureAddress );    //< Free the memory that is used by the DAE
    TS_ASSERT_EQUALS( pointerStructureAddress, 0 );
}

void TestCinterface::TestCinterfaceRunSystem()
{
    size_t pointerStructureAddress = 0;

    size_t definedStateSize, definedOutputSize;
    InitializeSystem( ConfigWithCellelement(), &pointerStructureAddress, 0, &definedStateSize, &definedOutputSize );
    const size_t realStateSize = GetRealStateSize( &pointerStructureAddress );
    const size_t realObserverSize = GetRealOutputSize( &pointerStructureAddress );
    const size_t statesize = PickLargerSize( realStateSize, definedStateSize );
    const size_t observersize = PickLargerSize( realObserverSize, definedOutputSize );

    const double dt = 0.001;

    std::vector< real_T > x( statesize );
    std::vector< real_T > dxdt( statesize );
    std::vector< real_T > totalVoltage( 1 );
    std::vector< real_T > totalCurrent( 1 );
    std::vector< real_T > totalPower( 1 );
    std::vector< real_T > voltageOutputVec( statesize );
    std::vector< real_T > currentOutputVec( statesize );
    std::vector< real_T > powerOutputVec( statesize );
    std::vector< real_T > socOutputVec( observersize );
    std::vector< real_T > socSurfaceOutputVec( observersize );


    SetOutputVectors( &pointerStructureAddress, &voltageOutputVec.at( 0 ), &currentOutputVec.at( 0 ),
                      &powerOutputVec.at( 0 ), &socOutputVec.at( 0 ), &socSurfaceOutputVec.at( 0 ) );

    SetCurrent( &pointerStructureAddress, 2.05 );

    RunAlgebraicElectricalSystemPart( &pointerStructureAddress, &x.at( 0 ), 0.001 );

    RunSystem( &pointerStructureAddress, &x.at( 0 ), &dxdt.at( 0 ) );

    IntegrateStep( &pointerStructureAddress, &dxdt.at( 0 ), dt );

    TriggerObserver( &pointerStructureAddress, 0 );

    DeleteSystem( &pointerStructureAddress );    //< Free the memory that is used by the DAE
}

void TestCinterface::TestCinterfaceSimulationWithParallelRC()
{
    size_t *pointerStructureAddress = new size_t( 0 );
    size_t stateSize = 0;
    size_t outputSize = 0;

    // mdlInitializeSizes
    InitializeSystem( ConfigWithParallelRC(), pointerStructureAddress, 0 );
    stateSize = GetRealStateSize( pointerStructureAddress );
    outputSize = GetRealOutputSize( pointerStructureAddress );
    DeleteSystem( pointerStructureAddress );
    // mdlInitializeSizes

    double savedTimeFromLastRun = 0.0;
    double current = 0.0;
    std::vector< const real_T * > balancingCurrentVector;

    std::vector< real_T > simulinkStates( stateSize, 0 );
    std::vector< real_T > simulinkDxdt( stateSize, 0 );
    std::vector< real_T > simulinkV( outputSize, 0 );
    std::vector< real_T > simulinkC( outputSize, 0 );
    std::vector< real_T > simulinkP( outputSize, 0 );
    std::vector< real_T > simulinkSOC( outputSize, 0 );
    std::vector< real_T > simulinkSurfaceSOC( outputSize, 0 );
    std::vector< real_T > simulinkTotalV( 1, 0 );
    std::vector< real_T > simulinkTotalC( 1, 0 );
    std::vector< real_T > simulinkTotalP( 1, 0 );
    std::vector< real_T > totalNumberOfBatteries( 1, 0 );

    real_T *stateVector = &simulinkStates.at( 0 );
    real_T *dxdtVector = &simulinkDxdt.at( 0 );
    real_T *OutputPortRealSignalV = &simulinkV.at( 0 );
    real_T *OutputPortRealSignalC = &simulinkC.at( 0 );
    real_T *OutputPortRealSignalP = &simulinkP.at( 0 );
    real_T *OutputPortRealSignalSOC = &simulinkSOC.at( 0 );
    real_T *OutputPortRealSignalSurfaceSOC = &simulinkSurfaceSOC.at( 0 );
    real_T *OutputPortRealSignalTotalV = &simulinkTotalV.at( 0 );
    real_T *OutputPortRealSignalTotalC = &simulinkTotalC.at( 0 );
    real_T *OutputPortRealSignalTotalP = &simulinkTotalP.at( 0 );
    real_T *OutputPortRealSignalNumberBatteries = &totalNumberOfBatteries.at( 0 );


    // mdlStart
    // InitializeSystem
    InitializeSystem( ConfigWithParallelRC(), pointerStructureAddress, stateVector );

    SetOutputVectors( pointerStructureAddress, OutputPortRealSignalV, OutputPortRealSignalC, OutputPortRealSignalP,
                      OutputPortRealSignalSOC, OutputPortRealSignalSurfaceSOC );
    OutputPortRealSignalNumberBatteries[0] = GetRealOutputSize( pointerStructureAddress );
    TS_ASSERT_DELTA( totalNumberOfBatteries[0], 1.0, sDelta );
    // mdlStart

    unsigned char currentSwitchCounter = 0;
    double lastCurrentSwitchTime = 0.0;
    double lastVoltageValue = 0.0;
    double lastExpectedValue = 0.0;

    // Emulates Simulink engine
    double taskTime = 0.0;
    const double simulinkDt = 0.01;

    // mdlOutputs
    // mdlOutputs

    // printf("\n\n[");
    while ( taskTime < 6.0 )
    {
        // mdlOutputs
        // mdlOutputs

        // Test assertions
        TS_ASSERT_DELTA( OutputPortRealSignalV[0], lastExpectedValue, 0.01 );
        // printf("%f,%f,%f;", taskTime, OutputPortRealSignalV[0], lastExpectedValue);
        if ( taskTime < 1.0 + simulinkDt )
            lastExpectedValue = 2.0 * 0.6 * ( 1 - exp( -( taskTime - 0.0 ) / 0.3 ) );
        else if ( taskTime < 3.0 + simulinkDt )
        {
            if ( currentSwitchCounter == 0 )
            {
                lastCurrentSwitchTime = 1.0;
                lastVoltageValue = OutputPortRealSignalTotalV[0];
                ++currentSwitchCounter;
            }
            lastExpectedValue = ( lastVoltageValue + 0.4 * 0.6 ) * exp( -( taskTime - lastCurrentSwitchTime ) / 0.3 ) - 0.4 * 0.6;
        }
        else
        {
            if ( currentSwitchCounter == 1 )
            {
                lastCurrentSwitchTime = 3.0;
                lastVoltageValue = OutputPortRealSignalTotalV[0];
                ++currentSwitchCounter;
            }
            lastExpectedValue = lastVoltageValue * exp( -( taskTime - lastCurrentSwitchTime ) / 0.3 );
        }
        // Test assertions

        // Set current
        if ( taskTime < 1.0 )
            current = 2.0;
        else if ( taskTime < 3.0 )
            current = -0.4;
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
        SetCurrent( pointerStructureAddress, current );    //< Sets the new current
        if ( !balancingCurrentVector.empty() )
            SetBalancingCurrent( pointerStructureAddress, &balancingCurrentVector[0] );

        // Run System
        RunAlgebraicElectricalSystemPart( pointerStructureAddress, stateVector, dt );

        // Set Output
        OutputRootElementData( pointerStructureAddress, OutputPortRealSignalTotalV, OutputPortRealSignalTotalC,
                               OutputPortRealSignalTotalP );
        TriggerObserver( pointerStructureAddress, taskTime );
        // mdlDerivatives
        RunSystem( pointerStructureAddress, stateVector, dxdtVector );

        // Solve with implicit Euler
        for ( size_t i = 0; i < stateSize; ++i )
            stateVector[i] += dxdtVector[i] * simulinkDt;
    }
    // printf("];\n\n");

    // mdlTerminate
    DeleteSystem( pointerStructureAddress );
    delete pointerStructureAddress;
    // mdlTerminate
}
