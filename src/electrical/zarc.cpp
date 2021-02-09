/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : voltagesource.cpp
* Creation Date : 20-03-2013
* Last Modified : So 08 Mai 2016 15:43:06 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "zarc.h"

namespace electrical
{
// static const char* VALUESFROM = "HEIKO WITZENHAUSEN";

template < typename T >
const double Zarc< T >::mPhi[] = {0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 1};

template < typename T >
const double Zarc< T >::mTauFactor[] = {39.9742, 21.8364, 13.6186, 9.23106, 6.58365,
                                        4.83194, 4.5502,  4.28597, 4.03746, 3.80306};

template < typename T >
const double Zarc< T >::mRFactorAussen[] = {0.28084, 0.26535,  0.2446,   0.21576, 0.17431,
                                            0.11057, 0.093487, 0.074352, 0.05276, 0.028198};

template < typename T >
const double Zarc< T >::mRFactorInnen[] = {0.39633, 0.44284, 0.49461, 0.55903, 0.64641,
                                           0.77687, 0.8115,  0.8502,  0.89378, 0.94327};

template < typename T >
Zarc< T >::Zarc( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
                 boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
                 boost::shared_ptr< object::Object< double > > lookupPhi, const double dtValue, const bool observable,
                 typename TwoPort< T >::DataType dataValues )
    : TwoPort< T >( observable, dataValues )
    , mZarcVoltageValue( T() )
    , mVoltageEquation( T() )
    , mVoltageValues( MAX_RC_ELEMENTS )
    , mLookupOhmicResistance( lookupOhmicResistance )
    , mLookupRelaxationTime( lookupRelaxationTime )
    , mLookupPhi( lookupPhi )
    , mLookupPhi2TauFactor( std::vector< double >( mTauFactor, mTauFactor + 10 ),
                            std::vector< double >( mPhi, mPhi + 10 ), lookup::LINEAR_INTERPOLATION )
    , mLookupPhi2RFactorInnen( std::vector< double >( mRFactorInnen, mRFactorInnen + 10 ),
                               std::vector< double >( mPhi, mPhi + 10 ), lookup::LINEAR_INTERPOLATION )
    , mLookupPhi2RFactorAussen( std::vector< double >( mRFactorAussen, mRFactorAussen + 10 ),
                                std::vector< double >( mPhi, mPhi + 10 ), lookup::LINEAR_INTERPOLATION )
    , mNumberOfElements( 0 )
{
    InitializeZarc( dtValue );
}

template < typename T >
void Zarc< T >::CalculateStateDependentValues()
{
    GetVoltageValues();

    if ( this->mObservable )
    {
        this->CalculateCurrentValue();
        this->CalculateVoltageValue();
    }

    this->mPowerValue = 0;

    for ( size_t i = 0; i < mNumberOfElements; ++i )
    {
        this->mPowerValue += ::std::pow( mVoltageValues[i], 2.0 ) / mRValues[i];
    }

    if ( mNumberOfElements < 3 )
    {
        if ( !this->mObservable )
            this->CalculateCurrentValue();

        this->mPowerValue += ::std::pow( this->mCurrentValue, 2 ) * mRValues.at( mNumberOfElements );
    }
}

template < typename T >
void Zarc< T >::InitializeZarc( const double samplingRate )
{
    CalculateZarcElements( samplingRate );
    mFunctionVector.push_back( &Zarc< T >::FirstRcVoltage );
    mFunctionVector.push_back( &Zarc< T >::SecondRcVoltage );
    mFunctionVector.push_back( &Zarc< T >::ThirdRcVoltage );

    mOhmicFunctionVector.push_back( &Zarc< T >::AddOhmicResistanceToVoltage3R );
    mOhmicFunctionVector.push_back( &Zarc< T >::AddOhmicResistanceToVoltage2R );
    mOhmicFunctionVector.push_back( &Zarc< T >::AddOhmicResistanceToVoltage1R );
}

template < typename T >
void Zarc< T >::CalculateZarcElements( const double dtValue )
{
    const double const_tau = mLookupRelaxationTime->GetMinValueOfLookup();
    const double min_tauFactor = mLookupPhi2TauFactor( mLookupPhi->GetMinValueOfLookup() );
    if ( dtValue == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );

    double minDtValue = dtValue * THREE_TAU;

    mTau.push_back( const_tau * min_tauFactor );
    mTau.push_back( const_tau );
    mTau.push_back( const_tau / min_tauFactor );

    for ( size_t i = 0; i < MAX_RC_ELEMENTS; ++i )
    {
        if ( mTau[i] > minDtValue )
            ++mNumberOfElements;
    }
    mRValues.resize( MAX_RC_ELEMENTS );
    mCValues.resize( mNumberOfElements );
    mVoltageValues.resize( mNumberOfElements );
}

template < typename T >
void Zarc< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{

    for ( size_t i = 0; i < mNumberOfElements; ++i )
        mUIDs.push_back( stateSystemGroup->mDglStateSystem.GetNewEquation() );

    TwoPort< T >::SetSystem( stateSystemGroup );
}

template < typename T >
void Zarc< T >::SetCurrent( const T current )
{
    TwoPort< T >::SetCurrent( current );
    SetVoltage();
    mVoltageEquation.resize( mNumberOfElements, this->mCurrent.cols() );
    mVoltageEquation.setZero();
}


template < typename T >
void Zarc< T >::CalculateLookupValues()
{
    const double const_phi = mLookupPhi->GetValue();
    mCurrentRcElements.rFactorAussen = mLookupPhi2RFactorAussen( const_phi );
    mCurrentRcElements.rFactorInnen = mLookupPhi2RFactorInnen( const_phi );
    mCurrentRcElements.tauFactor = mLookupPhi2TauFactor( const_phi );

    mCurrentRcElements.const_r = mLookupOhmicResistance->GetValue();
    mCurrentRcElements.const_tau = mLookupRelaxationTime->GetValue();
}

template < typename T >
T* Zarc< T >::GetVoltage()
{
    CalculateLookupValues();
    for ( size_t i = 0; i < mNumberOfElements; ++i )
    {
        ( this->*mFunctionVector[i] )( mCurrentRcElements, mVoltageEquation );
        this->mStateSystemGroup->mDglStateSystem.AddEquations( mUIDs[i], mVoltageEquation );
    }

    // If mNumberOfElements < 3 that means we have to add the ohmic resistance of the RC element to the voltage
    if ( mNumberOfElements < 3 )
        ( this->*mOhmicFunctionVector[mNumberOfElements] )();

    return TwoPort< T >::GetVoltage();
}

template < typename T >
void Zarc< T >::AddOhmicResistanceToVoltage1R()
{
    mRValues[2] = mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r;
    this->mVoltage = mZarcVoltageValue + this->mCurrent * ( mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r );
}

template < typename T >
void Zarc< T >::AddOhmicResistanceToVoltage2R()
{
    mRValues[1] = ( mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;
    this->mVoltage =
     mZarcVoltageValue +
     this->mCurrent * ( ( mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r );
}

template < typename T >
void Zarc< T >::AddOhmicResistanceToVoltage3R()
{
    mRValues[0] =
     ( ScalarUnit( 2 ) * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;
    this->mVoltage = mZarcVoltageValue +
                     this->mCurrent * ( ( ScalarUnit( 2 ) * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) *
                                        mCurrentRcElements.const_r );
}

// TAU = R*C
// C = TAU / R
template < typename T >
void Zarc< T >::FirstRcVoltage( const RcElements& rc, T& voltageEquation )
{
    mRValues[0] = rc.rFactorAussen * rc.const_r;
    mCValues[0] = rc.tauFactor * rc.const_tau / mRValues[0];
    voltageEquation = this->mCurrent * ( mRValues[0] / ( rc.tauFactor * rc.const_tau ) );
    voltageEquation.coeffRef( 0, mUIDs[0] ) -= ScalarUnit( 1.0 ) / ( rc.tauFactor * rc.const_tau );
}

template < typename T >
void Zarc< T >::SecondRcVoltage( const RcElements& rc, T& voltageEquation )
{
    mRValues[1] = rc.rFactorInnen * rc.const_r;
    mCValues[1] = rc.const_tau / mRValues[1];
    voltageEquation = this->mCurrent * ( ( rc.rFactorInnen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation.coeffRef( 0, mUIDs[1] ) -= ScalarUnit( 1.0 ) / ( rc.const_tau );
}

template < typename T >
void Zarc< T >::ThirdRcVoltage( const RcElements& rc, T& voltageEquation )
{
    mRValues[2] = rc.rFactorAussen * rc.const_r;
    mCValues[2] = ( rc.const_tau / rc.tauFactor ) / mRValues[2];
    voltageEquation = this->mCurrent * ( ( rc.tauFactor * rc.rFactorAussen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation.coeffRef( 0, mUIDs[2] ) -= rc.tauFactor / ( rc.const_tau );
}

template < typename T >
void Zarc< T >::SetVoltage()
{
    mZarcVoltageValue.resize( this->mCurrent.rows(), this->mCurrent.cols() );
    mZarcVoltageValue.setZero();

    for ( size_t i = 0; i < mNumberOfElements; ++i )
        mZarcVoltageValue.coeffRef( 0, mUIDs[i] ) = 1.0;

    this->mVoltage = mZarcVoltageValue;
}

template < typename T >
const char* Zarc< T >::GetName() const
{
    return "Zarc";
}
template < typename T >
size_t Zarc< T >::GetNumberOfElements() const
{
    return mNumberOfElements;
}

template < typename T >
const std::vector< ScalarUnit >& Zarc< T >::GetRValues() const
{
    return mRValues;
}

template < typename T >
const std::vector< ScalarUnit >& Zarc< T >::GetCValues() const
{
    return mCValues;
}

template < typename T >
const std::vector< double >& Zarc< T >::GetTau() const
{
    return mTau;
}

template < typename T >
const std::vector< ScalarUnit >& Zarc< T >::GetVoltageValues() const
{
    for ( size_t i = 0; i < mUIDs.size(); ++i )
        mVoltageValues[i] = this->mStateSystemGroup->mStateVector.coeff( mUIDs[i], 0 );

    return mVoltageValues;
}

template < typename T >
void Zarc< T >::SetResistanceFactor( const double factor )
{
    this->mLookupOhmicResistance->SetFactor( factor );
}

#ifdef _EIGEN_
template <>
Eigen::SparseMatrix< double, Eigen::RowMajor >* Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::GetVoltage();

template <>
void Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::FirstRcVoltage( const RcElements& rc,
                                                                             Eigen::SparseMatrix< double, Eigen::RowMajor >& voltageEquation );
template <>
void Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::SecondRcVoltage( const RcElements& rc,
                                                                              Eigen::SparseMatrix< double, Eigen::RowMajor >& voltageEquation );
template <>
void Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::ThirdRcVoltage( const RcElements& rc,
                                                                             Eigen::SparseMatrix< double, Eigen::RowMajor >& voltageEquation );
#endif /* _EIGEN_ */

#ifdef _SYMBOLIC_
template <>
void Zarc< symbolic::SymbolicMatrix< OutType > >::CalculateLookupValues();
#endif

#ifdef _EIGEN_
template <>
void Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::FirstRcVoltage( const RcElements& rc,
                                                                             Eigen::SparseMatrix< double, Eigen::RowMajor >& voltageEquation )
{
    mRValues[0] = rc.rFactorAussen * rc.const_r;
    mCValues[0] = rc.tauFactor * rc.const_tau / mRValues[0];
    voltageEquation.middleRows( 0, 1 ) = this->mCurrent * ( mRValues[0] / ( rc.tauFactor * rc.const_tau ) );
    voltageEquation.coeffRef( 0, mUIDs[0] ) -= 1.0 / ( rc.tauFactor * rc.const_tau );
}

template <>
void Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::SecondRcVoltage( const RcElements& rc,
                                                                              Eigen::SparseMatrix< double, Eigen::RowMajor >& voltageEquation )
{
    mRValues[1] = rc.rFactorInnen * rc.const_r;
    mCValues[1] = rc.const_tau / mRValues[1];
    voltageEquation.middleRows( 1, 1 ) = this->mCurrent * ( ( rc.rFactorInnen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation.coeffRef( 1, mUIDs[1] ) -= 1.0 / ( rc.const_tau );
}

template <>
void Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::ThirdRcVoltage( const RcElements& rc,
                                                                             Eigen::SparseMatrix< double, Eigen::RowMajor >& voltageEquation )
{
    mRValues[2] = rc.rFactorAussen * rc.const_r;
    mCValues[2] = ( rc.const_tau / rc.tauFactor ) / mRValues[2];
    voltageEquation.middleRows( 2, 1 ) = this->mCurrent * ( ( rc.tauFactor * rc.rFactorAussen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation.coeffRef( 2, mUIDs[2] ) -= rc.tauFactor / ( rc.const_tau );
}

template <>
Eigen::SparseMatrix< double, Eigen::RowMajor >* Zarc< Eigen::SparseMatrix< double, Eigen::RowMajor > >::GetVoltage()
{
    CalculateLookupValues();
    for ( size_t i = 0; i < mNumberOfElements; ++i )
    {
        ( this->*mFunctionVector[i] )( mCurrentRcElements, mVoltageEquation );
    }
    if ( mNumberOfElements > 0 )
        this->mStateSystemGroup->mDglStateSystem.AddEquations( mUIDs[0], mVoltageEquation );


    // If mNumberOfElements < 3 that means we have to add the ohmic resistance of the RC element to the voltage
    if ( mNumberOfElements < 3 )
        ( this->*mOhmicFunctionVector[mNumberOfElements] )();

    return TwoPort< Eigen::SparseMatrix< double, Eigen::RowMajor > >::GetVoltage();
}
#endif /* _EIGEN_ */

#ifdef _SYMBOLIC_
template <>
void Zarc< symbolic::SymbolicMatrix< OutType > >::CalculateLookupValues()
{
    mCurrentRcElements.rFactorAussen =
     ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< myMatrixType >::mID ) + "_LookupPhi2RFactorAussen" );
    mCurrentRcElements.rFactorInnen =
     ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< myMatrixType >::mID ) + "_LookupPhi2RFactorInnen" );
    mCurrentRcElements.tauFactor =
     ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< myMatrixType >::mID ) + "_LookupPhi2TauFactor" );

    mCurrentRcElements.const_r =
     ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< myMatrixType >::mID ) + "_ObjR" );
    mCurrentRcElements.const_tau =
     ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< myMatrixType >::mID ) + "_ObjTau" );
}
#endif
}    // namespace electrical

template class electrical::Zarc< myMatrixType >;
