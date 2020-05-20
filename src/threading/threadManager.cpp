/*
 * threadManager.cpp
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#if defined(BOOST_MPI) || defined(BOOST_THREAD)
#include "threadManager.h"

namespace threading
{

#ifdef BOOST_MPI

    ///After all ThreadedTask are registered, this method has to be called. It creates and runs the threads
    template< >
    void ThreadManager<MPICommunicator>::CreateAndRunThreads()
    {
        std::vector<size_t> groupArrangementVector = AssignThreadsToTasks( mCommunicator->GetSystemThreadCount() );

        mCommunicator->InitCommunicationGroups(groupArrangementVector);
        size_t rank = mCommunicator->GetThreadID();

        size_t tmpProcessID = 0;

        for (size_t groupID = 0; groupID < mTaskGroupVec.size(); ++groupID)
        {
            mTaskGroupVec.at( groupID )->Init( mCommunicator.get(), groupID, groupArrangementVector.at(groupID) );

            if(rank > tmpProcessID && rank <= tmpProcessID + groupArrangementVector.at(groupID))
            {
                mTaskGroupVec.at( groupID )->RegisterWorker( rank - tmpProcessID - 1 );
                break;
            }

            tmpProcessID += groupArrangementVector.at(groupID);
        }

        mCommunicator->RunThreads( );
    }

#endif //BOOST_MPI

#ifdef BOOST_THREAD

    ///After all ThreadedTask are registered, this method has to be called. It creates and runs the threads
    template< >
    void ThreadManager<ThreadCommunicator>::CreateAndRunThreads()
    {
        std::vector<size_t> groupArrangementVector = AssignThreadsToTasks( mCommunicator->GetSystemThreadCount() );

        for (size_t groupID = 0; groupID < mTaskGroupVec.size(); ++groupID)
        {
            mTaskGroupVec.at( groupID )->Init( mCommunicator.get(), groupID, groupArrangementVector.at(groupID) );

            mTaskGroupVec.at( groupID )->RegisterWorker();
        }

        mCommunicator->RunThreads( );
    }

#endif //BOOST_THREAD

template<> void ThreadManager< DummyCommunicator >::CreateAndRunThreads();

} /* namespace threading */

#endif /* defined(BOOST_MPI) || defined(BOOST_THREAD) */

