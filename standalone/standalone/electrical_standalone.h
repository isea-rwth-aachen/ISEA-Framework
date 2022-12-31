#ifndef _ELECTRICAL_STANDALONE_
#define _ELECTRICAL_STANDALONE_

#include "../../src/electrical/electrical_simulation.h"
#include "simulation_standalone.h"

namespace standalone
{

class ElectricalStandalone : virtual public SimulationStandalone
{
    public:
    ElectricalStandalone( const std::string& name );
    bool CreateElectricalSimulation( factory::FactoryBuilder< myMatrixType, double >* factoryBuilder = nullptr );
    bool HasElectricalSimulationEnded();
    virtual void DoElectricalStep();
    virtual void InitializeSimulation();
    double GetSimulationTime();

    protected:
    void SetCurrent();

    boost::shared_ptr< simulation::ElectricalSimulation< myMatrixType, double, true > > mElectricalSimulation;
    std::vector< boost::shared_ptr< electrical::TwoPort< myMatrixType > > > mCells;

    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > > >::type mStepperElectrical;
    std::vector< double > mStateVector;
};

}    // namespace standalone

#endif