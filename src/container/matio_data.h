/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matio_data.h
* Creation Date : 06-07-2015
* Last Modified : Di 13 Okt 2015 17:23:00 CEST
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MATIO_DATA_
#define _MATIO_DATA_

#ifndef _DS1006

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// ETC
#include "../exceptions/error_proto.h"

#include <matio.h>

// Forward declartion
struct matvar_t;

namespace matlab
{

template < int N >
struct DataVector
{
    using DataT = std::vector< typename DataVector< N - 1 >::DataT >;
};

template <>
struct DataVector< 1 >
{
    using DataT = std::vector< double >;
};

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

    /// Create uninitialized MatioData
    explicit MatioData();
    /// Create Matio Data from a matvar_t
    MatioData( matvar_t *matvar );
    /// Create Matio Data from a 1-dimensional vector
    MatioData( typename DataVector< 1 >::DataT &data, std::string name );
    /// Create Matio Data from a 2-dimensional vectorG
    MatioData( typename DataVector< 2 >::DataT &data, std::string name );
    /// Create Matio Data from a stringG
    MatioData( std::string &data, std::string name );


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

    template < int N >
    void LoadFromVector( typename DataVector< N >::DataT &data, std::string name );

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

template < int N >
void CopyData( typename DataVector< N >::DataT &vecIn, std::vector< double > &vecOut, size_t *dims, size_t position = 0,
               size_t stepsize = 1 );

template < int N >
void GetDimensions( typename DataVector< N >::DataT &data, size_t *dims );


template < int N >
MatioData CreateMatioData( typename DataVector< N >::DataT &data, std::string name )
{
    MatioData matData;
    matData.LoadFromVector< N >( data, name );
    return matData;
}

template < int N >
void MatioData::LoadFromVector( typename DataVector< N >::DataT &data, std::string name )
{
    size_t dims[N];
    GetDimensions< N >( data, dims );
    size_t totalSize = 1;
    for ( size_t i = 0; i < N; ++i )
    {
        totalSize *= dims[i];
    }
    std::vector< double > finalVec( totalSize );
    CopyData< N >( data, finalVec, dims );
    mMatlabVar = Mat_VarCreate( SetNames( name ).c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, N, dims, &finalVec[0], 0 );
}

template <>
void MatioData::LoadFromVector< 1 >( typename DataVector< 1 >::DataT &data, std::string name );

template < int N >
void CopyData( typename DataVector< N >::DataT &vecIn, std::vector< double > &vecOut, size_t *dims, size_t position, size_t stepsize )
{
    size_t length = vecIn.size();
    if ( length != dims[0] )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable", length, dims[0] );
    }
    for ( size_t i = 0; i < length; ++i )
    {
        CopyData< N - 1 >( vecIn[i], vecOut, dims + 1, position + i * stepsize, stepsize * length );
    }
}

template <>
void CopyData< 1 >( typename DataVector< 1 >::DataT &vecIn, std::vector< double > &vecOut, size_t *dims,
                    size_t position, size_t stepsize );

template < int N >
void GetDimensions( typename DataVector< N >::DataT &data, size_t *dims )
{
    dims[0] = data.size();
    GetDimensions< N - 1 >( data[0], dims + 1 );
}

template <>
void GetDimensions< 1 >( typename DataVector< 1 >::DataT &data, size_t *dims );

}    // namespace matlab

#endif /* _DS1006 */
#endif /* _MATIO_DATA_ */
