/*
 * LookupTask.cpp
 *
 *  Created on: 08.08.2013
 *      Author: chris
 */

#include "daeTask.h"

#include "../../../misc/matrixInclude.h"

namespace threading { namespace daetask
{

template class DAETask< DummyCommunicator, myMatrixType >;

#ifdef BOOST_MPI
    template class DAETask< MPICommunicator, myMatrixType >;
#endif

#ifdef BOOST_THREAD
    template class DAETask< ThreadCommunicator, myMatrixType >;
#endif


} } /* namespace threading */
