#include "matlabFilterAging.h"

#ifndef __NO_STRING__
#ifndef _DS1006
#ifdef BUILD_AGING_SIMULATION

template class observer::MatlabFilterAging< myMatrixType >;

#endif /* BUILD_AGING_SIMULATION */
#endif /* DS1006 */
#endif /* __NO_STRING__ */
