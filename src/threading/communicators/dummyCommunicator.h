/*
 * dummyCommunicator.h
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#ifndef DUMMYCOMMUNICATOR_H_
#define DUMMYCOMMUNICATOR_H_

#include "communicatorBase.h"

namespace threading
{

///Dummy communicator which can be used for single thread execution
class DummyCommunicator
{
public:
    DummyCommunicator()
    { }

    virtual
    ~DummyCommunicator()
    { }

    size_t GetSystemThreadCount() const
    {
        return 1;
    }

    void
    RunThreads()
    { }

    void
    SetReady()
    { }

    bool
    IsReady() const
    {
        return true;
    }

    void
    FinishThisCycle()
    { }

    bool
    IsCycleFinished() const
    {
        return false;
    }

    void
    StopAllProcesses()
    { }

    bool
    IsStop() const
    {
        return false;
    }

    size_t
    GetThreadID() const
    {
        return 0;
    }
};

} /* namespace threading */

#endif /* DUMMYCOMMUNICATOR_H_ */
