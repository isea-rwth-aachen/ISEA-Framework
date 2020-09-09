/*
 * systemlinearequations.h
 *
 *  Created on: 03.11.2013
 *      Author: chris
 */

#ifndef SYSTEMLINEAREQUATIONS_H_
#define SYSTEMLINEAREQUATIONS_H_

#include "../misc/matrixInclude.h"

// STD
#include <vector>

// BOOST
#include <boost/scoped_ptr.hpp>

// ETC
#include "../exceptions/error_proto.h"
#include "rawsparsedata.h"

namespace systm
{

/// Equationsystem which contains the statesystemmatrix
template < typename MatrixType = myMatrixType >
class StateSystem
{
    public:
    /// Standard constructor for empty StateSystem
    StateSystem();

    /// Constructs a system with a given matrix's data
    StateSystem( const MatrixType &matrixA, const MatrixType &vectorC );

    virtual ~StateSystem() {}

    /// Set one or more lines in the matrix
    void AddEquations( size_t equationNumber, const MatrixType &matrix );

    /// Set one or more lines in the matrix
    void AddEquations( size_t equationNumber, const MatrixType &matrix, const MatrixType &vectorC );

    /// Reset the matrix to zeros
    void ResetSystem();

    /// After all equations have been registered, this method will set the data sizes and calls itself the reset method
    void Initialize( size_t stateCount );

    /// Returns the constant part "C" of the matrix
    const MatrixType &GetEquationSystemCVector();

    /// Returns the matrix part "A" of the matrix
    const MatrixType &GetEquationSystemAMatrix();

    /// Register a new equation and get its rownumber
    size_t GetNewEquation();

    /// The number of registered equations
    size_t GetEquationCount();

    // Dummy. Only used with sparse
    void PartitionSystem( std::vector< std::pair< size_t, size_t > > /* partitions */ ) {}

    private:
    MatrixType mMatrixA;
    MatrixType mVectorC;
    size_t mEquationCount;
    size_t mStateCount;
    bool mIsInitialized;
};


#ifdef _EIGEN_
template <>
void StateSystem< Eigen::SparseMatrix< double, Eigen::RowMajor > >::AddEquations(
 size_t equationNumber, const Eigen::SparseMatrix< double, Eigen::RowMajor > &matrix );

template <>
void StateSystem< Eigen::SparseMatrix< double, Eigen::RowMajor > >::AddEquations(
 size_t equationNumber, const Eigen::SparseMatrix< double, Eigen::RowMajor > &matrix,
 const Eigen::SparseMatrix< double, Eigen::RowMajor > &vectorC );

template <>
void StateSystem< Eigen::SparseMatrix< double, Eigen::RowMajor > >::ResetSystem();
#endif /* _EIGEN_ */

/// Standard constructor for empty StateSystem
template < typename MatrixType >
StateSystem< MatrixType >::StateSystem()
    : mEquationCount( 0 )
    , mStateCount( 0 )
    , mIsInitialized( false )
{
    ResetSystem();
}

/// Constructs a system with a given matrix's data
template < typename MatrixType >
StateSystem< MatrixType >::StateSystem( const MatrixType &matrixA, const MatrixType &vectorC )
    : mMatrixA( matrixA )
    , mVectorC( vectorC )
    , mEquationCount( matrixA.rows() )
    , mStateCount( matrixA.cols() )
    , mIsInitialized( true )
{
}

/// Set one or more lines in the matrix
template < typename MatrixType >
void StateSystem< MatrixType >::AddEquations( size_t equationNumber, const MatrixType &matrix )
{
    for ( size_t row = 0; row < matrix.rows(); ++row )
    {
        for ( size_t col = 0; col < mMatrixA.cols(); ++col )
            mMatrixA.coeffRef( row + equationNumber, col ) = matrix.coeff( row, col );

        mVectorC.coeffRef( row + equationNumber, 0 ) = matrix.coeff( row, matrix.cols() - 1 );
    }
}

/// Set one or more lines in the matrix
template < typename MatrixType >
void StateSystem< MatrixType >::AddEquations( size_t equationNumber, const MatrixType &matrix, const MatrixType &vectorC )
{
    for ( size_t row = 0; row < matrix.rows(); ++row )
    {
        for ( size_t col = 0; col < matrix.cols(); ++col )
            mMatrixA.coeffRef( row + equationNumber, col ) = matrix.coeff( row, col );

        mVectorC.coeffRef( row + equationNumber, 0 ) = vectorC.coeff( row, 0 );
    }
}

/// Reset the matrix to zeros (used for sparse only)
template < typename MatrixType >
void StateSystem< MatrixType >::ResetSystem()
{
}

/// After all equations have been registered, this method will set the data sizes and calls itself the reset method
template < typename MatrixType >
void StateSystem< MatrixType >::Initialize( size_t stateCount )
{
    if ( mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlreadyInitialized", "StateSystem" );

    mStateCount = stateCount;
    mMatrixA.resize( mEquationCount, mStateCount );
    mMatrixA.setZero();
    mVectorC.resize( mEquationCount, 1 );
    mVectorC.setZero();

    mIsInitialized = true;
}

/// Returns the constant part "C" of the matrix
template < typename MatrixType >
const MatrixType &StateSystem< MatrixType >::GetEquationSystemCVector()
{
    return mVectorC;
}

/// Returns the matrix part "A" of the matrix
template < typename MatrixType >
const MatrixType &StateSystem< MatrixType >::GetEquationSystemAMatrix()
{
    return mMatrixA;
}

/// Register a new equation and get its rownumber
template < typename MatrixType >
size_t StateSystem< MatrixType >::GetNewEquation()
{
    if ( mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "CannotRegisterEquationAfterInitializationException" );

    return ++mEquationCount - 1;
}

/// The number of registered equations
template < typename MatrixType >
size_t StateSystem< MatrixType >::GetEquationCount()
{
    return mEquationCount;
}


/* SPARSE ADDITIONS */

#ifdef _ARMADILLO_

/// Equationsystem which contains the statesystemmatrix
template <>
class StateSystem< arma::SpMat< double > >
{
    public:
    /// Standard constructor for empty StateSystem
    StateSystem();

    /// Constructs a system with a given matrix's data
    StateSystem( const arma::SpMat< double > &matrixA, const arma::SpMat< double > &vectorC );

    virtual ~StateSystem() {}

    /// Set one or more lines in the matrix
    void AddEquations( size_t equationNumber, const arma::SpMat< double > &matrix );

    /// Set one or more lines in the matrix
    void AddEquations( size_t equationNumber, const arma::SpMat< double > &matrix, const arma::SpMat< double > &vectorC );

    /// Reset the matrix to zeros
    void ResetSystem();

    /// After all equations have been registered, this method will set the data sizes and calls itself the reset method
    void Initialize( size_t stateCount );

    /// Returns the constant part "C" of the matrix
    const arma::SpMat< double > &GetEquationSystemCVector();

    /// Returns the matrix part "A" of the matrix
    const arma::SpMat< double > &GetEquationSystemAMatrix();

    /// Register a new equation and get its rownumber
    size_t GetNewEquation();

    /// The number of registered equations
    size_t GetEquationCount();

    /// The systemmatrices can be parted into multiple matrices for being able to write to them in parallel. This method
    /// does the partitioning work.
    void PartitionSystem( std::vector< std::pair< size_t, size_t > > partitions );

    private:
    /// Add a new partition to the system.
    void AddPartition( size_t &rowOffset, size_t startRow, size_t endRow );

    arma::SpMat< double > mMatrixA;
    arma::SpMat< double > mVectorC;

    std::vector< RawSparseData > mSubMatrixARawDataVector;
    std::vector< RawSparseData > mSubVectorCRawDataVector;
    std::vector< size_t > mPartitionLookupVector;
    std::vector< size_t > mRowOffsetLookupVector;

    size_t mEquationCount;
    size_t mStateCount;
    bool mIsInitialized;
    bool mMatrixANeedsUpdate;
    bool mMatrixCNeedsUpdate;
};

#endif /* _ARMADILLO_ */

} /* namespace systm */

#endif /* SYSTEMLINEAREQUATIONS_H_ */
