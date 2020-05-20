/*
 * DAEInputData.h
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifndef DAEINPUTDATA_H_
#define DAEINPUTDATA_H_

#ifdef BOOST_MPI

#include "../../taskData.h"
#include "../../../misc/matrixInclude.h"
#include <vector>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>
#include "../../serialization/armadilloMatrix.h"
#include "../../serialization/matlabMatrix.h"

namespace threading { namespace daetask
{
    ///DTO for DAETask
    template< class MatrixType >
    class DAEInputData : public threading::TaskData
    {
    public:
        DAEInputData()
        { }

        virtual
        ~DAEInputData()
        { }

        bool
        operator ==(const TaskData &obj) const
        {
            return true;
        }

        double CurrentValue;
        MatrixType StateVector;
        double Dt;

    private:
        friend class boost::serialization::access;
        template<class Archive>
            void
            serialize(Archive &ar,
                      const unsigned int version)
            {
                ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( TaskData );
                ar & BOOST_SERIALIZATION_NVP( CurrentValue );
                ar & BOOST_SERIALIZATION_NVP( StateVector );
                ar & BOOST_SERIALIZATION_NVP( Dt );
            }
    };

} } /* namespace threading */

#endif /* BOOST_MPI */

#endif /* DAEINPUTDATA_H_ */
