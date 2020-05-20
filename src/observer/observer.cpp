#include "observer.h"
#include "../thermal/blocks/elements/thermal_element.h"
#include "../electrical/twoport.h"

template class observer::Observer< myMatrixType, electrical::TwoPort >;
template class observer::Observer< double, thermal::ThermalElement >;
