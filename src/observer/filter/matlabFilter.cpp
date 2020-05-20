/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matlabFilter.cpp
* Creation Date : 18-11-2012
* Last Modified : Fr 16 Okt 2015 17:49:40 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "matlabFilter.h"

#ifndef __NO_STRING__
#ifndef _DS1006
#ifndef _SYMBOLIC_

template class observer::MatlabFilterTwoPort< myMatrixType >;
template class observer::MatlabFilterThermal< double >;

#endif /* _SYMBOLIC_ */
#endif /* DS1006 */
#endif /* __NO_STRING__ */
