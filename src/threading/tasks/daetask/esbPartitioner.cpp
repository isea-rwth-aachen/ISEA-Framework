/*
 * esbPartitioner.cpp
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#if defined(BOOST_MPI) || defined(BOOST_THREAD)
#include "esbPartitioner.h"

#include "../../../misc/matrixInclude.h"
#include "../../communicators/communicatorInclude.h"

namespace threading { namespace daetask
{

#ifdef BOOST_MPI
    template class ESBPartitioner< ::threading::MPICommunicator, myMatrixType >;
#endif

#ifdef BOOST_THREAD
    template class ESBPartitioner< ::threading::ThreadCommunicator, myMatrixType >;
#endif


} } /* namespace threading */

#endif /* defined(BOOST_MPI) || defined(BOOST_THREAD) */


