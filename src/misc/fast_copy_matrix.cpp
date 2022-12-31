#include "fast_copy_matrix.h"
#include <cstring>


namespace misc
{

#if defined( _MATLAB_ )
#ifndef _DS1006
#include "mex.h"
#else
#include "rt_matrx.h"
#endif

template <>
void FastCopyMatrix( real_T *array, const matlab::MMatrix< mxDOUBLE_CLASS > &matrix, unsigned int numberOfElements )
{
    memcpy( (void *)array, mxGetPr( matrix.GetMxArray() ), numberOfElements * sizeof( real_T ) );
}
template <>
void FastCopyMatrix( matlab::MMatrix< mxDOUBLE_CLASS > &matrix, const real_T *array, unsigned int numberOfElements )
{
    memcpy( mxGetData( matrix.GetMxArray() ), array, numberOfElements * sizeof( real_T ) );
}
template <>
void FastCopyMatrix( matlab::MMatrix< mxDOUBLE_CLASS > &matrix, const matlab::MMatrix< mxDOUBLE_CLASS > &matrixToCopyFrom,
                     unsigned int numberOfElementsMatrix, const real_T *array, unsigned int numberOfElementsArray )
{
    double *const matrixPtr = mxGetPr( matrix.GetMxArray() );
    FastCopyMatrix( matrixPtr, matrixToCopyFrom, numberOfElementsMatrix );
    memcpy( (void *)( matrixPtr + numberOfElementsMatrix ), array, numberOfElementsArray * sizeof( real_T ) );
}


// Sparse
#elif defined( _EIGEN_ )
template <>
void FastCopyMatrix( ScalarUnit *array, const Eigen::SparseMatrix< double, Eigen::RowMajor > &matrix, unsigned int numberOfElements )
{
    memset( array, 0, numberOfElements * sizeof( ScalarUnit ) );

    for ( size_t i = 0; i < numberOfElements; ++i )
    {
        Eigen::SparseMatrix< double, Eigen::RowMajor >::InnerIterator it( matrix, i );
        if ( it )
            array[i] = it.value();
    }
}
template <>
void FastCopyMatrix( myMatrixType &matrix, const ScalarUnit *array, unsigned int numberOfElements )
{
    // There might be a faster way...
    for ( unsigned int i = 0; i < numberOfElements; ++i )
    {
        matrix.coeffRef( i, 0 ) = array[i];
    }
}


template <>
void FastCopyMatrix( myMatrixType &matrix, const myMatrixType &matrixToCopyFrom, unsigned int numberOfElementsMatrix,
                     const ScalarUnit *array, unsigned int numberOfElementsArray )
{
    const unsigned int numberOfElements = numberOfElementsMatrix + numberOfElementsArray;
    if ( matrix.nonZeros() == static_cast< int >( numberOfElements ) )
    {
        double *const matrixPtr = matrix.valuePtr();
        FastCopyMatrix( matrixPtr, matrixToCopyFrom, numberOfElementsMatrix );
        memcpy( (void *)( matrixPtr + numberOfElementsMatrix ), array, numberOfElementsArray * sizeof( ScalarUnit ) );
    }
    else
    {
        std::vector< Eigen::Triplet< double > > tripletVector;
        tripletVector.resize( numberOfElements );
        for ( size_t i = 0; i < numberOfElementsMatrix; ++i )
        {
            Eigen::SparseMatrix< double, Eigen::RowMajor >::InnerIterator it( matrixToCopyFrom, i );
            if ( it )
                tripletVector.push_back( Eigen::Triplet< double >( i, 0, it.value() ) );
        }
        for ( size_t i = 0; i < numberOfElementsArray; ++i )
            tripletVector.push_back( Eigen::Triplet< double >( numberOfElementsMatrix + i, 0, array[i] ) );
        matrix.setFromTriplets( tripletVector.begin(), tripletVector.end() );
    }
}
#endif


}    // namespace misc
