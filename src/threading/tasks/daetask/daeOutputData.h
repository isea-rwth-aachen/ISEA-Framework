/*
 * DAEInputData.h
 *
 *  Created on: 14.08.2013
 *      Author: chris
 */

#ifndef DAEOUTPUTDATA_H_
#define DAEOUTPUTDATA_H_

#include "../../taskData.h"
#include "../../../misc/matrixInclude.h"
#include <boost/shared_ptr.hpp>
#include "../../serialization/armadilloMatrix.h"
#include <boost/serialization/vector.hpp>


namespace threading
{
namespace daetask
{
/// DTO for DAETask
template < class MatrixType >
class DAEOutputData : public threading::TaskData
{
    public:
    DAEOutputData() {}

    virtual ~DAEOutputData() {}

    bool operator==( const TaskData & /* obj */ ) const { return true; }

    MatrixType Voltage;
    MatrixType DglSubMat;
    MatrixType DglSubVectorC;
    MatrixType AlgSubMat;
    MatrixType AlgSubVectorC;
    std::vector< double > ObservableTwoportDataVector;

    private:
    friend class boost::serialization::access;
    template < class Archive >
    void serialize( Archive &ar, const unsigned int /* version */ )
    {
        ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP( TaskData );
        ar &BOOST_SERIALIZATION_NVP( Voltage );
        ar &BOOST_SERIALIZATION_NVP( DglSubMat );
        ar &BOOST_SERIALIZATION_NVP( DglSubVectorC );
        ar &BOOST_SERIALIZATION_NVP( AlgSubMat );
        ar &BOOST_SERIALIZATION_NVP( AlgSubVectorC );
        ar &BOOST_SERIALIZATION_NVP( ObservableTwoportDataVector );
    }
};
}
} /* namespace threading */

#endif /* DAEOUTPUTDATA_H_ */
