/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : aging_observer.cpp
* Creation Date : 31-08-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "aging_observer.h"

#ifdef BUILD_AGING_SIMULATION
template class observer::AgingObserver< myMatrixType >;
#endif
