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
#if defined( _ARMADILLO_ ) && !defined( SPARSE_MATRIX_FORMAT )
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType > >::type mStepperElectrical;
    myMatrixType mStateVector;
#else
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > > >::type mStepperElectrical;
    std::vector< double > mStateVector;
#endif
};

}    // namespace standalone

#endif