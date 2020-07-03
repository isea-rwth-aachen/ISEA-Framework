#ifndef _THERMAL_ELECTRICAL_STANDALONE_
#define _THERMAL_ELECTRICAL_STANDALONE_

#include "electrical_standalone.h"
#include "thermal_standalone.h"

namespace standalone
{

class ThermalElectricalStandalone : public ElectricalStandalone, public ThermalStandalone
{
    public:
    ThermalElectricalStandalone( const std::string& name );
    void InitializeSimulation() override;
    bool CreateThermalSimulation() override;
    void DoElectricalStep() override;
    void DoThermalStep() override;
    void StartNewCycle();
};

}    // namespace standalone

#endif