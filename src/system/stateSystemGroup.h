/*
 * systemlinearequationsgroup.h
 *
 *  Created on: 03.11.2013
 *      Author: chris
 */

#ifndef SYSTEMLINEAREQUATIONSGROUP_H_
#define SYSTEMLINEAREQUATIONSGROUP_H_

#include "stateSystem.h"


namespace systm
{

/// The StateSystemGroup contains the two equationsystems (dgl + alg) and the statevector
template < typename MatrixType >
class StateSystemGroup
{
    public:
    /// Equationsystem for DGLpart
    StateSystem< MatrixType > mDglStateSystem;

    /// Equationsystem for ALGpart
    StateSystem< MatrixType > mAlgStateSystem;

    /// Statevector
    MatrixType mStateVector;

    /// Current dt
    double mDt;

    /// Standard constructor for empty StateSystemGroup
    StateSystemGroup();

    virtual ~StateSystemGroup() {}

    /// Resets the equationsystems and the statevector to zero (only for sparse)
    void ResetSystem();
    /// Resets all states to zero
    void ResetStates();

    /// After all equations have been registered, this method will set the data sizes and calls itself the reset method
    void Initialize();

    /// The number of registered states
    size_t GetStateCount();

    /// True if initialized
    bool IsInitialized();

    private:
    size_t mStateCount;
    bool mIsInitialized;
};

template < typename MatrixType >
StateSystemGroup< MatrixType >::StateSystemGroup()
    : mDt( 0 )
    , mStateCount( 0 )
    , mIsInitialized( false )

{
}

/// Resets the equationsystems and the statevector to zero
template < typename MatrixType >
void StateSystemGroup< MatrixType >::ResetSystem()
{
    mDglStateSystem.ResetSystem();
    mAlgStateSystem.ResetSystem();
}

template < typename MatrixType >
void StateSystemGroup< MatrixType >::ResetStates()
{
    mStateVector.resize( mStateCount + 1, 1 );
    mStateVector.setZero();
    mStateVector.insert( mStateCount, 0 ) = 1;
}

/// After all equations have been registered, this method will set the data sizes and calls itself the reset method
template < typename MatrixType >
void StateSystemGroup< MatrixType >::Initialize()
{
    if ( mIsInitialized )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AlreadyInitialized",
                                             "StateSystemGroup" );

    mStateCount = mDglStateSystem.GetEquationCount() + mAlgStateSystem.GetEquationCount();
    mDglStateSystem.Initialize( mStateCount );
    mAlgStateSystem.Initialize( mStateCount );
    mStateVector.resize( mStateCount + 1, 1 );
    mStateVector.setZero();
    mStateVector.coeffRef( mStateCount, 0 ) = 1;

    mIsInitialized = true;
}

/// The number of registered states
template < typename MatrixType >
size_t StateSystemGroup< MatrixType >::GetStateCount()
{
    return mStateCount;
}

/// True if initialized
template < typename MatrixType >
bool StateSystemGroup< MatrixType >::IsInitialized()
{
    return mIsInitialized;
}

} /* namespace systm */

#endif /* SYSTEMLINEAREQUATIONSGROUP_H_ */
