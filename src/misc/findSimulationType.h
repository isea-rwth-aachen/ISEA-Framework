#ifndef _FINDSIMULATIONTYPE_
#define _FINDSIMULATIONTYPE_

#include "../aging/aging_simulation.h"
#include "../thermal/electrical_simulation.h"
#include "../thermal/thermal_simulation.h"
#include "../xmlparser/xmlparameter.h"
#include <boost/shared_ptr.hpp>

namespace simulation
{

enum SimulationType
{
    ELECTRICAL,
    THERMAL,
    THERMAL_ELECTRICAL,
    AGING,
    NONE
};

/// can be used to get the simulations that were created and extract more information from them
struct CreatedSimulations
{
    boost::shared_ptr< simulation::ElectricalSimulation< myMatrixType, double, false > > electricalSimulation;
    boost::shared_ptr< simulation::ThermalSimulation< myMatrixType, double, false > > thermalSimulation;
#ifdef BUILD_AGING_SIMULATION
    boost::shared_ptr< simulation::AgingSimulation< myMatrixType, double, false > > agingSimulation;
#endif
};

SimulationType FindSimulationType( boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode,
                                   CreatedSimulations *simulations = nullptr );
}    // namespace simulation

#endif
