#include "electrical_simulation.h"
#include "../misc/matrixInclude.h"

template class simulation::ElectricalSimulation< myMatrixType, double, true >;
template class simulation::ElectricalSimulation< myMatrixType, double, false >;