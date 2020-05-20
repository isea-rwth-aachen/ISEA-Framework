#include "coolingclasswrapper.h"

namespace factory
{

template class Factory< thermal::Cooling<double> >;

template class CoolingClassWrapper< double, thermal::DirichletBoundaryCondition >;
template class CoolingClassWrapper< double, thermal::CoolingByLookUp >;
template class CoolingClassWrapper< double, thermal::CoolingByConstantValue >;
template class CoolingClassWrapperThermalIsolation< double>;

}
