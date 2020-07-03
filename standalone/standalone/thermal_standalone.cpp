#include "thermal_standalone.h"

#include "../../src/factory/factorybuilder.h"

extern template class simulation::ThermalSimulation< myMatrixType, double, true >;
extern template class observer::ThermalObserver< double >;
extern template class electrical::Cellelement< myMatrixType >;
extern template class factory::FactoryBuilder< myMatrixType, ScalarUnit >;

const double thermal::globalMaxValue = 1000000000.0;
const double thermal::globalMinValue = -thermal::globalMaxValue;

namespace standalone
{

ThermalStandalone::ThermalStandalone( const std::string &name )
    : SimulationStandalone( name )
{
}

bool ThermalStandalone::CreateThermalSimulation()
{
    try
    {
        mThermalSimulation.reset(
         new simulation::ThermalSimulation< myMatrixType, double, true >( mParser->GetRoot(), mStepTime, mProfileLength, mThermalStopCriterion,
                                                                          &mThermalVisualizer, NULL, NULL ) );

        mStepperThermal =
         make_controlled( 1.0e-10, 1.0e-10, boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > >() );

        std::cout << "Thermal Size: " << mThermalSimulation->mThermalStates.size() << std::endl;
        std::cout << "Thermal(unconnected) Size: " << mThermalSimulation->mUnconnectedThermalStates.size() << std::endl;
        std::cout << "Thermal(connected) Size: " << mThermalSimulation->mConnectedThermalElements.size() << std::endl;
        SetFixedPowerDissipation();
        mFileVolumeDissipation = std::ofstream( "Patches_Dissapation.csv" );
        mFileVolumeDissipation << mProfile->GetValue() << "\n";
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

void ThermalStandalone::SetFixedPowerDissipation()
{
    if ( mThermalSimulation->mTime >= this->mProfileChangeTime )
    {
        mProfile->SetTimeAndTriggerEvaluation( mThermalSimulation->mTime );
        double dissipation = mProfile->GetValue();
        for ( auto tState : mThermalSimulation->mThermalStates )
        {
            tState->SetFixedPowerDissipation( dissipation );
        }
        this->mProfileChangeTime = mProfile->GetTimeUntilMaxValueDeviation( 0.0 );
    }
}

bool ThermalStandalone::HasThermalSimulationEnded() { return mThermalSimulation->CheckIfSimulationTimeHasEnded(); }

void ThermalStandalone::DoThermalStep()
{
    SetFixedPowerDissipation();

    mThermalSimulation->InitializeStopCriterion();
    double maxTime;
    maxTime = this->mProfileChangeTime;
    while ( mThermalSimulation->CheckLoopConditionAndSetDeltaTime( maxTime ) )
    {

        if ( mThermalSimulation->IsStopCriterionFulfilled() )
        {
            mThermalSimulation->UpdateSystem();
            mThermalSimulation->UpdateAllThermalStatesTemperatures();
            break;
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
        mFileVolumeDissipation << mProfile->GetValue() << "\n";
    }
}

void ThermalStandalone::InitializeSimulation()
{
    // add default filters
    if ( !this->mQuiet )
    {
        if ( this->mOutputDecimation > 0.0 )
        {
            mThermalVisualizer->AddFilter( new observer::DecimateFilterThermal< double >( mStepTime ) );
        }
        mThermalVisualizer->AddFilter( new observer::StdoutFilterThermal< double >() );
    }

    // initial observer calls
    if ( mThermalSimulation )
        ( *mThermalVisualizer )( mThermalSimulation->mTime );
}

}    // namespace standalone