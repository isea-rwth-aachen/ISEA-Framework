/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : sphericalDiffusion.cpp
* Creation Date : 22-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "sphericalDiffusion.h"

template class electrical::SphericalDiffusion< myMatrixType >;


namespace electrical
{
// Heiko Witzenhause Dissertation page 107 numerical approximation by DRT
template < typename T >
const std::vector< double > SphericalDiffusion< T >::mRFactor = {
 0.991746408883833,    0.335189554500520,   0.172209582896012,   0.0990088488847815,  0.0894316108842422,
 0.0628533575690239,   0.0433650998346649,  0.0409465516111291,  0.0359600164087731,  0.0257087018345930,
 0.0206062060663296,   0.0163927855682163,  0.0133393793597222,  0.00963063557519788, 0.0105619640377753,
 0.00625486628390168,  0.00466427366388678, 0.00489302085372321, 0.00377038387403826, 0.00272828796945697,
 0.00220461281834244,  0.00172611255496559, 0.00122500955193401, 0.00100641130984464, 0.000988915520199850,
 0.000990703772136943, 0.00101843891449268, 0.00109746077161406, 0.00134411783912767};

template < typename T >
const std::vector< double > SphericalDiffusion< T >::mTauFactor = {
 0.991451359646436,    0.335870153447181,    0.167579701571178,    0.0979200017777679,   0.0616838415192040,
 0.0396617079878360,   0.0257407628044021,   0.0166771337841095,   0.0106849233907334,   0.00681822631765891,
 0.00435433073209765,  0.00278113078679821,  0.00177898142175253,  0.00113938740056861,  0.000731596152250355,
 0.000467596673254973, 0.000300414861021452, 0.000192385090821147, 0.000122045399317404, 7.75570733968097e-05,
 4.97188886786378e-05, 3.21196950002819e-05, 2.08531377797455e-05, 1.35775500596413e-05, 8.84548015906210e-06,
 5.76662104649152e-06, 3.76592279815960e-06, 2.46389055469806e-06, 1.61518180144545e-06};

template < typename T >
void SphericalDiffusion< T >::SetVoltage()
{
    mConstVoltageVector.resize( this->mCurrent.rows(), this->mCurrent.cols() );
    mConstVoltageVector.setZero();
    for ( size_t i = 0; i < mUIDCount; ++i )
    {
        mConstVoltageVector.coeffRef( 0, mUIDs[i] ) = 1;    // RCs
    }

    if ( mHasCapacity )
        mConstVoltageVector.coeffRef( 0, mUIDs.back() ) = 1;    // C

    this->mVoltage = mConstVoltageVector;
    mDGLPart.resize( this->mCurrent.rows(), this->mCurrent.cols() );
    mDGLPart.setZero();
    mPowerVec.resize( this->mCurrent.rows() * this->mCurrent.cols(), 0 );
}


template < typename T >
T* SphericalDiffusion< T >::GetVoltage()
{
    this->mRvalue = this->GetValue();
    if ( this->HasSystem() )
    {
#ifndef _SYMBOLIC_
        ScalarUnit inv_c = this->mRvalue / mTau->GetValue();
        ScalarUnit neg_inv_tau = -1.0 / mTau->GetValue();
#else
        auto tau = ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjTau" );
        ScalarUnit inv_c = this->mRvalue / tau;
        ScalarUnit neg_inv_tau = ScalarUnit( -1.0 ) / tau;
#endif
        for ( size_t i = 0; i < mUIDCount; ++i )
        {
            mDGLPart = this->mCurrent * ( inv_c * ( ScalarUnit( mRFactor[i] ) / ScalarUnit( mTauFactor[i] ) ) );
            mDGLPart.coeffRef( this->mCurrent.rows() - 1, mUIDs[i] ) += neg_inv_tau / ScalarUnit( mTauFactor[i] );
            this->mStateSystemGroup->mDglStateSystem.AddEquations( mUIDs[i], mDGLPart );
        }

        if ( mHasCapacity )
        {
            mDGLPart = ( mHasCapacity * this->mCurrent ) * ( inv_c * ( ( 3.0 / 2.0 ) ) );
            this->mStateSystemGroup->mDglStateSystem.AddEquations( mUIDs.back(), mDGLPart );
        }
    }
    this->mVoltage = mConstVoltageVector + this->mCurrent * mResidualResistanceFactor * mRvalue;
    return &this->mVoltage;
}

template < typename T >
void SphericalDiffusion< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    for ( size_t i = 0; i < mUIDCount; ++i )
    {
        mUIDs.push_back( stateSystemGroup->mDglStateSystem.GetNewEquation() );    // One per RC-element
    }

    if ( mHasCapacity )
        mUIDs.push_back( stateSystemGroup->mDglStateSystem.GetNewEquation() );    // One capacity at the end

    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
void SphericalDiffusion< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );
    SetVoltage();
}

template < typename T >
void SphericalDiffusion< T >::CalculateStateDependentValues()
{
    this->CalculateCurrentValue();

    if ( this->mObservable )
        this->CalculateVoltageValue();

    this->mPowerValue = 0;

    for ( size_t i = 0; i < mUIDCount; ++i )
    {
        auto V = this->mStateSystemGroup->mStateVector.coeff( mUIDs[i], 0 );
        mPowerVec[i] = V * V / ( mRvalue * mRFactor[i] );
        this->mPowerValue += mPowerVec[i];
    }
    this->mPowerValue += ::std::pow( this->mCurrentValue, 2.0 ) * mResidualResistanceFactor * mRvalue;
}

template < typename T >
void SphericalDiffusion< T >::SetResistanceFactor( const double factor )
{
    this->mR->SetFactor( factor );
    this->mTau->SetFactor( factor );
}
}    // namespace electrical
