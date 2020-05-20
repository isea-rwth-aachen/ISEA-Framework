/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : csvfilter.cpp
* Creation Date : 18-11-2012
* Last Modified : Fr 04 Sep 2015 10:26:03 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "csvfilter.h"


namespace observer
{
#ifndef __NO_STRING__
template class CsvFilterBase< myMatrixType, electrical::TwoPort, PreparationType< myMatrixType > >;
template class CsvFilterBase< double, thermal::ThermalElement, ThermalPreperation >;
#endif /* __NO_STRING__ */
} /*namespace*/
