/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : linAlg_sys.h
* Creation Date : 04-09-2014
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLIC_

#ifndef _LINEARALGEBRAISCHESSYSTEM_
#define _LINEARALGEBRAISCHESSYSTEM_

// BOOST
#include <boost/shared_ptr.hpp>

#include "../misc/matrixInclude.h"
#include "generalizedsystem.h"
#include "stateSystemGroup.h"


namespace systm
{

template < typename MatrixT >
class LinAlgSystem : public GeneralizedSystem< MatrixT >
{
    public:
    LinAlgSystem( StateSystemGroup< MatrixT > *stateSystemGroup );
    ~LinAlgSystem(){};

    void PrepareEquationSystem();
    void operator()( const MatrixT &x, MatrixT &dxdt, const double /* t */ );
    void operator()( const std::vector< double > &x, std::vector< double > &dxdt, const double /* t */ );
    void SolveForStateVector( MatrixT &x );
    virtual const MatrixT GetA() const { return MatrixT( 0, 0 ); };
    virtual const MatrixT GetC() const { return MatrixT( 0, 0 ); };
    virtual const MatrixT GetAODE() const { return MatrixT( 0, 0 ); };
    virtual const MatrixT GetCODE() const { return MatrixT( 0, 0 ); };

    size_t GetEquationCount();
    virtual const char *GetName() const;

    private:
    //        MatrixT mDxDt;
    StateSystemGroup< MatrixT > *mStateSystemGroup;
    StateSystem< MatrixT > &mAlgStateSystem;
};

template < typename MatrixT >
LinAlgSystem< MatrixT >::LinAlgSystem( StateSystemGroup< MatrixT > *stateSystemGroup )
    : GeneralizedSystem< MatrixT >()
    , mStateSystemGroup( stateSystemGroup )
    , mAlgStateSystem( stateSystemGroup->mAlgStateSystem )
//  , mDxDt( MatrixT())
{
    // const size_t stateCount = stateSystemGroup->GetStateCount();
    // mDxDt.zeros(stateCount ,1); //< Initialize the size and set matrix mDxDt to zero
}

template < typename MatrixT >
void LinAlgSystem< MatrixT >::operator()( const MatrixT & /* x */, MatrixT & /* dxdt */, const double /* t */ )
{
}

template < typename MatrixT >
void LinAlgSystem< MatrixT >::
operator()( const std::vector< double > & /* x */, std::vector< double > & /* dxdt */, const double /* t */ )
{
}

template < typename MatrixT >
void LinAlgSystem< MatrixT >::PrepareEquationSystem()
{
    if ( mAlgStateSystem.GetEquationCount() > 0 )
    {
        const size_t equationCount = GetEquationCount();

        MatrixT rightHandMat = -1 * mAlgStateSystem.GetEquationSystemCVector();
        const MatrixT &tmpMatrix = solve( mAlgStateSystem.GetEquationSystemAMatrix(), rightHandMat );
        CopyTopToMatrixLeft( mStateSystemGroup->mStateVector, tmpMatrix, equationCount );
        mStateSystemGroup->mStateVector.coeffRef( mStateSystemGroup->mStateVector.rows() - 1, 0 ) = 1;
    }
}

template < typename MatrixT >
size_t LinAlgSystem< MatrixT >::GetEquationCount()
{
    return mAlgStateSystem.GetEquationCount();
};


template < typename MatrixT >
void LinAlgSystem< MatrixT >::SolveForStateVector( MatrixT &x )
{
    if ( mAlgStateSystem.GetEquationCount() > 0 )
    {
        MatrixT tmpMatrix = -1 * mAlgStateSystem.GetEquationSystemCVector();
        MatrixT tmpMatrixLeftHand = mAlgStateSystem.GetEquationSystemAMatrix();
        x = solve( tmpMatrixLeftHand, tmpMatrix );
    }
}

template < typename MatrixT >
const char *LinAlgSystem< MatrixT >::GetName() const
{
    return "LinAlgSystem";
}

}    // namespace systm

#endif /* _LINEARALGEBRAISCHESSYSTEM_ */

#endif    // _SYMBOLIC_
