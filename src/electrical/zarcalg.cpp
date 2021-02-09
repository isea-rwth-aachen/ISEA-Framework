/*
 * zarcalg.cpp
 *
 *  Created on: 04.12.2013
 *      Author: chris
 */

#include "zarcalg.h"
#include "../misc/matrixInclude.h"

namespace electrical
{

template < typename T >
const double ZarcAlg< T >::mPhi[] = {0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.92, 0.94, 0.96, 1};

template < typename T >
const double ZarcAlg< T >::mTauFactor[] = {39.9742, 21.8364, 13.6186, 9.23106, 6.58365,
                                           4.83194, 4.5502,  4.28597, 4.03746, 3.80306};

template < typename T >
const double ZarcAlg< T >::mRFactorAussen[] = {0.28084, 0.26535,  0.2446,   0.21576, 0.17431,
                                               0.11057, 0.093487, 0.074352, 0.05276, 0.028198};

template < typename T >
const double ZarcAlg< T >::mRFactorInnen[] = {0.39633, 0.44284, 0.49461, 0.55903, 0.64641,
                                              0.77687, 0.8115,  0.8502,  0.89378, 0.94327};

template < typename T >
ZarcAlg< T >::ZarcAlg( boost::shared_ptr< object::Object< double > > lookupRelaxationTime,
                       boost::shared_ptr< object::Object< double > > lookupOhmicResistance,
                       boost::shared_ptr< object::Object< double > > lookupPhi, const double samplingRate,
                       const bool observable, typename TwoPort< T >::DataType dataValues )
    : TwoPort< T >( observable, dataValues )
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
    InitializeZarc( samplingRate );
}

template < typename T >
void ZarcAlg< T >::CalculateStateDependentValues()
{
#ifndef _SYMBOLIC_
    this->CalculateCurrentValue();

    this->mPowerValue =
     ::std::pow( this->mCurrentValue, 2.0 ) *
     ( ( 2 * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r );
#endif
}

template < typename T >
void ZarcAlg< T >::InitializeZarc( const double samplingRate )
{
    CalculateZarcElements( samplingRate );
    mCapacitorVoltages[0] = 0;
    mCapacitorVoltages[1] = 0;
    mCapacitorVoltages[2] = 0;
}

template < typename T >
void ZarcAlg< T >::CalculateZarcElements( const double samplingRate )
{
    const double const_tau = mLookupRelaxationTime->GetMinValueOfLookup();
    const double min_tauFactor = mLookupPhi2TauFactor( mLookupPhi->GetMinValueOfLookup() );
    if ( samplingRate == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );

    double minSamplingRate = samplingRate * THREE_TAU;


    mTau.push_back( const_tau * min_tauFactor );
    mTau.push_back( const_tau );
    mTau.push_back( const_tau / min_tauFactor );

    for ( size_t i = 0; i < MAX_RC_ELEMENTS; ++i )
    {
        if ( mTau[i] < minSamplingRate )
            ++mNumberOfElements;
    }
}

template < typename T >
T* ZarcAlg< T >::GetVoltage()
{
#ifndef _SYMBOLIC_
    const double const_phi = mLookupPhi->GetValue();
    mCurrentRcElements.rFactorAussen = mLookupPhi2RFactorAussen( const_phi );
    mCurrentRcElements.rFactorInnen = mLookupPhi2RFactorInnen( const_phi );
    mCurrentRcElements.tauFactor = mLookupPhi2TauFactor( const_phi );

    mCurrentRcElements.const_r = mLookupOhmicResistance->GetValue();
    mCurrentRcElements.const_tau = mLookupRelaxationTime->GetValue();

    double re[MAX_RC_ELEMENTS];
    double rc[MAX_RC_ELEMENTS];
    double c[MAX_RC_ELEMENTS];

    c[0] = ( ( mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r ) /
             ( mCurrentRcElements.tauFactor * mCurrentRcElements.const_tau ) );
    rc[0] = 1.0 / ( mCurrentRcElements.tauFactor * mCurrentRcElements.const_tau );
    re[0] = ( 2 * mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;

    c[1] = ( ( mCurrentRcElements.rFactorInnen * mCurrentRcElements.const_r ) / ( mCurrentRcElements.const_tau ) );
    rc[1] = 1.0 / ( mCurrentRcElements.const_tau );
    re[1] = ( mCurrentRcElements.rFactorAussen + mCurrentRcElements.rFactorInnen ) * mCurrentRcElements.const_r;

    c[2] = ( ( mCurrentRcElements.tauFactor * mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r ) /
             ( mCurrentRcElements.const_tau ) );
    rc[2] = mCurrentRcElements.tauFactor / ( mCurrentRcElements.const_tau );
    re[2] = mCurrentRcElements.rFactorAussen * mCurrentRcElements.const_r;

    this->mVoltageValue = 0;

    const double dt = this->mStateSystemGroup->mDt;

    for ( size_t i = 0; i < mNumberOfElements; ++i )
    {
        double deltaU = this->mCurrentValue * c[i] - mCapacitorVoltages[i] * rc[i];
        mCapacitorVoltages[i] += deltaU * dt;

        // Einzelne Spannungen adddieren
        this->mVoltageValue += mCapacitorVoltages[i];
    }

    // If mNumberOfElements < 3 that means we have to add the ohmic resistance of the RC element to the voltage
    if ( mNumberOfElements < 3 )
        this->mVoltageValue += this->mCurrentValue * re[mNumberOfElements];

    // return the constant value
    this->mVoltage.coeffRef( 0, this->mCurrent.cols() - 1 ) = this->mVoltageValue;
#endif
    return TwoPort< T >::GetVoltage();
}

template < typename T >
const char* ZarcAlg< T >::GetName() const
{
    return "ZarcAlg";
}

template < typename T >
void ZarcAlg< T >::SetResistanceFactor( const double factor )
{
    this->mLookupOhmicResistance->SetFactor( factor );
}

} /* namespace electrical */

template class electrical::ZarcAlg< myMatrixType >;
