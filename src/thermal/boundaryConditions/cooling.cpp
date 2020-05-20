#include "cooling.h"

template class thermal::Radiation<double>;
template class thermal::CoolingByLookUp<double>;
template class thermal::CoolingByConstantValue<double>;
template class thermal::DirichletBoundaryCondition<double>;
