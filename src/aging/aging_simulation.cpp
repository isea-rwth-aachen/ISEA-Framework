#include "aging_simulation.h"

#ifdef BUILD_AGING_SIMULATION
template class simulation::AgingSimulation< myMatrixType, double, true >;
template class simulation::AgingSimulation< myMatrixType, double, false >;
#endif
