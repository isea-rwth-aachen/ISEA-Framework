/*
 * communicatorInclude.h
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#ifndef COMMUNICATORINCLUDE_H_
#define COMMUNICATORINCLUDE_H_

#include "dummyCommunicator.h"

#ifdef BOOST_MPI
    #include "mpiCommunicator.h"
#endif

#ifdef BOOST_THREAD
    #include "threadCommunicator.h"
#endif

#endif /* COMMUNICATORINCLUDE_H_ */
