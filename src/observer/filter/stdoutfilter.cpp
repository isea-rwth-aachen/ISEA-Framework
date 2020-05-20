/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : stdoutfilter.cpp
* Creation Date : 18-11-2012
* Last Modified : Fr 04 Sep 2015 10:35:05 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "stdoutfilter.h"

namespace observer
{
#ifndef __NO_STRING__
template class StdoutFilterBase< myMatrixType, electrical::TwoPort, PreparationType< myMatrixType > >;
#endif
}
