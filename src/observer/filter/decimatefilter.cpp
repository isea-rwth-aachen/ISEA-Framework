#include "decimatefilter.h"

namespace observer
{
template class DecimateFilterBase< myMatrixType, electrical::TwoPort, PreparationType< myMatrixType > >;
template class DecimateFilterBase< double, thermal::ThermalElement, ThermalPreperation >;
}
