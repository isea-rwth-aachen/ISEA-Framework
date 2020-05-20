/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : rmphn.h
* Creation Date : 24-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _RMN_
#define _RMN_

#include "../object/object.h"
#include "serialtwoport.h"
#include <cmath>
#include <numeric>

extern template class electrical::SerialTwoPort< myMatrixType >;
extern template class object::Object< double >;

namespace electrical
{

/// DESCRIPTION
template < typename T = myMatrixType >
class Rmphn : public SerialTwoPort< T >
{
    public:
    explicit Rmphn( boost::shared_ptr< object::Object< double > > rCtObj,
                    boost::shared_ptr< object::Object< double > > rMpObj, const bool observable = false,
                    typename SerialTwoPort< T >::DataType dataValues = typename SerialTwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) )
        : SerialTwoPort< T >( observable, dataValues )
        , mR_Ct( rCtObj )
        , mRMp( rMpObj ){};


    virtual T* GetVoltage();    ///< returns the voltage over the RC element  Urc  = 1

    virtual ~Rmphn(){};
    virtual void CalculateStateDependentValues();

    virtual double GetTotalCapacity() const
    {
        return 0;
    }    ///< If the element has a capacity (e.g. Cellelement) we return  its capacity


    std::vector< boost::shared_ptr< object::Object< double > > > GetObjects() const { return {mRMp, mR_Ct}; };
    virtual const char* GetName() const { return "Rmphn"; };

    private:
    boost::shared_ptr< object::Object< double > > mR_Ct;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    boost::shared_ptr< object::Object< double > > mRMp;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup
    double mRser = 0;

    protected:
};

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
#endif /* _RMN_ */
