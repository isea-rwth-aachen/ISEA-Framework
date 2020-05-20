#include "thermal_simulation.h"
#include "../misc/matrixInclude.h"

template class simulation::ThermalSimulation< myMatrixType, double, true >;
template class simulation::ThermalSimulation< myMatrixType, double, false >;
