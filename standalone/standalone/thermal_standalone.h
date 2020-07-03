#ifndef _THERMAL_STANDALONE_
#define _THERMAL_STANDALONE_

#include "../../src/observer/thermal_observer.h"
#include "../../src/thermal/thermal_simulation.h"
#include "simulation_standalone.h"

namespace standalone
{

class ThermalStandalone : virtual public SimulationStandalone
{
    public:
    ThermalStandalone( const std::string &name );
    virtual bool CreateThermalSimulation();
    bool HasThermalSimulationEnded();
    virtual void DoThermalStep();
    virtual void InitializeSimulation();

    protected:
    void SetFixedPowerDissipation();

    boost::shared_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > mThermalSimulation;
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type mStepperThermal;
    boost::shared_ptr< observer::ThermalObserver< double > > mThermalVisualizer;
    std::ofstream mFileVolumeDissipation;
};

}    // namespace standalone

#endif