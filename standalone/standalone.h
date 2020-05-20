#ifndef _STANDALONE_
#define _STANDALONE_

#include <cstring>
#include <string>

#include <boost/numeric/odeint.hpp>

#include "../src/aging/aging_simulation.h"
#include "../src/factory/factorybuilder.h"
#include "../src/misc/CLI11.hpp"
#include "../src/misc/matrixInclude.h"
#include "../src/misc/tinydir.h"
#include "../src/observer/thermal_observer.h"
#include "../src/thermal/electrical_simulation.h"
#include "../src/thermal/thermal_simulation.h"
#include "../src/time_series/time_series.h"
#include "../src/xmlparser/tinyxml2/xmlparserimpl.h"

namespace standalone
{
class Standalone
{
    public:
    Standalone( std::string name );
    Standalone( const Standalone & ) = delete;
    Standalone &operator=( const Standalone & ) = delete;
    static void PrintVersion( int /* count */ );
    bool ParseCommandLine( int argc, char *argv[] );
    void ReadOptions();
    bool CreateElectricalSimulation( factory::FactoryBuilder< myMatrixType, double > *factoryBuilder = nullptr );
    void FreeXml();
    void StartNewCycle();
    bool HasElectricalSimulationEnded();
    void DoElectricalStep();
    bool CreateThermalSimulation();
    void SetFixedPowerDissipation();
    bool HasThermalSimulationEnded();
    void DoThermalStep();
    void InitializeSimulation();
#ifdef BUILD_AGING_SIMULATION
    bool CreateAgingSimulation();
    void DoAgingStep( size_t agingStep );
    void SetCollectAgingData( bool collectAgingData );
    void CollectAgingData();
#endif

    // options from command line and XML
    bool mQuiet;
    bool mResetThElStates;
    double mStepTime;
    double mSocStopCriterion;
    double mPowerStopCriterion;
    double mThermalStopCriterion;
    size_t mCycles;
    size_t mAgingCycles;
    size_t mAgingSteps;
    size_t mSteadyStateCycles;
    size_t mAgingStepTime;

    boost::scoped_ptr< xmlparser::XmlParser > mParser;

    private:
    void AddOptions();
    void SetCurrent();

    CLI::App mApp;
    std::string mXmlFilename;
    std::string mProfileFilename;

    // electrical simulation
    boost::scoped_ptr< electrical::TimeSeries< double, electrical::EvalNoInterpolation > > mProfile;
    double mProfileChangeTime;
    boost::shared_ptr< simulation::ElectricalSimulation< myMatrixType, double, true > > mElectricalSimulation;
    std::vector< boost::shared_ptr< electrical::TwoPort< myMatrixType > > > mCells;
#if defined( _ARMADILLO_ ) && !defined( SPARSE_MATRIX_FORMAT )
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< myMatrixType > >::type mStepperElectrical;
    myMatrixType mStateVector;
#else
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< std::vector< double > > >::type mStepperElectrical;
    std::vector< double > mStateVector;
#endif

    // thermal simulation
    boost::shared_ptr< simulation::ThermalSimulation< myMatrixType, double, true > > mThermalSimulation;
    boost::numeric::odeint::result_of::make_controlled< boost::numeric::odeint::runge_kutta_cash_karp54< vector< double > > >::type mStepperThermal;
    boost::shared_ptr< observer::ThermalObserver< double > > mThermalVisualizer;
    std::ofstream mFileVolumeDissipation;

// aging simulation
#ifdef BUILD_AGING_SIMULATION
    boost::shared_ptr< simulation::AgingSimulation< myMatrixType, double, true > > mAgingSimulation;
#endif
};
}    // namespace standalone

#endif /* _STANDALONE */
