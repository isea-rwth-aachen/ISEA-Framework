/*
 * CommunicatorBase.h
 *
 *  Created on: 08.08.2013
 *      Author: chris
 */

#ifndef COMMUNICATORBASE_H_
#define COMMUNICATORBASE_H_

#include "../taskData.h"
#include <boost/function.hpp>

namespace threading
{
    ///Functor for the ThreadedTask::RunTask method
    typedef boost::function<void(size_t workerID)> RemoteTaskExecutionHandler;

} /* namespace threading */
#endif /* COMMUNICATORBASE_H_ */
