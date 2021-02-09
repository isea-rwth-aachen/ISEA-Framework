/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : dae_sys.h
* Creation Date : 03-11-2012
* Last Modified : Mo 19 Jun 2017 16:57:10 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _DIFFERNTIALALGEBRAICSYSTEM_
#define _DIFFERNTIALALGEBRAICSYSTEM_

// STD
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif /* __EXCEPTIONS__ */

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../misc/fast_copy_matrix.h"
#include "../misc/macros.h"
#include "../misc/matrixInclude.h"
#include "stateSystemGroup.h"

/// Namespace for system objects
namespace systm
{
/// Class for creating a Differential Algebraic System from a electric circuit
template < typename T = myMatrixType >
class DifferentialAlgebraicSystem
{
    public:
    DifferentialAlgebraicSystem( StateSystemGroup< T >* stateSystemGroup );
    virtual ~DifferentialAlgebraicSystem(){};

    virtual const T GetA() const { return mMatrixA; }    ///< Get MatrixA
    virtual const T GetC() const { return mVectorC; }    ///< Get VectorC

    virtual const T GetAODE() const;    ///< Get MatrixA
    virtual const T GetCODE() const;    ///< Get VectorC

    void PrepareEquationSystem();
    void operator()( const T& x, T& dxdt, const double /* t */ );
    void operator()( const std::vector< double >& x, std::vector< double >& dxdt, const double /* t */ );

    void CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount, const T& sourceMatrix,
                                          const T& sourceVector );
    void CopyALGResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount, const T& sourceMatrix,
                                          const T& sourceVector, const T& invertedMat );

    virtual const char* GetName() const;

    bool FoundNonZeros( const T& matrix ) const;
    void ThrowIfMatrixIsNonZero( const T& matrix ) const;

    private:
    void CalculateInitialState();    ///< output x vector with all DGL values set to zero
    //        void ODEifyEquations(); ///<  make ode equations out of the linear ones (alg1,alg2)
    //        void PrepareStateVector();

    T mMatrixA;
    T mVectorC;
    StateSystemGroup< T >* mStateSystemGroup;
    StateSystem< T >* mDglStateSystem;
    StateSystem< T >* mAlgStateSystem;
};

template < typename T >
DifferentialAlgebraicSystem< T >::DifferentialAlgebraicSystem( StateSystemGroup< T >* stateSystemGroup )
    : mMatrixA( T() )
    , mStateSystemGroup( stateSystemGroup )
    , mDglStateSystem( &stateSystemGroup->mDglStateSystem )
    , mAlgStateSystem( &stateSystemGroup->mAlgStateSystem )
{
    const size_t stateCount = mStateSystemGroup->GetStateCount();
    mMatrixA.zeros( stateCount, stateCount );    //< Initialize the size and set matrix A to 0
    mVectorC.zeros( stateCount, 1 );             //< Initialize the size and set vector C to 0
    CalculateInitialState();
}

template < typename T >
void DifferentialAlgebraicSystem< T >::PrepareEquationSystem()    ///<  make ode equations out of the linear ones (alg1,alg2)
{
    //#ifndef _SYMBOLIC_
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( dglUIDCount == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

    const T& dglMatrixA = mStateSystemGroup->mDglStateSystem.GetEquationSystemAMatrix();
    const T& dglVectorC = mDglStateSystem->GetEquationSystemCVector();

    CopyDGLResultsToMatrixAndVector( dglUIDCount, algUIDCount, dglMatrixA, dglVectorC );

    if ( algUIDCount == 0 )
        return;

    const T& algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const T alg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().submat( 0, 0, algUIDCount - 1, dglUIDCount - 1 );
    const T alg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().submat( 0, dglUIDCount, algUIDCount - 1, dglUIDCount + algUIDCount - 1 );

    // CalculateInitialStateFromCurrentState
    T& stateVector = mStateSystemGroup->mStateVector;
    T invertedMat = solve( alg2MatrixA, ( algVectorC + alg1MatrixA * ( stateVector.submat( 0, 0, dglUIDCount - 1, 0 ) ) ) * -1 );
    //.....................................

    const T invAlg2MinusAlg1 = inv( alg2MatrixA ) * ( alg1MatrixA * -1.0 );
    const T DGLifiedA = invAlg2MinusAlg1 * dglMatrixA;
    const T DGLifiedC = invAlg2MinusAlg1 * dglVectorC;

    CopyALGResultsToMatrixAndVector( dglUIDCount, algUIDCount, DGLifiedA, DGLifiedC, invertedMat );
    //#endif
}

template < typename T >
void DifferentialAlgebraicSystem< T >::operator()( const T& x, T& dxdt, const double /* t */ )
{
    dxdt = mMatrixA * x + mVectorC;
}

template < typename T >
void DifferentialAlgebraicSystem< T >::operator()( const std::vector< double >& x, std::vector< double >& dxdt, const double /* t */ )
{
#ifndef _SYMBOLIC_
    Eigen::Map< const T > xMap( &x[0], x.size(), 1 );
    Eigen::Map< T > dxdtMap( &dxdt[0], x.size(), 1 );
    operator()( xMap, dxdtMap, 0.0 );
#else
    UNUSED( x );
    UNUSED( dxdt );
#endif
}

template < typename T >
void DifferentialAlgebraicSystem< T >::CalculateInitialState()    ///< output x vector with all DGL values set to zero
{
#ifndef _SYMBOLIC_
    const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
    const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

    if ( algUIDCount == 0 )
        return;

    const T& algVectorC = mAlgStateSystem->GetEquationSystemCVector();
    const T alg2MatrixA =
     mAlgStateSystem->GetEquationSystemAMatrix().submat( 0, dglUIDCount, algUIDCount - 1, dglUIDCount + algUIDCount - 1 );
    ThrowIfMatrixIsNonZero( alg2MatrixA );


    T& stateVector = mStateSystemGroup->mStateVector;

    T invertedMat = solve( alg2MatrixA, algVectorC );    // -1 is beeing put later
    for ( size_t i = 0; i < algUIDCount; ++i )           // is faster than memcpy ...?
        stateVector( dglUIDCount + i, 0 ) = -invertedMat( i, 0 );
#endif
}

template < typename T >
void DifferentialAlgebraicSystem< T >::ThrowIfMatrixIsNonZero( const T& matrix ) const
{
    if ( !FoundNonZeros( matrix ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlgNotInvertable" );
}

template < typename T >
bool DifferentialAlgebraicSystem< T >::FoundNonZeros( const T& matrix ) const
{
    for ( size_t x = 0; x < matrix.rows(); ++x )
    {
        for ( size_t y = 0; y < matrix.cols(); ++y )
        {
            if ( matrix( x, y ) != 0.0 )
                return true;
        }
    }
    return false;
}


template < typename T >
void DifferentialAlgebraicSystem< T >::CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                        const T& sourceMatrix, const T& sourceVector )
{
    for ( size_t i = 0; i < dglUIDCount; ++i )
    {
        for ( size_t j = 0; j < algUIDCount + dglUIDCount; ++j )
            mMatrixA( i, j ) = sourceMatrix( i, j );

        mVectorC( i, 0 ) = sourceVector( i, 0 );
    }
}

template < typename T >
void DifferentialAlgebraicSystem< T >::CopyALGResultsToMatrixAndVector( const size_t dglUIDCount,
                                                                        const size_t algUIDCount, const T& sourceMatrix,
                                                                        const T& sourceVector, const T& invertedMat )
{
    T& stateVector = mStateSystemGroup->mStateVector;
    for ( size_t i = 0; i < algUIDCount; ++i )
    {
        stateVector( dglUIDCount + i, 0 ) = invertedMat( i, 0 );

        for ( size_t j = 0; j < algUIDCount + dglUIDCount; ++j )
            mMatrixA( i + dglUIDCount, j ) = sourceMatrix( i, j );

        mVectorC( i + dglUIDCount, 0 ) = sourceVector( i, 0 );
    }
}

template < typename T >
const char* DifferentialAlgebraicSystem< T >::GetName() const
{
    return "DifferentialAlgebraicSystem";
}

#ifdef _MATLAB_
template <>
void DifferentialAlgebraicSystem< matlab::MMatrix< mxDOUBLE_CLASS > >::CopyALGResultsToMatrixAndVector(
 const size_t dglUIDCount, const size_t algUIDCount, const matlab::MMatrix< mxDOUBLE_CLASS >& sourceMatrix,
 const matlab::MMatrix< mxDOUBLE_CLASS >& sourceVector, const matlab::MMatrix< mxDOUBLE_CLASS >& invertedMat );

template <>
void DifferentialAlgebraicSystem< matlab::MMatrix< mxDOUBLE_CLASS > >::CopyDGLResultsToMatrixAndVector(
 const size_t dglUIDCount, const size_t algUIDCount, const matlab::MMatrix< mxDOUBLE_CLASS >& sourceMatrix,
 const matlab::MMatrix< mxDOUBLE_CLASS >& sourceVector );
#endif

#ifdef _ARMADILLO_

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyALGResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double >& sourceMatrix,
                                                                                    const Mat< double >& sourceVector,
                                                                                    const Mat< double >& invertedMat );

template <>
void DifferentialAlgebraicSystem< Mat< double > >::CopyDGLResultsToMatrixAndVector( const size_t dglUIDCount, const size_t algUIDCount,
                                                                                    const Mat< double >& sourceMatrix,
                                                                                    const Mat< double >& sourceVector );


/// Class for creating a Differential Algebraic System from a electric circuit
template <>
class DifferentialAlgebraicSystem< arma::SpMat< double > >
{
    public:
    DifferentialAlgebraicSystem( StateSystemGroup< arma::SpMat< double > >* stateSystemGroup );
    virtual ~DifferentialAlgebraicSystem(){};

    virtual const arma::SpMat< double > GetA() const;    ///< Get MatrixA
    virtual const arma::SpMat< double > GetC() const;    ///< Get VectorC

    virtual const arma::SpMat< double > GetAODE() const;    ///< Get MatrixA
    virtual const arma::SpMat< double > GetCODE() const;    ///< Get VectorC

    void PrepareEquationSystem();
    void operator()( const arma::SpMat< double >& x, arma::SpMat< double >& dxdt, const double /* t */ );
    void operator()( const arma::Mat< double >& x, arma::Mat< double >& dxdt, const double /* t */ );
    void operator()( const std::vector< double >& x, std::vector< double >& dxdt, const double /* t */ );

    virtual const char* GetName() const { return "DifferentialAlgebraicSystem"; }

    private:
    void CalculateInitialState();    ///< output x vector with all DGL values set to zero
    //        void ODEifyEquations(); ///<  make ode equations out of the linear ones (alg1,alg2)

    arma::Mat< double > mMatrixA;
    arma::Mat< double > mVectorC;
    StateSystemGroup< arma::SpMat< double > >* mStateSystemGroup;
    StateSystem< arma::SpMat< double > >* mDglStateSystem;
    StateSystem< arma::SpMat< double > >* mAlgStateSystem;
};

#endif /* _ARMADILLO_ */


#ifdef _SYMBOLIC_
typedef symbolic::Symbolic< double > _Scalar;
#elif _EIGEN_
typedef double _Scalar;
#endif
typedef Eigen::SparseMatrix< _Scalar, Eigen::RowMajor > MatrixType;

/// Class for creating a Differential Algebraic System from a electric circuit
template <>
class DifferentialAlgebraicSystem< MatrixType >
{
    public:
    DifferentialAlgebraicSystem( StateSystemGroup< MatrixType >* stateSystemGroup )
        : mStateSystemGroup( stateSystemGroup )
        , mDglStateSystem( &stateSystemGroup->mDglStateSystem )
        , mAlgStateSystem( &stateSystemGroup->mAlgStateSystem )
    {
        CalculateInitialState();
    }

    virtual ~DifferentialAlgebraicSystem(){};

    void PrepareEquationSystem()
    {
        const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
        const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

        // if ( dglUIDCount == 0 )
        //    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

        mAlg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().leftCols( dglUIDCount );
        mAlg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );

        if ( algUIDCount == 0 )
            return;

        // CalculateInitialStateFromCurrentState
        const MatrixType& xDgl = mStateSystemGroup->mStateVector.topRows( dglUIDCount );
        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > b = ( mAlg1MatrixA * xDgl );
        b += Eigen::SparseMatrix< _Scalar, Eigen::ColMajor >( mAlgStateSystem->GetEquationSystemCVector() );
        b *= -1;

        b.makeCompressed();
        mAlg2MatrixA.makeCompressed();

        // Compute the ordering permutation vector from the structural pattern of A
        mSolver.analyzePattern( mAlg2MatrixA );
        // Compute the numerical factorization
        mSolver.factorize( mAlg2MatrixA );
        // Use the factors to solve the linear system

        Eigen::ComputationInfo o;
        if ( Eigen::Success != ( o = mSolver.info() ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                                 mSolver.lastErrorMessage().c_str() );

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > ret = mSolver.solve( b );

        mStateSystemGroup->mStateVector.middleRows( dglUIDCount, algUIDCount ) = ret;
    }

    template < typename DerivedX, typename DerivedDxdt >
    void operator()( const Eigen::MatrixBase< DerivedX >& x, Eigen::MatrixBase< DerivedDxdt >& dxdt, const double /* t */ )
    {
        dxdt.setZero();

        const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
        const size_t dglUIDCount = mDglStateSystem->GetEquationCount();
        const MatrixType& dglMatrixA = mDglStateSystem->GetEquationSystemAMatrix();
        const MatrixType& dglVectorC = mDglStateSystem->GetEquationSystemCVector();

        Eigen::Matrix< _Scalar, Eigen::Dynamic, 1 > dxdt_dgl;
        dxdt_dgl.noalias() = dglMatrixA * x;
        dxdt_dgl += dglVectorC;

        dxdt.topRows( dglUIDCount ) = dxdt_dgl;

        if ( algUIDCount == 0 )
            return;

        Eigen::Matrix< _Scalar, Eigen::Dynamic, 1 > b;
        b.noalias() = -mAlg1MatrixA * dxdt_dgl;

        Eigen::Matrix< _Scalar, Eigen::Dynamic, 1 > ret = mSolver.solve( b );

        dxdt.bottomRows( algUIDCount ) = ret;
    }

    template < typename DerivedX, typename DerivedDxdt >
    void operator()( const Eigen::SparseMatrixBase< DerivedX >& x, Eigen::SparseMatrixBase< DerivedDxdt >& dxdt, const double /* t */ )
    {
        const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
        const size_t dglUIDCount = mDglStateSystem->GetEquationCount();
        const MatrixType& dglMatrixA = mDglStateSystem->GetEquationSystemAMatrix();
        const MatrixType& dglVectorC = mDglStateSystem->GetEquationSystemCVector();

        MatrixType dxdt_dgl = dglMatrixA * x;
        dxdt_dgl += dglVectorC;

        dxdt.topRows( dglUIDCount ) = dxdt_dgl;

        if ( algUIDCount == 0 )
            return;

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > b = -mAlg1MatrixA * dxdt_dgl;
        b.makeCompressed();

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > ret = mSolver.solve( b );

        dxdt.bottomRows( algUIDCount ) = ret;
    }

#if defined( _DS1006 ) || !defined( _SYMBOLIC_ )
    void operator()( const arma::Mat< double >& x, arma::Mat< double >& dxdt, const double /* t */ )
    {
        // Read from arma matrix
        // TODO:Use memcpy in future
        Eigen::Matrix< _Scalar, Eigen::Dynamic, 1 > x2( x.n_rows, x.n_cols );
        for ( size_t i = 0; i < x.n_rows; ++i )
            x2.coeffRef( i, 0 ) = x( i, 0 );

        dxdt.resize( x.n_rows, x.n_cols );
        Eigen::Matrix< _Scalar, Eigen::Dynamic, 1 > dxdt2( dxdt.n_rows, dxdt.n_cols );

        // Do the job
        operator()( x2, dxdt2, 0 );

        // Write back to arma matrix
        for ( size_t i = 0; i < dxdt.n_rows; ++i )
            dxdt( i, 0 ) = dxdt2( i, 0 );
    }
#endif
    void operator()( const std::vector< _Scalar >& x, std::vector< _Scalar >& dxdt, const double /* t */ )
    {
        const size_t stateCount = mStateSystemGroup->GetStateCount();
        dxdt.resize( x.size() );
        Eigen::Map< const Eigen::Matrix< _Scalar, Eigen::Dynamic, 1 > > xMap( &x[0], stateCount );
        Eigen::Map< Eigen::Matrix< _Scalar, Eigen::Dynamic, 1 > > dxdtMap( &dxdt[0], stateCount );
        operator()( xMap, dxdtMap, 0.0 );
    }

    virtual const MatrixType GetA() const    ///< Get MatrixA
    {
        const size_t statecount( mStateSystemGroup->GetStateCount() );
        MatrixType returnMatrix( statecount, statecount );
        auto matADGL = mDglStateSystem->GetEquationSystemAMatrix();
        auto matAALG = mAlgStateSystem->GetEquationSystemAMatrix();
        returnMatrix.topRows( matADGL.rows() ) = matADGL;
        returnMatrix.bottomRows( matAALG.rows() ) = matAALG;
        return returnMatrix;
    }

    virtual const MatrixType GetC() const    ///< Get VectorC
    {
        const size_t statecount( mStateSystemGroup->GetStateCount() );
        MatrixType returnMatrix( statecount, 1 );
        auto matCDGL = mDglStateSystem->GetEquationSystemCVector();
        auto matCALG = mAlgStateSystem->GetEquationSystemCVector();
        returnMatrix.topRows( matCDGL.rows() ) = matCDGL;
        returnMatrix.bottomRows( matCALG.rows() ) = matCALG;
        return returnMatrix;
    }

    virtual const MatrixType GetAODE() const    ///< Get MatrixA
    {
        const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
        const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

        if ( dglUIDCount == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

        MatrixType Alg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().leftCols( dglUIDCount );
        MatrixType Alg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );

        auto matADGL( mDglStateSystem->GetEquationSystemAMatrix() );

        if ( algUIDCount == 0 )
            return matADGL;

        // CalculateInitialStateFromCurrentState
        const MatrixType& xDgl = mStateSystemGroup->mStateVector.topRows( dglUIDCount );
        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > b = ( Alg1MatrixA );
        b = b * Eigen::SparseMatrix< _Scalar, Eigen::ColMajor >( mDglStateSystem->GetEquationSystemAMatrix() );
        b *= -1;

        b.makeCompressed();
        Alg2MatrixA.makeCompressed();

        Eigen::SparseLU< Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > > solver;

        // Compute the ordering permutation vector from the structural pattern of A
        solver.analyzePattern( mAlg2MatrixA );
        // Compute the numerical factorization
        solver.factorize( mAlg2MatrixA );
        // Use the factors to solve the linear system

        Eigen::ComputationInfo o;
        if ( Eigen::Success != ( o = solver.info() ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                                 solver.lastErrorMessage().c_str() );
        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > ret = solver.solve( b );

        Eigen::SparseMatrix< _Scalar, Eigen::RowMajor > returnMatrix( dglUIDCount + algUIDCount, dglUIDCount + algUIDCount );

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > matAdgl = Eigen::SparseMatrix< _Scalar, Eigen::ColMajor >( matADGL );
        returnMatrix.topRows( matADGL.rows() ) = matAdgl;
        returnMatrix.bottomRows( ret.rows() ) = ret;

        return returnMatrix;
    }

    virtual const MatrixType GetCODE() const    ///< Get VectorC
    {

        const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
        const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

        if ( dglUIDCount == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DGLNotEnough" );

        MatrixType Alg1MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().leftCols( dglUIDCount );
        MatrixType Alg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );

        auto matCDGL( mDglStateSystem->GetEquationSystemCVector() );

        if ( algUIDCount == 0 )
            return matCDGL;

        // CalculateInitialStateFromCurrentState
        const MatrixType& xDgl = mStateSystemGroup->mStateVector.topRows( dglUIDCount );
        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > b = ( Alg1MatrixA );
        b = b * Eigen::SparseMatrix< _Scalar, Eigen::ColMajor >( mDglStateSystem->GetEquationSystemCVector() );
        b *= -1;

        b.makeCompressed();
        Alg2MatrixA.makeCompressed();

        Eigen::SparseLU< Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > > solver;

        // Compute the ordering permutation vector from the structural pattern of A
        solver.analyzePattern( mAlg2MatrixA );
        // Compute the numerical factorization
        solver.factorize( mAlg2MatrixA );
        // Use the factors to solve the linear system

        Eigen::ComputationInfo o;
        if ( Eigen::Success != ( o = solver.info() ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                                 solver.lastErrorMessage().c_str() );

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > ret = solver.solve( b );

        const size_t statecount( mStateSystemGroup->GetStateCount() );
        Eigen::SparseMatrix< _Scalar, Eigen::RowMajor > returnMatrix( statecount, 1 );

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > matcdgl = Eigen::SparseMatrix< _Scalar, Eigen::ColMajor >( matCDGL );
        returnMatrix.topRows( matCDGL.rows() ) = matcdgl;
        returnMatrix.bottomRows( ret.rows() ) = ret;

        return returnMatrix;
    }

    virtual const char* GetName() const { return "DifferentialAlgebraicSystem"; };

    private:
    void CalculateInitialState()    ///< output x vector with all DGL values set to zero
                                    //        void PrepareStateVector();
    {
        const size_t algUIDCount = mAlgStateSystem->GetEquationCount();
        const size_t dglUIDCount = mDglStateSystem->GetEquationCount();

        if ( algUIDCount == 0 )
            return;

        mAlg2MatrixA = mAlgStateSystem->GetEquationSystemAMatrix().rightCols( algUIDCount );
        if ( mAlg2MatrixA.nonZeros() == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlgNotInvertable" );

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > b = mAlgStateSystem->GetEquationSystemCVector();
        b.makeCompressed();

        Eigen::SparseLU< Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > > solver;
        // Compute the ordering permutation vector from the structural pattern of A
        solver.analyzePattern( mAlg2MatrixA );
        // Compute the numerical factorization
        solver.factorize( mAlg2MatrixA );
        // Use the factors to solve the linear system

        Eigen::ComputationInfo o;
        if ( Eigen::Success != ( o = solver.info() ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                                 solver.lastErrorMessage().c_str() );

        Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > ret = solver.solve( b );

        mStateSystemGroup->mStateVector.middleRows( dglUIDCount, algUIDCount ) = -ret;
    }


    MatrixType mAlg1MatrixA;
    Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > mAlg2MatrixA;

    StateSystemGroup< MatrixType >* mStateSystemGroup;
    StateSystem< MatrixType >* mDglStateSystem;
    StateSystem< MatrixType >* mAlgStateSystem;

    Eigen::SparseLU< Eigen::SparseMatrix< _Scalar, Eigen::ColMajor > > mSolver;
};


}    // namespace systm

#endif /* _DIFFERNTIALALGEBRAICSYSTEM_ */
