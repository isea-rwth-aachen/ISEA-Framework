/*
 * SampleTaskOutputData.h
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifndef SAMPLETASKOUTPUTDATA_H_
#define SAMPLETASKOUTPUTDATA_H_

#include "../../taskData.h"

namespace threading { namespace sampletask
{

    class SampleTaskOutputData : public threading::TaskData
    {
    public:
        SampleTaskOutputData()
            : mytestvar( 0 )
        { }

        virtual
        ~SampleTaskOutputData()
        { }

        bool
        operator ==(const TaskData & /* obj */) const
        {
            return true;
        }

        int mytestvar;
        size_t mMiliSecsOfOptionalWork;

    private:
        friend class boost::serialization::access;
        template<class Archive>
            void
            serialize(Archive &ar,
                      const unsigned int /* version */)
            {
                ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( TaskData );
                ar & BOOST_SERIALIZATION_NVP( mytestvar );
                ar & BOOST_SERIALIZATION_NVP( mMiliSecsOfOptionalWork );
            }
    };

} } /* namespace threading */
#endif /* SAMPLETASKOUTPUTDATA_H_ */
