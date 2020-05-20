#include "standalone.h"

#ifndef _SYMBOLIC_

#include <fstream>

#include "../src/container/matio_file.h"
#include "../src/factory/factorybuilder_for_aging.h"
#include "../src/misc/fast_copy_matrix.h"
#include "../src/version/version.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, true >;
extern template class electrical::TimeSeries< double, electrical::EvalNoInterpolation >;
extern template class simulation::ThermalSimulation< myMatrixType, double, true >;
extern template class observer::ThermalObserver< double >;
extern template class electrical::Cellelement< myMatrixType >;
#ifdef BUILD_AGING_SIMULATION
extern template class simulation::AgingSimulation< myMatrixType, double, true >;
extern template class observer::AgingObserver< myMatrixType >;
#endif

const double thermal::globalMaxValue = 1000000000.0;
const double thermal::globalMinValue = -thermal::globalMaxValue;

namespace standalone
{

Standalone::Standalone( std::string name )
    : mQuiet( false )
    , mStepTime( 1.0 )
    , mSocStopCriterion( 5 )
    , mPowerStopCriterion( 1 )
    , mThermalStopCriterion( 5.0 )
    , mCycles( 1 )
    , mAgingCycles( 1 )
    , mAgingSteps( 1 )
    , mSteadyStateCycles( 0 )
    , mApp( name )
    , mProfileChangeTime( 0.0 )
{
    AddOptions();
}

void Standalone::PrintVersion( int /* count */ )
{
    printf( "Version: %s\n", VERSION_NUMBER );
    throw CLI::Success();
}

void Standalone::AddOptions()
{
    mApp.add_flag_function( "-v,--version", PrintVersion, "Display version information and exit" )->short_circuit();
    mApp.add_flag( "-q,--quiet", mQuiet, "Suppress almost all output" );
    mApp.add_option( "xml-file", mXmlFilename, "XML configuration file" )->required();
    mApp.add_option( "current-profile", mProfileFilename, "Current profile" )->required();
}

bool Standalone::ParseCommandLine( int argc, char *argv[] )
{
    CLI11_PARSE( mApp, argc, argv );

    try
    {
        mParser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        mParser->ReadFromFile( mXmlFilename );
    }
    catch ( std::exception &e )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "%s\n", e.what() );
        return false;
    }

    try
    {
        if ( mProfileFilename.rfind( ".mat" ) != std::string::npos )
        {
            matlab::MatFile matFile( mProfileFilename );
            matlab::MatioData &matStruct = matFile["diga"]["daten"];
            std::vector< double > time = matStruct["Programmdauer"].operator()< matlab::MatioData::X_DIM >( 0 );
            std::vector< double > current = matStruct["Strom"].operator()< matlab::MatioData::X_DIM >( 0 );
            mProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( time, current ) );
        }
        else
        {
            std::ifstream profileFile( mProfileFilename );
            mProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( profileFile ) );
        }
    }
    catch ( const std::exception &e )
    {
        printf( "At 2. parameter: profile\n" );
        printf( "%s\n", e.what() );
        return false;
    }
    return true;
}

void Standalone::ReadOptions()
{
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = mParser->GetRoot()->GetElementChild( "Options" );
    if ( optionsNode->HasElementDirectChild( "StepTime" ) )
        mStepTime = optionsNode->GetElementDoubleValue( "StepTime" );
    else
        printf( "Could not read steptime from XML file, assuming default value of %.1f\n", mStepTime );
    if ( optionsNode->HasElementDirectChild( "PowerStopCriterion" ) )
        mPowerStopCriterion = optionsNode->GetElementDoubleValue( "PowerStopCriterion" );
    if ( optionsNode->HasElementDirectChild( "SocStopCriterion" ) )
        mSocStopCriterion = optionsNode->GetElementDoubleValue( "SocStopCriterion" );
    if ( optionsNode->HasElementDirectChild( "ThermalStopCriterionInDegreeC" ) )
        mThermalStopCriterion = optionsNode->GetElementDoubleValue( "ThermalStopCriterionInDegreeC" );
    if ( optionsNode->HasElementDirectChild( "Cycles" ) )
        mCycles = optionsNode->GetElementUnsignedIntValue( "Cycles" );

    // aging options
    if ( !optionsNode->HasElementDirectChild( "Aging" ) )
        return;
    boost::shared_ptr< xmlparser::XmlParameter > agingNode = optionsNode->GetElementChild( "Aging" );
    mAgingCycles = agingNode->GetElementUnsignedIntValue( "CalculationCycles" );
    mAgingSteps = agingNode->GetElementUnsignedIntValue( "AgingSteps" );
    mSteadyStateCycles = agingNode->GetElementUnsignedIntValue( "SteadyStateCycles" );
    mAgingStepTime = agingNode->GetElementDoubleValue( "AgingStepTime" );
    if ( agingNode->HasElementDirectChild( "ResetThermalElectricalStates" ) )
        mResetThElStates = agingNode->GetElementBoolValue( "ResetThermalElectricalStates" );
    else
        mResetThElStates = true;
    if ( mSteadyStateCycles >= mAgingCycles )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TooManySteadyStateCycles" );
}

bool Standalone::CreateElectricalSimulation( factory::FactoryBuilder< myMatrixType, double > *factoryBuilder )
{
    try
    {
        mElectricalSimulation.reset( new simulation::ElectricalSimulation< myMatrixType, double, true >(
         mParser->GetRoot(), mStepTime, mProfile->GetMaxTime(), mSocStopCriterion, &mCells, factoryBuilder ) );

        if ( mElectricalSimulation->mObserver->GetObservedTwoPortsPtr().empty() )
        {
            if ( mElectricalSimulation->mRootTwoPort )
            {
                mElectricalSimulation->mObserver->SetRoot( mElectricalSimulation->mRootTwoPort );
            }
        }

        if ( mProfile->GetType() == electrical::TimeSeriesType::POWER )
        {
            mElectricalSimulation->SetPowerStopCriterion( mPowerStopCriterion );
        }

// create equation solvers
#if defined( _ARMADILLO_ ) && not defined( SPARSE_MATRIX_FORMAT )
        mStepperElectrical =
         make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType >() );
#else
        mStepperElectrical =
         make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > >() );
        mStateVector = std::vector< double >( mElectricalSimulation->mStateSystemGroup.mStateVector.rows(), 0.0 );
#endif
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the electrical simulation\n" );
        printf( "%s", message );
        return false;
    }
    return true;
}

void Standalone::FreeXml() { mParser.reset(); }

void Standalone::SetCurrent()
{
    if ( mElectricalSimulation->mTime >= mProfileChangeTime )
    {
        mProfile->SetTimeAndTriggerEvaluation( mElectricalSimulation->mTime );
        mProfileChangeTime = mProfile->GetTimeUntilMaxValueDeviation( 0.0 );
        if ( mProfile->GetType() == electrical::TimeSeriesType::CURRENT )
        {
            mElectricalSimulation->mRootTwoPort->SetCurrent( mProfile->GetValue() );
            return;
        }
    }

    if ( mProfile->GetType() == electrical::TimeSeriesType::POWER )
    {
        double profileValue = mProfile->GetValue();
        double current;
        double voltage = mElectricalSimulation->mRootTwoPort->GetVoltageValue();
        do
        {
            if ( voltage == 0 )
                voltage = 3;
            current = mProfile->GetValue() / voltage;
            mElectricalSimulation->mRootTwoPort->SetCurrent( current );
            mElectricalSimulation->mRootTwoPort->CalculateStateDependentValues();
            voltage = mElectricalSimulation->mRootTwoPort->GetVoltageValue();
        } while ( std::abs( ( voltage * current / profileValue ) - 1 ) > ( mPowerStopCriterion / 1000 ) );
    }
}

bool Standalone::HasElectricalSimulationEnded() { return mElectricalSimulation->CheckIfSimulationTimeHasEnded(); }

void Standalone::DoElectricalStep()
{
    SetCurrent();
    mElectricalSimulation->StartNewTimeStep();
    while ( mElectricalSimulation->CheckLoopConditionAndSetDeltaTime( mProfileChangeTime ) &&
            !mElectricalSimulation->IsStopCriterionFulfilled() )
    {
        mElectricalSimulation->UpdateSystem();

#if defined( _ARMADILLO_ ) && !defined( SPARSE_MATRIX_FORMAT )
        // Run electrical equation solver

        mStateVector = mElectricalSimulation->mStateSystemGroup.mStateVector.submat(
         0, 0, mElectricalSimulation->mStateSystemGroup.mStateVector.n_rows - 2, 0 );
        mElectricalSimulation->mStateSystemGroup.mDt = mElectricalSimulation->mDeltaTime;
        while ( mStepperElectrical.try_step( boost::ref( *mElectricalSimulation->mEqSystem ), mStateVector,
                                             mElectricalSimulation->mTime,
                                             mElectricalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
        {
            mElectricalSimulation->mStateSystemGroup.mStateVector.submat(
             0, 0, mElectricalSimulation->mStateSystemGroup.mStateVector.n_rows - 2, 0 ) = mStateVector;
            mElectricalSimulation->mStateSystemGroup.mDt = mElectricalSimulation->mDeltaTime;
        }
#else
        // Run electrical equation solver
        misc::FastCopyMatrix( &mStateVector[0], mElectricalSimulation->mStateSystemGroup.mStateVector, mStateVector.size() );
        mElectricalSimulation->mStateSystemGroup.mDt = mElectricalSimulation->mDeltaTime;
        while ( mStepperElectrical.try_step( boost::ref( *mElectricalSimulation->mEqSystem ), mStateVector,
                                             mElectricalSimulation->mTime,
                                             mElectricalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
        {
            mElectricalSimulation->mStateSystemGroup.mDt = mElectricalSimulation->mDeltaTime;
        }
        misc::FastCopyMatrix( mElectricalSimulation->mStateSystemGroup.mStateVector, &mStateVector[0], mStateVector.size() );
#endif

        mElectricalSimulation->UpdateSystemValues();

        // prevents SOCs from changing when calling UpdateSystemValues multiple times
        mElectricalSimulation->mStateSystemGroup.mDt = 0;

        if ( mThermalSimulation )
            mElectricalSimulation->UpdateAllThermalStatesPowerDissipation();
        ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
        mElectricalSimulation->SaveStatesForLaterReset();
    }

    // Finish step
    mElectricalSimulation->FinshStep();

    if ( mAgingSimulation )
        mAgingSimulation->mTime = mElectricalSimulation->mTime;
}

bool Standalone::CreateThermalSimulation()
{
    try
    {
        if ( mElectricalSimulation )
        {
            mThermalSimulation.reset( new simulation::ThermalSimulation< myMatrixType, double, true >(
             mParser->GetRoot(), mStepTime, mProfile->GetMaxTime(), mThermalStopCriterion, &mThermalVisualizer,
             &mElectricalSimulation->mThermalStates, NULL ) );
        }
        else
        {
            mThermalSimulation.reset( new simulation::ThermalSimulation< myMatrixType, double, true >(
             mParser->GetRoot(), mStepTime, mProfile->GetMaxTime(), mThermalStopCriterion, &mThermalVisualizer, NULL, NULL ) );
        }

        mStepperThermal =
         make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );

        if ( mElectricalSimulation )
        {
            // Output conductivity matrix
            std::ofstream fileConduction( "Conductivity.csv" );
            BOOST_FOREACH ( const std::vector< thermal::IndexedValue< double > > &line,
                            mThermalSimulation->mThermalSystem->GetA_th_Conductivity() )
            {
                BOOST_FOREACH ( const thermal::IndexedValue< double > &indexedValue, line )
                    fileConduction << indexedValue.mIndex << ", " << indexedValue.mValue << "; ";
                fileConduction << std::endl;
            }

            // Output finite volumes coordinates
            ofstream AllGridVerticesXYZCoordinates( "AllGridVerticesXYZCoordinates.csv" );
            BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalElement<> > &elem,
                            mThermalSimulation->mThermalSystem->GetThermalElements() )
            {
                AllGridVerticesXYZCoordinates << elem->GetGridVertex().GetX() << ", " << elem->GetGridVertex().GetY()
                                              << ", " << elem->GetGridVertex().GetZ() << std::endl;
            }
        }
        else
        {
            std::cout << "Thermal Size: " << mThermalSimulation->mThermalStates.size() << std::endl;
            std::cout << "Thermal(unconnected) Size: " << mThermalSimulation->mUnconnectedThermalStates.size() << std::endl;
            std::cout << "Thermal(connected) Size: " << mThermalSimulation->mConnectedThermalElements.size() << std::endl;
            SetFixedPowerDissipation();
            mFileVolumeDissipation = std::ofstream( "Patches_Dissapation.csv" );
            mFileVolumeDissipation << mProfile->GetValue() << "\n";
        }
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the thermal simulation\n" );
        printf( "%s", message );
        return false;
    }
    return true;
}

void Standalone::SetFixedPowerDissipation()
{
    if ( mThermalSimulation->mTime >= mProfileChangeTime )
    {
        mProfile->SetTimeAndTriggerEvaluation( mThermalSimulation->mTime );
        double dissipation = mProfile->GetValue();
        for ( auto tState : mThermalSimulation->mThermalStates )
        {
            tState->SetFixedPowerDissipation( dissipation );
        }
        mProfileChangeTime = mProfile->GetTimeUntilMaxValueDeviation( 0.0 );
    }
}

bool Standalone::HasThermalSimulationEnded() { return mThermalSimulation->CheckIfSimulationTimeHasEnded(); }

void Standalone::DoThermalStep()
{
    if ( !mElectricalSimulation )
    {
        SetFixedPowerDissipation();
    }

    mThermalSimulation->InitializeStopCriterion();
    double maxTime;
    if ( mElectricalSimulation )
        maxTime = mElectricalSimulation->mTime;
    else
        maxTime = mProfileChangeTime;
    while ( mThermalSimulation->CheckLoopConditionAndSetDeltaTime( maxTime ) )
    {

        if ( mThermalSimulation->IsStopCriterionFulfilled() )
        {
            if ( mElectricalSimulation )
            {
                printf( "Temperature stop criterion triggered at %.0f sec, electrical states reset\n",
                        mThermalSimulation->mTime );
                mElectricalSimulation->ResetStatesToPointOfTime( mThermalSimulation->mTime );
                return;
            }
            else
            {
                mThermalSimulation->UpdateSystem();
                mThermalSimulation->UpdateAllThermalStatesTemperatures();
                break;
            }
        }

        // Run thermal equation solver
        while ( mStepperThermal.try_step( boost::ref( *mThermalSimulation->mThermalSystem ),
                                          mThermalSimulation->mTemperatures, mThermalSimulation->mTime,
                                          mThermalSimulation->mDeltaTime ) != boost::numeric::odeint::success )
        {
        }

        mThermalSimulation->UpdateSystem();
        mThermalSimulation->UpdateAllThermalStatesTemperatures();
        ( *mThermalVisualizer )( mThermalSimulation->mTime );
        if ( !mElectricalSimulation )
            mFileVolumeDissipation << mProfile->GetValue() << "\n";
    }
}

void Standalone::StartNewCycle()
{
    mProfile->StartNewCycle();
    if ( !mAgingSimulation )
    {
        mElectricalSimulation->UpdateSimulationDuration( mProfile->GetMaxTime() );
        mThermalSimulation->UpdateSimulationDuration( mProfile->GetMaxTime() );
    }
    else
    {
        mAgingSimulation->UpdateThElSimulationDuration( mProfile->GetMaxTime() );
    }
}

#ifdef BUILD_AGING_SIMULATION
bool Standalone::CreateAgingSimulation()
{
    try
    {
        factory::FactoryBuilderForAging< myMatrixType, double > factoryBuilder;
        CreateElectricalSimulation( &factoryBuilder );
        CreateThermalSimulation();
        mAgingSimulation.reset(
         new simulation::AgingSimulation< myMatrixType, double, true >( mParser->GetRoot(), mElectricalSimulation, mThermalSimulation,
                                                                        mCells, mAgingStepTime, &factoryBuilder ) );
        mAgingSimulation->CalculateAging( 0.0, false );
        mAgingSimulation->ResetThElStates();
    }
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the aging simulation\n" );
        printf( "%s", message );
        return false;
    }
    return true;
}

void Standalone::DoAgingStep( size_t agingStep )
{
    mAgingSimulation->CalculateAging( mElectricalSimulation->mSimulationDuration / ( agingStep + 1 ) / ( mSteadyStateCycles + 1 ) );

    std::cout << "Aging Step " << agingStep + 1 << std::endl << std::endl;

    // Calls the aging observer
    ( *mAgingSimulation->mAgingObserver )( agingStep + 1 );

    // Resets the simulation
    if ( agingStep < mAgingSteps - 1 )
    {
        mAgingSimulation->mTime = mProfile->GetMaxTime();
        if ( mResetThElStates )
            mAgingSimulation->ResetThElStates();
        StartNewCycle();
        CreateNewFilters( *mElectricalSimulation->mObserver, *mParser->GetRoot() );
        CreateNewFilters( *mThermalVisualizer, *mParser->GetRoot() );
        InitializeSimulation();
    }
}

void Standalone::CollectAgingData() { mAgingSimulation->CollectCellAgingData(); }

void Standalone::SetCollectAgingData( bool collectAgingData )
{
    mAgingSimulation->SetCollectAgingData( collectAgingData );
}

void Standalone::InitializeSimulation()
{
    // Initialize the simulation
    if ( mElectricalSimulation )
    {
        SetCurrent();
        mElectricalSimulation->UpdateSystem();
        mElectricalSimulation->UpdateSystemValues();
    }

    // add default filters
    if ( !this->mQuiet )
    {
        if ( mElectricalSimulation )
        {
            mElectricalSimulation->mObserver->AddFilter( new observer::DecimateFilterTwoPort< myMatrixType >( mStepTime ) );
            mElectricalSimulation->mObserver->AddFilter( new observer::StdoutFilterTwoPort< myMatrixType >() );
        }
        if ( mThermalSimulation )
        {
            mThermalVisualizer->AddFilter( new observer::DecimateFilterThermal< double >( mStepTime ) );
            mThermalVisualizer->AddFilter( new observer::StdoutFilterThermal< double >() );
        }
        if ( mAgingSimulation && mAgingSimulation->mTime == 0.0 )
        {
            mAgingSimulation->mAgingObserver->AddFilter( new observer::StdoutFilterAging< myMatrixType >() );
        }
    }

    // initial observer calls
    if ( mElectricalSimulation )
        ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
    if ( mThermalSimulation )
        ( *mThermalVisualizer )( mThermalSimulation->mTime );
    if ( mAgingSimulation && mAgingSimulation->mTime == 0.0 )
        ( *mAgingSimulation->mAgingObserver )( 0.0 );
}

#endif /* BUILD_AGING_SIMULATION */

}    // namespace standalone

#endif
