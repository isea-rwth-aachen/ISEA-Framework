#include "thermal_electrical_standalone.h"
#include "../../src/observer/filter/metadataFilter.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class simulation::ElectricalSimulation< myMatrixType, double, true >;
extern template class electrical::TimeSeries< double, electrical::EvalNoInterpolation >;
extern template class simulation::ThermalSimulation< myMatrixType, double, true >;
extern template class observer::ThermalObserver< double >;
extern template class electrical::Cellelement< myMatrixType >;
extern template class factory::FactoryBuilder< myMatrixType, ScalarUnit >;
extern template class observer::MetadataFilter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >;

namespace standalone
{

ThermalElectricalStandalone::ThermalElectricalStandalone( const std::string &name )
    : ElectricalStandalone( name )
    , ThermalStandalone( name )
    , SimulationStandalone( name )
{
}

void ThermalElectricalStandalone::InitializeSimulation()
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
    }
    if ( !this->mNoMetadata )
    {
        mElectricalSimulation->mObserver->AddFilter(
         new observer::MetadataFilterTwoPort< myMatrixType >( "simulation.xml", *mParser, *mProfile ) );
    }

    // initial observer calls
    ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
    ( *mThermalVisualizer )( mThermalSimulation->mTime );
}

void ThermalElectricalStandalone::DoElectricalStep()
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

        mElectricalSimulation->UpdateAllThermalStatesPowerDissipation();
        ( *mElectricalSimulation->mObserver )( mElectricalSimulation->mTime );
        mElectricalSimulation->SaveStatesForLaterReset();
    }

    // Finish step
    mElectricalSimulation->FinshStep();
}

bool ThermalElectricalStandalone::CreateThermalSimulation()
{
    try
    {
        mThermalSimulation.reset(
         new simulation::ThermalSimulation< myMatrixType, double, true >( mParser->GetRoot(), mStepTime, mProfileLength,
                                                                          mThermalStopCriterion, &mThermalVisualizer,
                                                                          &mElectricalSimulation->mThermalStates, NULL, mUUID ) );

        mStepperThermal =
         make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );

        // Output conductivity matrix
        std::ofstream fileConduction( "Conductivity.csv" );
        const auto &conMat = mThermalSimulation->mThermalSystem->GetA_th_Conductivity();
        for ( size_t row = 0; row < conMat.rows(); ++row )
        {
            for ( Eigen::SparseMatrix< double, Eigen::RowMajor >::InnerIterator it( conMat, row ); it; ++it )
            {
                fileConduction << it.col() << ", " << it.value() << "; ";
            }
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
    catch ( std::exception &e )
    {
        const char *message = e.what();
        printf( "Error while creating the thermal simulation\n" );
        printf( "%s", message );
        return false;
    }
    return true;
}

void ThermalElectricalStandalone::DoThermalStep()
{
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
            printf( "Temperature stop criterion triggered at %.0f sec, electrical states reset\n", mThermalSimulation->mTime );
            mElectricalSimulation->ResetStatesToPointOfTime( mThermalSimulation->mTime );
            return;
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
    }
}

void ThermalElectricalStandalone::StartNewCycle()
{
    mProfile->StartNewCycle();
    mElectricalSimulation->UpdateSimulationDuration( mProfile->GetMaxTime() );
    mThermalSimulation->UpdateSimulationDuration( mProfile->GetMaxTime() );
}

}    // namespace standalone
