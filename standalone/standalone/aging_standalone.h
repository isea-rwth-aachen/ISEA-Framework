#ifndef _AGINGSTANDALONE_
#define _AGINGSTANDALONE_

#ifdef BUILD_AGING_SIMULATION

#include "thermal_electrical_standalone.h"

#include "../../src/aging/aging_simulation.h"

namespace standalone
{

class AgingStandalone : public ThermalElectricalStandalone
{
    public:
    AgingStandalone( const std::string &name );
    void ReadXmlOptions() override;
    void InitializeSimulation() override;
    bool CreateAgingSimulation();
    void DoAgingStep( size_t agingStep, bool scaleToAgingStep = true );
    void SetCollectAgingData( bool collectAgingData );
    void CollectAgingData();
    void ResetThElSimulation();
    double GetTimeOfLastAgingStep();
    double GetTimeSinceLastAgingStep();

    // options from command line and XML
    size_t mAgingCycles;
    size_t mAgingSteps;
    size_t mSteadyStateCycles;
    size_t mAgingStepTime; /// Duration of one Aging Step in Days
    bool mResetThElStates;

    private:
    boost::shared_ptr< simulation::AgingSimulation< myMatrixType, double, true > > mAgingSimulation;
    double mTimeOfLastAgingStep;
};

}    // namespace standalone

#endif

#endif