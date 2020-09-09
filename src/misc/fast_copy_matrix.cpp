#include "fast_copy_matrix.h"
#include <cstring>


namespace misc
{

// Arma
#if defined( _ARMADILLO_ ) && !defined( SPARSE_MATRIX_FORMAT )
template <>
void FastCopyMatrix( real_T *array, const arma::Mat< double > &matrix, unsigned int numberOfElements )
{
    memcpy( (void *)array, matrix.memptr(), numberOfElements * sizeof( real_T ) );
}
template <>
void FastCopyMatrix( arma::Mat< double > &matrix, const real_T *array, unsigned int numberOfElements )
{
    memcpy( (void *)matrix.memptr(), array, numberOfElements * sizeof( real_T ) );
}
template <>
void FastCopyMatrix( arma::Mat< double > &matrix, const arma::Mat< double > &matrixToCopyFrom,
                     unsigned int numberOfElementsMatrix, const real_T *array, unsigned int numberOfElementsArray )
{
    real_T *const matrixPtr = matrix.memptr();
    FastCopyMatrix( matrixPtr, matrixToCopyFrom, numberOfElementsMatrix );
    memcpy( (void *)( matrixPtr + numberOfElementsMatrix ), array, numberOfElementsArray * sizeof( real_T ) );
}


// Matlab
#elif defined( _MATLAB_ )
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
#elif defined( _ARMADILLO_ ) && defined( SPARSE_MATRIX_FORMAT )
template <>
void FastCopyMatrix( real_T *array, const arma::SpMat< double > &matrix, unsigned int numberOfElements )
{
    memset( array, 0, numberOfElements * sizeof( real_T ) );

    const arma::SpMat< double >::iterator itEnd = matrix.end();
    for ( arma::SpMat< double >::iterator it = matrix.begin(); it != itEnd; ++it )
        if ( it.row() < numberOfElements )
            array[it.row()] = *it;
}
template <>
void FastCopyMatrix( arma::SpMat< double > &matrix, const real_T *array, unsigned int numberOfElements )
{
    arma::umat locations( 2, numberOfElements, arma::fill::zeros );
    for ( size_t i = 0; i < numberOfElements; ++i )
        locations( 0, i ) = i;
    arma::Col< double > values( (double *)array, numberOfElements, false );

    matrix = arma::SpMat< double >( locations, values, false );
}
template <>
void FastCopyMatrix( arma::SpMat< double > &matrix, const arma::SpMat< double > &matrixToCopyFrom,
                     unsigned int numberOfElementsMatrix, const real_T *array, unsigned int numberOfElementsArray )
{
    const unsigned int numberOfElements = numberOfElementsMatrix + numberOfElementsArray;
    // locations and values are used for arma::SpMat<>-Constructor
    arma::umat locations( 2, numberOfElements, arma::fill::zeros );
    for ( size_t i = 0; i < numberOfElements; ++i )
        locations( 0, i ) = i;

    arma::Col< double > values( numberOfElements );
    double *const colVecPtr = values.memptr();
    // At first, fill in values of the electrical part of the states into values
    memset( colVecPtr, 0, numberOfElementsMatrix * sizeof( real_T ) );
    const arma::SpMat< double >::iterator itEnd = matrixToCopyFrom.end();
    for ( arma::SpMat< double >::iterator it = matrixToCopyFrom.begin(); it != itEnd; ++it )
        colVecPtr[it.row()] = *it;
    // Then, fill in values of the thermal part of the states into values
    memcpy( (void *)( colVecPtr + numberOfElementsMatrix ), array, numberOfElementsArray * sizeof( real_T ) );

    matrix = arma::SpMat< double >( locations, values, false );
}


// Eigen
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
