#ifdef BUILD_AGING_SIMULATION

#include "aging_standalone.h"
#include "../../src/factory/factorybuilder_for_aging.h"
#include "../../src/observer/filter/metadataFilter.h"

extern template class simulation::AgingSimulation< myMatrixType, double, true >;
extern template class observer::AgingObserver< myMatrixType >;
extern template class factory::FactoryBuilderForAging< myMatrixType, ScalarUnit >;

namespace standalone
{

AgingStandalone::AgingStandalone( const std::string &name )
    : ThermalElectricalStandalone( name )
    , SimulationStandalone( name )
    , mAgingCycles( 1 )
    , mAgingSteps( 1 )
    , mSteadyStateCycles( 0 )
    , mAgingStepTime( 0.0 )
    , mResetThElStates( true )
    , mTimeOfLastAgingStep( 0.0 )
{
}

void AgingStandalone::ReadXmlOptions()
{
    SimulationStandalone::ReadXmlOptions();

    boost::shared_ptr< xmlparser::XmlParameter > agingNode =
     mParser->GetRoot()->GetElementChild( "Options" )->GetElementChild( "Aging" );
    mAgingStepTime = agingNode->GetElementDoubleValue( "AgingStepTime" );
    if ( agingNode->HasElementDirectChild( "CalculationCycles" ) )
        mAgingCycles = agingNode->GetElementUnsignedIntValue( "CalculationCycles" );
    if ( agingNode->HasElementDirectChild( "AgingSteps" ) )
        mAgingSteps = agingNode->GetElementUnsignedIntValue( "AgingSteps" );
    if ( agingNode->HasElementDirectChild( "SteadyStateCycles" ) )
        mSteadyStateCycles = agingNode->GetElementUnsignedIntValue( "SteadyStateCycles" );
    if ( agingNode->HasElementDirectChild( "ResetThermalElectricalStates" ) )
        mResetThElStates = agingNode->GetElementBoolValue( "ResetThermalElectricalStates" );
    if ( mSteadyStateCycles >= mAgingCycles )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "TooManySteadyStateCycles" );
}

bool AgingStandalone::CreateAgingSimulation()
{
    factory::FactoryBuilderForAging< myMatrixType, double > factoryBuilder;
    if ( !CreateElectricalSimulation( &factoryBuilder ) )
    {
        return false;
    }
    if ( !CreateThermalSimulation() )
    {
        return false;
    }
    try
    {
        mAgingSimulation.reset(
         new simulation::AgingSimulation< myMatrixType, double, true >( mParser->GetRoot(), this->mElectricalSimulation,
                                                                        this->mThermalSimulation, this->mCells,
                                                                        mAgingStepTime, &factoryBuilder, mUUID ) );
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

void AgingStandalone::DoAgingStep( size_t agingStep, bool scaleToAgingStep )
{
    mAgingSimulation->mTime = mElectricalSimulation->mTime;
    mAgingSimulation->CalculateAging( GetTimeSinceLastAgingStep() - ( mSteadyStateCycles * GetProfileLength() ), scaleToAgingStep );
    std::cout << "Aging Step " << agingStep + 1 << std::endl << std::endl;
    // Calls the aging observer
    ( *mAgingSimulation->mAgingObserver )( agingStep + 1 );
    // mAgingSimulation->mTime = mProfile->GetTime();
    mTimeOfLastAgingStep = GetSimulationTime();
}

void AgingStandalone::ResetThElSimulation()
{
    if ( mResetThElStates )
        mAgingSimulation->ResetThElStates();
    StartNewCycle();
    CreateNewFilters( *mElectricalSimulation->mObserver, *mParser->GetRoot() );
    CreateNewFilters( *mThermalVisualizer, *mParser->GetRoot() );
    InitializeSimulation();
}

void AgingStandalone::CollectAgingData()
{
    mAgingSimulation->mTime = mElectricalSimulation->mTime;
    mAgingSimulation->CollectCellAgingData();
}

void AgingStandalone::SetCollectAgingData( bool collectAgingData )
{
    mAgingSimulation->SetCollectAgingData( collectAgingData );
}

void AgingStandalone::InitializeSimulation()
{
    // Initialize the simulation
    SetCurrent();
    mElectricalSimulation->UpdateSystem();
    mElectricalSimulation->UpdateSystemValues();

    // add default filters
    if ( !this->mQuiet )
    {
        if ( this->mOutputDecimation > 0.0 )
        {
            mElectricalSimulation->mObserver->AddFilter( new observer::DecimateFilterTwoPort< myMatrixType >( mOutputDecimation ) );
            mThermalVisualizer->AddFilter( new observer::DecimateFilterThermal< double >( mOutputDecimation ) );
        }
        mElectricalSimulation->mObserver->AddFilter( new observer::StdoutFilterTwoPort< myMatrixType >() );
        mThermalVisualizer->AddFilter( new observer::StdoutFilterThermal< double >() );
        if ( mTimeOfLastAgingStep == 0.0 )
        {
            mAgingSimulation->mAgingObserver->AddFilter( new observer::StdoutFilterAging< myMatrixType >() );
        }
    }
    if ( !this->mNoMetadata && mTimeOfLastAgingStep == 0.0 )
    {
        mAgingSimulation->mAgingObserver->AddFilter(
         new observer::MetadataFilterAging< myMatrixType >( "simulation.xml", *mParser, *mProfile ) );
    }

    // initial observer calls
    ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
    ( *mThermalVisualizer )( mThermalSimulation->mTime );
    if ( mTimeOfLastAgingStep == 0.0 )
        ( *mAgingSimulation->mAgingObserver )( 0.0 );
}

double AgingStandalone::GetTimeOfLastAgingStep() { return mTimeOfLastAgingStep; }

double AgingStandalone::GetTimeSinceLastAgingStep() { return GetSimulationTime() - mTimeOfLastAgingStep; }

}    // namespace standalone

#endif