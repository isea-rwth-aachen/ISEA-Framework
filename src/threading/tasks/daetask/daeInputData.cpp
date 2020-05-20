/*
 * DAEInputData.cpp
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifdef BOOST_MPI
#include "daeInputData.h"
#include "../../../misc/matrixInclude.h"


namespace threading { namespace daetask
{

template class DAEInputData<myMatrixType>;

} }/* namespace threading */

#endif /* BOOST_MPI */
