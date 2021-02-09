#include "ohmicresistance.h"

namespace electrical
{

template < typename T >
OhmicResistance< T >::OhmicResistance( boost::shared_ptr< object::Object< double > > obj, const bool observable,
                                       typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
{
}

template < typename T >
OhmicResistance< T >::OhmicResistance( object::Object< double >* obj, const bool observable,

                                       typename TwoPort< T >::DataType dataValues )
    : OhmicResistance< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
void OhmicResistance< T >::CalculateStateDependentValues()
{
    this->CalculateCurrentValue();

    if ( this->mObservable )
        this->CalculateVoltageValue();

    this->mPowerValue = ::std::pow( this->mCurrentValue, 2.0 ) * this->GetValue();
}

template < typename T >
T* OhmicResistance< T >::GetVoltage()
{
    this->mVoltage = this->mCurrent * this->GetValue();
    return TwoPort< T >::GetVoltage();
}

template < typename T >
ScalarUnit OhmicResistance< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ElectricalElement< T >::GetValue();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjR" );
#endif
}


template < typename T >
const char* OhmicResistance< T >::GetName() const
{
    return "OhmicResistance";
}

template < typename T >
void OhmicResistance< T >::SetResistanceFactor( const double factor )
{
    this->mObject->SetFactor( factor );
}

}    // namespace electrical

template class electrical::OhmicResistance< myMatrixType >;
