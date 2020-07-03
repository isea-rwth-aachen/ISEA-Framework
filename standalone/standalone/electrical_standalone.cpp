#include "electrical_standalone.h"

#include "../../src/factory/factorybuilder.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, true >;
extern template class electrical::Cellelement< myMatrixType >;
extern template class factory::FactoryBuilder< myMatrixType, ScalarUnit >;

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
                                                                             mSocStopCriterion, &mCells, factoryBuilder ) );

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

    // initial observer calls
    ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
}

double ElectricalStandalone::GetSimulationTime() { return mElectricalSimulation->mTime; }

}    // namespace standalone