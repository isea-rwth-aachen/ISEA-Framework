/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : elementCounterFilter.cpp
* Creation Date : 08-10-2015
* Last Modified : Do 08 Okt 2015 10:25:11 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "elementCounterFilter.h"

namespace observer
{
#ifndef _DS1006
#ifndef __NO_STRING__
template class ElementCounterFilter< myMatrixType, electrical::TwoPort, PreparationType< myMatrixType > >;
template class ElementCounterFilter< double, thermal::ThermalElement, ThermalPreperation >;
#endif /* __NO_STRING__ */
#endif /* _DS1006 */
} /*namespace*/
