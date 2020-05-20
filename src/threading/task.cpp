/*
 * Task.cpp
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#include "task.h"

namespace threading
{

template class Task< DummyCommunicator >;

#ifdef BOOST_MPI

    Task<MPICommunicator>::Task(const size_t minimalNumberOfWorker,
                 const size_t maximalNumberOfWorker)
        : TaskBase<MPICommunicator>(minimalNumberOfWorker, maximalNumberOfWorker)
    { }

    Task<MPICommunicator>::Task()
        : TaskBase<MPICommunicator>()
    { }

    bool Task< MPICommunicator >::DoOptionalWork(size_t workerID)
    {
        return true; //Nothing todo
    }

#endif //BOOST_MPI

#ifdef BOOST_THREAD

    Task<ThreadCommunicator>::Task(const size_t minimalNumberOfWorker,
                 const size_t maximalNumberOfWorker)
        : TaskBase<ThreadCommunicator>(minimalNumberOfWorker, maximalNumberOfWorker)
    { }

    Task<ThreadCommunicator>::Task()
        : TaskBase<ThreadCommunicator>()
    { }

    bool Task< ThreadCommunicator >::DoOptionalWork(size_t workerID)
    {
        return true; //Nothing todo
    }

#endif //BOOST_THREAD

#ifdef BOOST_MPI
    template class Task< MPICommunicator >;
#endif

#ifdef BOOST_THREAD
    template class Task< ThreadCommunicator >;
#endif


} /* namespace threading */
