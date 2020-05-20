#include "filter.h"
#include "../../thermal/blocks/elements/thermal_element.h"
#include "../../electrical/twoport.h"

template class observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >;
template class observer::Filter< double, thermal::ThermalElement, observer::ThermalPreperation >;
