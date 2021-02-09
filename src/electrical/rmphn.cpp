/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : rmphn.cpp
* Creation Date : 24-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "rmphn.h"

namespace electrical
{

template < typename T >
T* Rmphn< T >::GetVoltage()
{
#ifndef _SYMBOLIC_
    const double r_CT = mR_Ct->GetValue();
    const double r_MP = mRMp->GetValue();

    const double r_SER = std::pow( r_MP * r_CT, 0.5 ) * ( 1.0 / tanh( std::pow( r_MP / r_CT, 0.5 ) ) ) -
                         r_CT;    // Could be helpful https://en.wikipedia.org/wiki/Fast_inverse_square_root
    this->mRser = r_SER;
#else

    ScalarUnit r_CT( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjCT" );
    ScalarUnit r_MP( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjMP" );
    ScalarUnit prod = r_CT * r_MP;
    prod = ScalarUnit( misc::StrCont( "POW(" ) + prod.GetString() + misc::StrCont( ",0.5)" ) );
    ScalarUnit quotient = r_MP / r_CT;
    quotient = ScalarUnit( misc::StrCont( misc::StrCont( "POW(" ) + quotient.GetString() + misc::StrCont( ",0.5)" ) ) );
    quotient = ScalarUnit( misc::StrCont( misc::StrCont( "TANH(" ) + quotient.GetString() + misc::StrCont( ")" ) ) );
    quotient = ScalarUnit( 1 ) / quotient;
    ScalarUnit r_SER = prod * quotient - r_CT;

#endif

    // \omega = 0
    /*     R_ser = R_ser+real((R_MP.*(R_ct./(1+1i.*w.*Tau_dl).^Phi_HN)).^0.5 ...
                 .*coth((R_MP./(R_ct./(1+1i.*w.*Tau_dl).^Phi_HN)).^0.5)...
                     -R_ct);
                     */

    this->mVoltage = *SerialTwoPort< T >::GetVoltage() + this->mCurrent * r_SER;
    return &this->mVoltage;
}

template < typename T >
void Rmphn< T >::CalculateStateDependentValues()
{
    this->mPowerValue = 0;
    TwoPortWithChild< T >::CalculateStateDependentValues();
    if ( !this->mObservable )
        this->CalculateCurrentValue();
    this->mPowerValue += ::std::pow( this->mCurrentValue, 2.0 ) * mRser;
}
}    // namespace electrical

template class electrical::Rmphn< myMatrixType >;
