/*
 * taskGroupData.h
 *
 *  Created on: 17.01.2014
 *      Author: chris
 */

#ifndef TASKGROUPDATA_H_
#define TASKGROUPDATA_H_

#include <boost/serialization/access.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
//#include <boost/serialization/string.hpp>
//#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
//#include <boost/date_time/posix_time/time_serialize.hpp>
//#include <boost/date_time/gregorian/greg_serialize.hpp>
//#include <boost/serialization/extended_type_info.hpp>

#include "taskData.h"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace threading
{
///Data object which holds several TaskData.
struct TaskGroupData
{
    TaskGroupData()
        : mIsStop( false )
    { }

    TaskGroupData(size_t numberOfTasks)
        : mDataVec( std::vector< boost::shared_ptr< TaskData > >(numberOfTasks, boost::shared_ptr< TaskData >() ) ),
          mIsStop( false )
    { }

    std::vector< boost::shared_ptr< TaskData > > mDataVec;
    bool mIsStop;

private:
    friend class boost::serialization::access;
    template<class Archive>
        void
        serialize(Archive &ar,
                  const unsigned int /* version */)
        {
            ar & mDataVec;
            ar & mIsStop;
        }

};

} /* namespace threading */

#endif /* TASKGROUPDATA_H_ */
