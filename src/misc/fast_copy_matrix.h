#ifndef _FAST_COPY_MATRIX_
#define _FAST_COPY_MATRIX_

#include "../misc/matrixInclude.h"

namespace misc
{

/// Copies a matrix in Form of a column vector into a ScalarUnit array
template < typename Matrix >
void FastCopyMatrix( ScalarUnit *array, const Matrix &matrix, unsigned int numberOfElements )
{
    for ( unsigned int i = 0; i < numberOfElements; ++i )
        array[i] = matrix.coeff( i, 0 );
}

/// Copies a ScalarUnit array into a matrix in Form of a column vector
template < typename Matrix >
void FastCopyMatrix( Matrix &matrix, const ScalarUnit *array, unsigned int numberOfElements )
{
    for ( unsigned int i = 0; i < numberOfElements; ++i )
        matrix.coeffRef( i, 0 ) = array[i];
}

/// Fuses a matrix in Form of a colomn vector and a ScalarUnit array into a matrix in Form of a column vector
template < typename Matrix >
void FastCopyMatrix( Matrix &matrix, const Matrix &matrixToCopyFrom, unsigned int numberOfElementsMatrix,
                     const ScalarUnit *array, unsigned int numberOfElementsArray )
{
    for ( unsigned int i = 0; i < numberOfElementsMatrix; ++i )
        matrix.coeffRef( i, 0 ) = matrixToCopyFrom( i, 0 );
    for ( unsigned int i = 0; i < numberOfElementsArray; ++i )
        matrix.coeffRef( numberOfElementsMatrix + i, 0 ) = array[i];
}

#if  defined( _MATLAB_ ) || defined( _EIGEN_ )
template <>
void FastCopyMatrix< myMatrixType >( ScalarUnit *array, const myMatrixType &matrix, unsigned int numberOfElements );
template <>
void FastCopyMatrix< myMatrixType >( myMatrixType &matrix, const ScalarUnit *array, unsigned int numberOfElements );
template <>
void FastCopyMatrix< myMatrixType >( myMatrixType &matrix, const myMatrixType &matrixToCopyFrom, unsigned int numberOfElementsMatrix,
                                     const ScalarUnit *array, unsigned int numberOfElementsArray );
#endif

}    // namespace misc
#endif
