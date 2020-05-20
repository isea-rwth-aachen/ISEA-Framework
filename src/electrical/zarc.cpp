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

#ifdef _EIGEN_
template <>
void Zarc< Eigen::SparseMatrix< double, RowMajor > >::FirstRcVoltage( const RcElements& rc,
                                                                      Eigen::SparseMatrix< double, RowMajor >& voltageEquation )
{
    mRValues[0] = rc.rFactorAussen * rc.const_r;
    mCValues[0] = rc.tauFactor * rc.const_tau / mRValues[0];
    voltageEquation.middleRows( 0, 1 ) = this->mCurrent * ( mRValues[0] / ( rc.tauFactor * rc.const_tau ) );
    voltageEquation.coeffRef( 0, mUIDs[0] ) -= 1.0 / ( rc.tauFactor * rc.const_tau );
}

template <>
void Zarc< Eigen::SparseMatrix< double, RowMajor > >::SecondRcVoltage( const RcElements& rc,
                                                                       Eigen::SparseMatrix< double, RowMajor >& voltageEquation )
{
    mRValues[1] = rc.rFactorInnen * rc.const_r;
    mCValues[1] = rc.const_tau / mRValues[1];
    voltageEquation.middleRows( 1, 1 ) = this->mCurrent * ( ( rc.rFactorInnen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation.coeffRef( 1, mUIDs[1] ) -= 1.0 / ( rc.const_tau );
}

template <>
void Zarc< Eigen::SparseMatrix< double, RowMajor > >::ThirdRcVoltage( const RcElements& rc,
                                                                      Eigen::SparseMatrix< double, RowMajor >& voltageEquation )
{
    mRValues[2] = rc.rFactorAussen * rc.const_r;
    mCValues[2] = ( rc.const_tau / rc.tauFactor ) / mRValues[2];
    voltageEquation.middleRows( 2, 1 ) = this->mCurrent * ( ( rc.tauFactor * rc.rFactorAussen * rc.const_r ) / ( rc.const_tau ) );
    voltageEquation.coeffRef( 2, mUIDs[2] ) -= rc.tauFactor / ( rc.const_tau );
}

template <>
Eigen::SparseMatrix< double, RowMajor >* Zarc< Eigen::SparseMatrix< double, RowMajor > >::GetVoltage()
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

    return TwoPort< Eigen::SparseMatrix< double, RowMajor > >::GetVoltage();
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
