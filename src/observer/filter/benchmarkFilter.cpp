/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : benchmarkFilter.cpp
* Creation Date : 07-10-2015
* Last Modified : Do 08 Okt 2015 09:52:05 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "benchmarkFilter.h"

namespace observer
{
#ifndef _DS1006
#ifndef __NO_STRING__
template class BenchmarkFilter< myMatrixType, electrical::TwoPort, PreparationType< myMatrixType > >;
template class BenchmarkFilter< double, thermal::ThermalElement, ThermalPreperation >;
#endif /* __NO_STRING__ */
#endif /* _DS1006 */
} /*namespace*/
