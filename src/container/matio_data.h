/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matio_data.h
* Creation Date : 06-07-2015
* Last Modified : Di 13 Okt 2015 17:23:00 CEST
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MATIO_DATA_
#define _MATIO_DATA_

#ifndef _DS1006

#include <vector>
#include <string>
#include <stdexcept>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// ETC
#include "matio.h"
#include "../exceptions/error_proto.h"

// Forward declartion
struct matvar_t;

namespace matlab
{

/// Wrapper for matio data
/// Basic access to structs is granted through the []operator
/// Access to 2-D arrays is granted thought the ()operator
/// a Slice can be created in X and Y direction and a vector is returned
struct MatioData
{
    /// Dimension for slices only X and Y are currently supported
    enum DIMENSION
    {
        X_DIM = 0,
        Y_DIM = 1
    };


    /// Create Matio Data from a matvar_t
    MatioData( matvar_t *matvar );
    /// Create Matio Data from a vector
    MatioData( std::vector< double > &data, std::string name );
    /// Create Matio Data from a vector<vector>
    MatioData( std::vector< std::vector< double > > &data, std::string name );


    /// This function returns a slice of the matlab array as a dense vector
    /// The slice direction can be manipulated with the template parameter
    /// \param DIM is either X_DIM or Y_DIM
    /// \param x get slice number x
    template < size_t DIM >
    std::vector< double > operator()( size_t x ) const
    {
        CheckSanity();

        if ( x > mMatlabVar->dims[DIM] )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", x, mMatlabVar->dims[DIM] );

        std::vector< double > returnVec( mMatlabVar->dims[!DIM] );
        for ( size_t i = 0; i < returnVec.size(); ++i )
        {
            size_t offset = ( mMatlabVar->dims[X_DIM] ) * ( i * !DIM + DIM * x ) + ( x * !DIM + DIM * i );
            returnVec[i] = static_cast< double * >( mMatlabVar->data )[offset];
        }
        return returnVec;
    }

    /// This function returns a double at the position \param x, \param y
    double &operator()( size_t x, size_t y );
    double operator()( size_t x, size_t y ) const;
    /// This function accesses the struct variableName
    /// \param variableName struct to be accessed
    MatioData &operator[]( std::string variableName );

    matvar_t *mMatlabVar;

    std::string mName;

    private:
    /// This function sets mName and returns the name of the last token
    std::string SetNames( std::string &matvarName );

    /// This function checks if all parameters that can be evaluated are sane e.g. the rank of the matrix is two
    void CheckSanity() const;

    /// This function checks if x and y are inside the definded bound of mMatlabVar
    void CheckOutOfBoundAccess( size_t x, size_t y ) const;

    boost::shared_ptr< MatioData > mReturnMember;
};

} /* matlab */

#endif /* _DS1006 */
#endif /* _MATIO_DATA_ */
