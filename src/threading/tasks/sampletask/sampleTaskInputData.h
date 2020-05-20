/*
 * SampleTaskInputData.h
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifndef SAMPLETASKINPUTDATA_H_
#define SAMPLETASKINPUTDATA_H_

#include "../../taskData.h"

namespace threading { namespace sampletask
{

    class SampleTaskInputData : public threading::TaskData
    {
    public:
        SampleTaskInputData()
            : mysecondtestvar( 0 )
        { }

        virtual
        ~SampleTaskInputData()
        { }

        bool
        operator ==(const TaskData & /* obj */ ) const
        {
            return true;
        }

        int mysecondtestvar;

    private:
        friend class boost::serialization::access;
        template<class Archive>
            void
            serialize(Archive &ar,
                      const unsigned int /* version */)
            {
                ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( TaskData );
                ar & BOOST_SERIALIZATION_NVP( mysecondtestvar );
            }
    };

} } /* namespace threading */
#endif /* SAMPLETASKINPUTDATA_H_ */
