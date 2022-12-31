#include "electrical_standalone.h"

#include "../../src/factory/factorybuilder.h"
#include "../../src/observer/filter/metadataFilter.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, true >;
extern template class electrical::Cellelement< myMatrixType >;
extern template class factory::FactoryBuilder< myMatrixType, ScalarUnit >;
extern template class observer::MetadataFilter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >;

namespace standalone
{

ElectricalStandalone::ElectricalStandalone( const std::string &name )
    : SimulationStandalone( name )
{
}

bool ElectricalStandalone::CreateElectricalSimulation( factory::FactoryBuilder< myMatrixType, double > *factoryBuilder )
{
    try
    {
        mElectricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double, true >( mParser->GetRoot(), mStepTime, mProfileLength,
                                                                             mSocStopCriterion, &mCells, factoryBuilder, mUUID ) );

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

        mElectricalSimulation->SetVoltageStopCriterion( mMinCellVoltage, mMaxCellVoltage );

        // create equation solvers

        mStepperElectrical =
         make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > >() );
        mStateVector = std::vector< double >( mElectricalSimulation->mStateSystemGroup.mStateVector.rows(), 0.0 );
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

void ElectricalStandalone::SetCurrent()
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
    // If max. or min. voltage reached modify plan
    else if ( mElectricalSimulation->IsStopElectricalCriterionFulfilled() )
    {
        printf( "Move to the next Step\n" );
        double remainingTime = 0.0;
        if ( mProfile->MoveToNextProfileValue( mElectricalSimulation->mTime, remainingTime ) )
        {
            mElectricalSimulation->mRootTwoPort->AddRemainingLoadTime( remainingTime );
            // Update time limits when profile length has changed
            UpdateProfileLength();
            mElectricalSimulation->UpdateSimulationDuration( mProfile->GetMaxTime() );
            mProfileChangeTime = mProfile->GetTimeUntilMaxValueDeviation( 0.0 );
            if ( mProfile->GetType() == electrical::TimeSeriesType::CURRENT )
            {
                mElectricalSimulation->mRootTwoPort->SetCurrent( mProfile->GetValue() );
                mElectricalSimulation->mRootTwoPort->CalculateStateDependentValues();
                return;
            }
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

bool ElectricalStandalone::HasElectricalSimulationEnded()
{
    return mElectricalSimulation->CheckIfSimulationTimeHasEnded();
}

void ElectricalStandalone::DoElectricalStep()
{
    SetCurrent();
    mElectricalSimulation->StartNewTimeStep();
    while ( mElectricalSimulation->CheckLoopConditionAndSetDeltaTime( mProfileChangeTime ) &&
            !mElectricalSimulation->IsStopCriterionFulfilled() )
    {
        mElectricalSimulation->UpdateSystem();


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

        mElectricalSimulation->UpdateSystemValues();

        // prevents SOCs from changing when calling UpdateSystemValues multiple times
        mElectricalSimulation->mStateSystemGroup.mDt = 0;
        ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
        mElectricalSimulation->SaveStatesForLaterReset();
    }

    // Finish step
    mElectricalSimulation->FinshStep();
}

void ElectricalStandalone::InitializeSimulation()
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
            mElectricalSimulation->mObserver->AddFilter( new observer::DecimateFilterTwoPort< myMatrixType >( this->mOutputDecimation ) );
        }
        mElectricalSimulation->mObserver->AddFilter( new observer::StdoutFilterTwoPort< myMatrixType >() );
    }
    if ( !this->mNoMetadata )
    {
        mElectricalSimulation->mObserver->AddFilter(
         new observer::MetadataFilterTwoPort< myMatrixType >( "simulation.xml", *mParser, *mProfile ) );
    }

    // initial observer calls
    ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
}

double ElectricalStandalone::GetSimulationTime() { return mElectricalSimulation->mTime; }

}    // namespace standalone