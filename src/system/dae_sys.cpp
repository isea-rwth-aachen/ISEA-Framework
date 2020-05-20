/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : dae_sys.cpp
* Creation Date : 20-03-2013
* Last Modified : 16.07.2018 17:14:19
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "dae_sys.h"
#include <cstring>

/// Namespace for system objects
namespace systm
{

#ifdef _MATLAB_

template <>
void DifferentialAlgebraicSystem< matlab::MMatrix< mxDOUBLE_CLASS > >::CopyALGResultsToMatrixAndVector(
 const size_t dglUIDCount, const size_t algUIDCount, const matlab::MMatrix< mxDOUBLE_CLASS > &sourceMatrix,
 const matlab::MMatrix< mxDOUBLE_CLASS > &sourceVector, const matlab::MMatrix< mxDOUBLE_CLASS > &invertedMat )
{
    memcpy( reinterpret_cast< void * >( &mStateSystemGroup->mStateVector.mArrayStartElement[dglUIDCount] ),
            reinterpret_cast< void * >( invertedMat.mArrayStartElement ), invertedMat.n_rows * sizeof( double ) );
    memcpy( reinterpret_cast< void * >( &mVectorC.mArrayStartElement[dglUIDCount] ),
            reinterpret_cast< void * >( sourceVector.mArrayStartElement ), sourceVector.n_rows * sizeof( double ) );

    for ( size_t j = 0; j < algUIDCount + dglUIDCount; ++j )
        memcpy( reinterpret_cast< void * >( &mMatrixA.mArrayStartElement[dglUIDCount + j * mMatrixA.n_rows] ),
                reinterpret_cast< void * >( &sourceMatrix.mArrayStartElement[j * sourceMatrix.n_rows] ),
                sourceMatrix.n_rows * sizeof( double ) );
}

template <>
void DifferentialAlgebraicSystem< matlab::MMatrix< mxDOUBLE_CLASS > >::CopyDGLResultsToMatrixAndVector(
 const size_t dglUIDCount, const size_t algUIDCount, const matlab::MMatrix< mxDOUBLE_CLASS > &sourceMatrix,
 const matlab::MMatrix< mxDOUBLE_CLASS > &sourceVector )
{
    for ( size_t j = 0; j < algUIDCount + dglUIDCount; ++j )
        memcpy( reinterpret_cast< void * >( &mMatrixA.mArrayStartElement[j * mMatrixA.n_rows] ),
                reinterpret_cast< void * >( &sourceMatrix.mArrayStartElement[j * sourceMatrix.n_rows] ),
                sourceMatrix.n_rows * sizeof( double ) );

    memcpy( reinterpret_cast< void * >( mVectorC.mArrayStartElement ),
            reinterpret_cast< void const * >( sourceVector.mArrayStartElement ), sourceVector.n_rows * sizeof( double ) );
}
#endif


#ifdef _ARMADILLO_

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyALGResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double > &sourceMatrix,
                                                                                    const Mat< double > &sourceVector,
                                                                                    const Mat< double > &invertedMat )
{
    mStateSystemGroup->mStateVector.submat( dglUIDCount, 0, dglUIDCount + algUIDCount - 1, 0 ) = invertedMat;
    mMatrixA.submat( dglUIDCount, 0, dglUIDCount + algUIDCount - 1, algUIDCount + dglUIDCount - 1 ) = sourceMatrix;
    mVectorC.submat( dglUIDCount, 0, dglUIDCount + algUIDCount - 1, 0 ) = sourceVector;
}

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double > &sourceMatrix,
                                                                                    const Mat< double > &sourceVector )
{
    mMatrixA.submat( 0, 0, dglUIDCount - 1, algUIDCount + dglUIDCount - 1 ) = sourceMatrix;
    mVectorC.submat( 0, 0, dglUIDCount - 1, 0 ) = sourceVector;
}

template <>
DifferentialAlgebraicSystem< arma::SpMat< double > >::DifferentialAlgebraicSystem( StateSystemGroup< arma::SpMat< double > > *stateSystemGroup )
    : GeneralizedSystem< arma::SpMat< double > >()
    , mMatrixA( arma::Mat< double >() )
    , mStateSystemGroup( stateSystemGroup )
    , mDglStateSystem( &stateSystemGroup->mDglStateSystem )
    , mAlgStateSystem( &stateSystemGroup->mAlgStateSystem )
{
    const size_t statecount = mStateSystemGroup->GetStateCount();
    mMatrixA.zeros( statecount, statecount );
    mVectorC.zeros( statecount, 1 );
    CalculateInitialState();
}

template <>
const arma::SpMat< double > DifferentialAlgebraicSystem< arma::SpMat< double > >::GetA() const    ///< Get MatrixA
{
    return arma::SpMat< double >( mMatrixA );
}

template <>
const arma::SpMat< double > DifferentialAlgebraicSystem< arma::SpMat< double > >::GetC() const    ///< Get VectorC
{
    return arma::SpMat< double >( mVectorC );
}

template <>
void DifferentialAlgebraicSystem< arma::SpMat< double > >::PrepareEquationSystem()    ///<  make ode equations out of the linear ones (alg1,alg2)
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    mMatrixA.zeros();
    mVectorC.zeros();

    const arma::SpMat< double > &dglMatrixA = mStateSystemGroup->mDglStateSystem.GetEquationSystemAMatrix();
    const arma::SpMat< double > &dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    for ( arma::SpMat< double >::const_iterator it = dglMatrixA.begin(); it != dglMatrixA.end(); it++ )
        mMatrixA( it.row(), it.col() ) = *it;

    for ( arma::SpMat< double >::const_iterator it = dglVectorC.begin(); it != dglVectorC.end(); it++ )
        mVectorC( it.row(), 0 ) = *it;

    if ( algUIDCount == 0 )
        return;

    const arma::SpMat< double > &algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const arma::SpMat< double > mAlg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().cols( 0, dglUIDCount - 1 );
    const arma::SpMat< double > mAlg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().cols( dglUIDCount, dglUIDCount + algUIDCount - 1 );

    // CalculateInitialStateFromCurrentState
    arma::SpMat< double > &stateVector = mStateSystemGroup->mStateVector;
    arma::SpMat< double > invertedMat =
     solve( mAlg2MatrixA, -1 * algVectorC - mAlg1MatrixA * ( stateVector.rows( 0, dglUIDCount - 1 ) ) );
    for ( size_t i = 0; i < algUIDCount; ++i )
        stateVector( dglUIDCount + i, 0 ) = invertedMat( i, 0 );
    //.....................................

    const arma::SpMat< double > invAlg2MinusAlg1 = inv( mAlg2MatrixA ) * ( -1.0 * mAlg1MatrixA );
    const arma::SpMat< double > DGLifiedA = invAlg2MinusAlg1 * dglMatrixA;
    const arma::SpMat< double > DGLifiedC = invAlg2MinusAlg1 * dglVectorC;

    for ( arma::SpMat< double >::const_iterator it = DGLifiedA.begin(); it != DGLifiedA.end(); it++ )
        mMatrixA( it.row() + dglUIDCount, it.col() ) = *it;

    for ( arma::SpMat< double >::const_iterator it = DGLifiedC.begin(); it != DGLifiedC.end(); it++ )
        mVectorC( it.row() + dglUIDCount, 0 ) = *it;
}

template <>
void DifferentialAlgebraicSystem< arma::SpMat< double > >::
operator()( const arma::SpMat< double > &x, arma::SpMat< double > &dxdt, const double /* t */ )
{
    dxdt = mMatrixA * x + mVectorC;
}

template <>
void DifferentialAlgebraicSystem< arma::SpMat< double > >::
operator()( const arma::Mat< double > &x, arma::Mat< double > &dxdt, const double /* t */ )
{
    dxdt = mMatrixA * x + mVectorC;
}

template <>
void DifferentialAlgebraicSystem< arma::SpMat< double > >::
operator()( const std::vector< double > &x, std::vector< double > &dxdt, const double /* t */ )
{
    const size_t stateCount = mStateSystemGroup->GetStateCount();

    // Store x into temporary states
    arma::SpMat< double > tmpX( stateCount, 1 );
    misc::FastCopyMatrix( tmpX, &x[0], stateCount );

    // Calculate dxdt
    arma::SpMat< double > tmpDxdt( stateCount, 1 );
    operator()( tmpX, tmpDxdt, 0.0 );

    // Store result back into dxdt
    dxdt.resize( x.size() );
    misc::FastCopyMatrix( &dxdt[0], tmpDxdt, stateCount );
}

template <>
void DifferentialAlgebraicSystem< arma::SpMat< double > >::CalculateInitialState()    ///< output x vector with all DGL values set to zero
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( algUIDCount == 0 )
        return;

    const arma::SpMat< double > &algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const arma::SpMat< double > &mAlg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().cols( dglUIDCount, dglUIDCount + algUIDCount - 1 );

    if ( mAlg2MatrixA.n_nonzero == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlgNotInvertable" );

    arma::SpMat< double > &stateVector = mStateSystemGroup->mStateVector;

    arma::SpMat< double > invertedMat = solve( mAlg2MatrixA, algVectorC );    // -1 is beeing put later
    for ( size_t i = 0; i < algUIDCount; ++i )                                // is faster than memcpy ...?
        stateVector( dglUIDCount + i, 0 ) = -invertedMat( i, 0 );
}

template <>
const char *DifferentialAlgebraicSystem< arma::SpMat< double > >::GetName() const
{
    return "DifferentialAlgebraicSystem";
}

#endif /* _ARMADILLO_ */

#if defined( _EIGEN_ ) || defined( _SYMBOLIC_ )

/*
DifferentialAlgebraicSystem< SparseMatrix< _Scalar, RowMajor > >::DifferentialAlgebraicSystem( StateSystemGroup<
MatrixType > *stateSystemGroup )
    : GeneralizedSystem< SparseMatrix< _Scalar, RowMajor > >()
    , mStateSystemGroup( stateSystemGroup )
    , mDglStateSystem( &stateSystemGroup->mDglStateSystem )
    , mAlgStateSystem( &stateSystemGroup->mAlgStateSystem )
{
    CalculateInitialState();
}
*/

/*
template <>
void systm::DifferentialAlgebraicSystem< SparseMatrix< _Scalar, RowMajor > >::PrepareEquationSystem()    ///<  make ode equations out of the linear ones (alg1,alg2)
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    mAlg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().leftCols( dglUIDCount );
    mAlg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );

    if ( algUIDCount == 0 )
        return;

    // CalculateInitialStateFromCurrentState
    const MatrixType &xDgl = mStateSystemGroup->mStateVector.topRows( dglUIDCount );
    SparseMatrix< _Scalar, ColMajor > b = ( mAlg1MatrixA * xDgl );
    b += SparseMatrix< _Scalar, ColMajor >( mAlgStateSystem->GetEquationSystemCVector() );
    b *= -1;

    b.makeCompressed();
    mAlg2MatrixA.makeCompressed();

    SparseLU< SparseMatrix< _Scalar, ColMajor > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );


    SparseMatrix< _Scalar, ColMajor > ret = solver.solve( b );

    mStateSystemGroup->mStateVector.middleRows( dglUIDCount, algUIDCount ) = ret;
}
*/


// template <>
// void DifferentialAlgebraicSystem< SparseMatrix< _Scalar, RowMajor > >::operator()( const MatrixType &x, MatrixType
// &dxdt, const double /* t */ )
/*
{
    dxdt.setZero();

    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();
    const MatrixType &dglMatrixA = mDglStateSystem->GetEquationSystemAMatrix();
    const MatrixType &dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    SparseMatrix< _Scalar > dxdt_dgl = dglMatrixA * x;
    dxdt_dgl += dglVectorC;

    dxdt.topRows( dglUIDCount ) = dxdt_dgl;

    if ( algUIDCount == 0 )
        return;

    SparseMatrix< _Scalar, ColMajor > b = -mAlg1MatrixA * dxdt_dgl;
    b.makeCompressed();

    SparseLU< SparseMatrix< _Scalar, ColMajor > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< _Scalar, ColMajor > ret = solver.solve( b );

    dxdt.bottomRows( algUIDCount ) = ret;
}
*/

#if defined( _DS1006 ) || !defined( _SYMBOLIC_ )
// template <>
// void DifferentialAlgebraicSystem< SparseMatrix< _Scalar, RowMajor > >::
// operator()( const MatrixType &x, MatrixXd &dxdt, const double /* t */ )
/*
{
    dxdt.setZero();

    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();
    const MatrixType &dglMatrixA = mDglStateSystem->GetEquationSystemAMatrix();
    const MatrixType &dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    SparseMatrix< _Scalar, RowMajor > dxdt_dgl = dglMatrixA * x;
    dxdt_dgl += dglVectorC;

    dxdt.topRows( dglUIDCount ) = dxdt_dgl;

    if ( algUIDCount == 0 )
        return;

    SparseMatrix< _Scalar, ColMajor > b = -mAlg1MatrixA * dxdt_dgl;
    b.makeCompressed();

    SparseLU< SparseMatrix< _Scalar, ColMajor > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< _Scalar, ColMajor > ret = solver.solve( b );

    dxdt.bottomRows( algUIDCount ) = ret;
}
*/
#endif


#if defined( _DS1006 ) || !defined( _SYMBOLIC_ )
// template <>
// void DifferentialAlgebraicSystem< SparseMatrix< _Scalar, RowMajor > >::
// operator()( const arma::Mat< double > &x, arma::Mat< double > &dxdt, const double /* t */ )
/*
{
    // Read from arma matrix
    // TODO:Use memcpy in future
    MatrixType x2( x.n_rows, x.n_cols );
    for ( size_t i = 0; i < x.n_rows; ++i )
        x2( i, 0 ) = x( i, 0 );

    dxdt.resize( x.n_rows, x.n_cols );
    MatrixXd dxdt2( dxdt.n_rows, dxdt.n_cols );

    // Do the job
    operator()( x2, dxdt2, 0 );

    // Write back to arma matrix
    for ( size_t i = 0; i < dxdt.n_rows; ++i )
        dxdt( i, 0 ) = dxdt2( i, 0 );
}
*/
#endif

// template <>
// void systm::DifferentialAlgebraicSystem< SparseMatrix< _Scalar, RowMajor > >::
// operator()( const std::vector< _Scalar > &x, std::vector< _Scalar > &dxdt, const double /* t */ )
/*
{
    const size_t stateCount = mStateSystemGroup->GetStateCount();

    // Store x into temporary states
    SparseMatrix< _Scalar, RowMajor > tmpX( stateCount, 1 );
    misc::FastCopyMatrix( tmpX, &x[0], stateCount );

    // Calculate dxdt
    SparseMatrix< _Scalar, RowMajor > tmpDxdt( stateCount, 1 );
    operator()( tmpX, tmpDxdt, 0.0 );

    // Store result back into dxdt
    dxdt.resize( x.size() );
    misc::FastCopyMatrix( &dxdt[0], tmpDxdt, stateCount );
}
*/

/*
template <>
void systm::DifferentialAlgebraicSystem< SparseMatrix< _Scalar, RowMajor > >::CalculateInitialState()    ///< output x vector with all DGL values set to zero
{
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( algUIDCount == 0 )
        return;

    mAlg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );
    if ( mAlg2MatrixA.nonZeros() == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlgNotInvertable" );

    SparseMatrix< _Scalar, ColMajor > b = mAlgStateSystem->GetEquationSystemCVector();
    b.makeCompressed();

    SparseLU< SparseMatrix< _Scalar, ColMajor > > solver;
    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( mAlg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( mAlg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< _Scalar, ColMajor > ret = solver.solve( b );

    mStateSystemGroup->mStateVector.middleRows( dglUIDCount, algUIDCount ) = -ret;
}
*/


/*
template <>
const SparseMatrix< double, RowMajor > DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::GetA() const
{
    const size_t statecount( mStateSystemGroup->GetStateCount() );
    SparseMatrix< double, RowMajor > returnMatrix( statecount, statecount );
    auto matADGL = mDglStateSystem->GetEquationSystemAMatrix();
    auto matAALG = mAlgStateSystem->GetEquationSystemAMatrix();
    returnMatrix.topRows( matADGL.n_rows ) = matADGL;
    returnMatrix.bottomRows( matAALG.n_rows ) = matAALG;
    return returnMatrix;
}
*/

/*
template <>
const SparseMatrix< double, RowMajor > DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::GetAODE() const
{
    SparseMatrix< double, RowMajor > matADGL = mDglStateSystem->GetEquationSystemAMatrix();

    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();
    SparseMatrix< double, RowMajor > returnMatrix( dglUIDCount + algUIDCount, dglUIDCount + algUIDCount );

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    SparseMatrix< double, RowMajor > Alg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().leftCols( dglUIDCount );
    SparseMatrix< double, RowMajor > Alg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );

    if ( algUIDCount == 0 )
        return SparseMatrix< double, RowMajor >( 0, 0 );

    // CalculateInitialStateFromCurrentState
    SparseMatrix< double > b( algUIDCount, algUIDCount );
    b.setIdentity();

    b.makeCompressed();
    Alg2MatrixA.makeCompressed();

    SparseLU< SparseMatrix< double > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( Alg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( Alg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< double > ret = solver.solve( b );

    returnMatrix.topRows( matADGL.n_rows ) = matADGL;
    returnMatrix.bottomRows( ret.n_rows ) = ret * -Alg1MatrixA * matADGL;

    return returnMatrix;
}
*/

/*
template <>
const SparseMatrix< double, RowMajor > DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::GetC() const
{
    const size_t statecount( mStateSystemGroup->GetStateCount() );
    SparseMatrix< double, RowMajor > returnMatrix( statecount, 1 );
    auto matCDGL = mDglStateSystem->GetEquationSystemCVector();
    auto matCALG = mAlgStateSystem->GetEquationSystemCVector();
    returnMatrix.topRows( matCDGL.n_rows ) = matCDGL;
    returnMatrix.bottomRows( matCALG.n_rows ) = matCALG;
    return returnMatrix;
}
*/

/*
template <>
const SparseMatrix< double, RowMajor > DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::GetCODE() const
{
    const size_t statecount( mStateSystemGroup->GetStateCount() );
    SparseMatrix< double, RowMajor > returnMatrix( statecount, 1 );

    SparseMatrix< double, RowMajor > matADGL = mDglStateSystem->GetEquationSystemAMatrix();

    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    SparseMatrix< double, RowMajor > Alg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().leftCols( dglUIDCount );
    SparseMatrix< double, RowMajor > Alg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );

    if ( algUIDCount == 0 )
        return SparseMatrix< double, RowMajor >( 0, 0 );

    // CalculateInitialStateFromCurrentState
    SparseMatrix< double > b( algUIDCount, algUIDCount );
    b.setIdentity();

    b.makeCompressed();
    Alg2MatrixA.makeCompressed();

    SparseLU< SparseMatrix< double > > solver;

    // Compute the ordering permutation vector from the structural pattern of A
    solver.analyzePattern( Alg2MatrixA );
    // Compute the numerical factorization
    solver.factorize( Alg2MatrixA );
    // Use the factors to solve the linear system

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );

    SparseMatrix< double > ret = solver.solve( b );

    returnMatrix.topRows( matADGL.rows() ) = mDglStateSystem->GetEquationSystemCVector();
    returnMatrix.bottomRows( ret.rows() ) = ret * -Alg1MatrixA * mDglStateSystem->GetEquationSystemCVector();
    return returnMatrix;
}
*/

/*
template <>
const char *DifferentialAlgebraicSystem< SparseMatrix< double, RowMajor > >::GetName() const
{
    return "DifferentialAlgebraicSystem";
}

*/

#endif /* _EIGEN_ */
template class DifferentialAlgebraicSystem< myMatrixType >;
// template class systm::DifferentialAlgebraicSystem< Eigen::SparseMatrix< symbolic::Symbolic< double >, 1, int > >;


}    // namespace systm
