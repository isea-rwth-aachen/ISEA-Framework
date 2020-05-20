/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : sphericalDiffusion.h
* Creation Date : 22-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SERICALDIFFUSION_
#define _SERICALDIFFUSION_
// STD
#include <numeric>

// ETC
#include "../object/object.h"
#include "twoport.h"

extern template class electrical::TwoPort< myMatrixType >;
extern template class object::Object< double >;

namespace electrical
{

/// This class defines the spherical diffusion derived by Heiko Witzenhausen in particles mostly used on the anode
template < typename T = myMatrixType >
class SphericalDiffusion : public TwoPort< T >
{
    public:
    explicit SphericalDiffusion(
     boost::shared_ptr< object::Object< double > > rObj, boost::shared_ptr< object::Object< double > > tauObj,
     size_t rcElements, int hasCapacity = 1, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) )
        : TwoPort< T >( observable, dataValues )
        , mHasCapacity( hasCapacity )
        , mUIDCount( rcElements )
        , mResidualResistanceFactor( std::accumulate( mRFactor.begin() + rcElements, mRFactor.end(), 0.0 ) )
        , mR( rObj )
        , mTau( tauObj ){};

    explicit SphericalDiffusion( object::Object< double >* rObj, object::Object< double >* tauObj, size_t rcElements,
                                 int hasCapacity = 1, const bool observable = false,
                                 typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) )
        : SphericalDiffusion( boost::shared_ptr< object::Object< double > >( rObj ),
                              boost::shared_ptr< object::Object< double > >( tauObj ), rcElements, hasCapacity,
                              observable, dataValues ){};    // delegating constructor

    virtual ~SphericalDiffusion(){};

    virtual ScalarUnit GetValue() const
    {
#ifndef _SYMBOLIC_
        return mR->GetValue();
#else
        return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjR" );
#endif

    };    ///< Get Value of Object

    virtual void CalculateStateDependentValues();

    const object::Object< double >* GetElementObject() const
    {
        return mR.get();    ///< Get Object if instance is const
    }
    virtual const char* GetName() const { return "SphericalDiffusion"; };

    void SetVoltage();
    virtual T* GetVoltage();    ///< Return a Voltage over the TwoPort including children

    virtual void SetCurrent( const T currentval );    ///< Sets the current value
    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );

    const static std::vector< double > mRFactor;
    const static std::vector< double > mTauFactor;
    size_t GetUIDCount() const { return mUIDCount; };
    bool HasCapacity() const { return mHasCapacity; };
    virtual void SetResistanceFactor( const double factor );


    std::vector< boost::shared_ptr< object::Object< double > > > GetObjects() const { return {mR, mTau}; };

    private:
    int mHasCapacity = 0;
    const size_t mUIDCount = 0;

    const ScalarUnit mResidualResistanceFactor = ScalarUnit();
    T mConstVoltageVector = T();
    T mDGLPart = T();
    ScalarUnit mRvalue = 0;
    std::vector< size_t > mUIDs;
    boost::shared_ptr< object::Object< double > > mR;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    boost::shared_ptr< object::Object< double > > mTau;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    std::vector< ScalarUnit > mPowerVec;
};

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

#endif /* _SERICALDIFFUSION_ */
