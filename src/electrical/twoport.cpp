/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : twoport.cpp
* Creation Date : 13-03-2015
* Last Modified : So 08 Mai 2016 11:52:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "twoport.h"
namespace electrical
{

template < typename T >
void TwoPort< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
    {
        this->CalculateCurrentValue();
        this->CalculateVoltageValue();
    }
}

#ifndef _SYMBOLIC_

template < typename T >
void TwoPort< T >::LoadInternalData( std::vector< double >& dataVector )
{
    mPowerValue = dataVector.back();
    dataVector.pop_back();

    mVoltageValue = dataVector.back();
    dataVector.pop_back();

    mCurrentValue = dataVector.back();
    dataVector.pop_back();
}

template < typename T >
void TwoPort< T >::LoadInternalData( const double& current, const double& voltage, const double& power )
{
    mCurrentValue = current;
    mVoltageValue = voltage;
    mPowerValue = power;
}

template < typename T >
void TwoPort< T >::SaveInternalData( std::vector< double >& dataVector )
{
    dataVector.push_back( mCurrentValue );
    dataVector.push_back( mVoltageValue );
    dataVector.push_back( mPowerValue );
}
#endif

template < typename T >
ScalarUnit TwoPort< T >::GetCurrentValue() const
{
    return mCurrentValue;
}

template < typename T >
ScalarUnit TwoPort< T >::GetVoltageValue() const
{
    return mVoltageValue;
}

template < typename T >
ScalarUnit TwoPort< T >::GetPowerValue() const
{
    return mPowerValue;
}

template < typename T >
double TwoPort< T >::GetLostChargeAh() const
{
    return mLostChargeValue;
}

template < typename T >
double TwoPort< T >::GetLostDischargeAh() const
{
    return mLostDischargeValue;
}


template < typename T >
inline ScalarUnit ReturnFirstElement( T& mat )
{
    return mat.coeffRef( 0, 0 );
}

#ifdef _EIGEN_
template <>
ScalarUnit ReturnFirstElement( Eigen::SparseMatrix< double, Eigen::RowMajor >& mat );
#endif

template < typename T >
void TwoPort< T >::CalculateCurrentValue()
{
    T m = mCurrent * this->mStateSystemGroup->mStateVector;
    this->mCurrentValue = ReturnFirstElement( m );
}

template < typename T >
void TwoPort< T >::CalculateVoltageValue()
{
    T m = mVoltage * this->mStateSystemGroup->mStateVector;
    this->mVoltageValue = ReturnFirstElement( m );
}

template < typename T >
TwoPort< T >::TwoPort( const bool observable, DataType dataValues )
    : mDataStruct( dataValues )
    , mID( 0 )
    , mCurrent( T() )
    , mVoltage( T() )
    , mCurrentValue( mDataStruct->mCurrentValue )
    , mVoltageValue( mDataStruct->mVoltageValue )
    , mPowerValue( mDataStruct->mPowerValue )
    , mLostChargeValue( 0.0 )
    , mLostDischargeValue( 0.0 )
    , mStateSystemGroup( 0 )
    , mObservable( observable )
{
}

template < typename T >
void TwoPort< T >::UpdateStateSystemGroup()
{
    if ( mID )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OnlyRootport" );

    this->GetVoltage();
}

template < typename T >
void TwoPort< T >::SetInitialCurrent( ScalarUnit current )
{
    if ( mID )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "OnlyRootport" );
    mObservable = true;    // Das RootElement soll VCP automatisch berechnen und bereitstellen

    T tmpcurrent;
    tmpcurrent.resize( 1, 1 + mStateSystemGroup->GetStateCount() );
    tmpcurrent.setZero();
    tmpcurrent.coeffRef( 0, mStateSystemGroup->GetStateCount() ) = current;
    this->SetCurrent( tmpcurrent );
}

template < typename T >
bool TwoPort< T >::HasSystem() const
{
    return mStateSystemGroup != 0;    // double negation for bool conversion
}

template < typename T >
size_t TwoPort< T >::GetID() const
{
    return mID;
}

template < typename T >
void TwoPort< T >::SetID( size_t iD )
{
    mID = iD;
}

template < typename T >
T* TwoPort< T >::GetVoltage()
{
    return &mVoltage;
}

template < typename T >
const T* TwoPort< T >::GetConstVoltage() const
{
    return static_cast< const T* >( &mVoltage );
}

template < typename T >
bool TwoPort< T >::IsObservable() const
{
    return mObservable;
}

template < typename T >
void TwoPort< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    if ( HasSystem() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SystemAlreadyDefined" );
    mStateSystemGroup = stateSystemGroup;
}

template < typename T >
void TwoPort< T >::SetCurrent( const T current )
{
    mCurrent = current;
    mVoltage.resize( mCurrent.rows(), mCurrent.cols() );
    mVoltage.setZero();
}


template < typename T >
void TwoPort< T >::AddRemainingLoadTime( const double lostTime )
{
#ifndef _SYMBOLIC_
    if ( mCurrentValue < 0 )
    {
        mLostDischargeValue = mLostDischargeValue + abs( mCurrentValue * lostTime / 3600 );
    }
    else
    {
        mLostChargeValue = mLostChargeValue + abs( mCurrentValue * lostTime / 3600 );
    }
#endif
}


template < typename T >
const T TwoPort< T >::GetCurrent() const
{
    return mCurrent;
}

template < typename T >
void TwoPort< T >::SetCurrent( const ScalarUnit currentval )
{
    this->mCurrent.coeffRef( 0, this->mCurrent.cols() - 1 ) = currentval;
}

template < typename T >
bool TwoPort< T >::IsCellelement() const
{
    return false;
}

template < typename T >
bool TwoPort< T >::CanHaveChildren() const
{
    return false;
}

template < typename T >
bool TwoPort< T >::HasChildren() const
{
    return false;
}

template < typename T >
const char* TwoPort< T >::GetName() const
{
    return "TwoPort";
}

template < typename T >
void TwoPort< T >::SetResistanceFactor( const double )
{
}

template < typename T >
const boost::shared_ptr< ElectricalDataStruct< ScalarUnit > >& TwoPort< T >::GetElectricalData() const
{
    return mDataStruct;
}

#ifdef _EIGEN_
template <>
ScalarUnit ReturnFirstElement( Eigen::SparseMatrix< double, Eigen::RowMajor >& mat )
{
    return mat.coeff( 0, 0 );
}
#endif /* _EIGEN_ */
}    // namespace electrical

template class electrical::TwoPort< myMatrixType >;
