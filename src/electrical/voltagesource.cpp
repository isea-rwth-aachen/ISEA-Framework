/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : voltagesource.cpp
* Creation Date : 20-03-2013
* Last Modified : Di 01 Okt 2013 23:10:33 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "voltagesource.h"

namespace electrical
{

template < typename T >
VoltageSource< T >::VoltageSource( boost::shared_ptr< object::Object< double > > obj, const bool observable,
                                   typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
{
}

template < typename T >
VoltageSource< T >::VoltageSource( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : VoltageSource< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
void VoltageSource< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
        this->CalculateCurrentValue();
}


template < typename T >
T* VoltageSource< T >::GetVoltage()
{
    this->mVoltageValue = this->GetValue();
    this->mVoltage.coeffRef( 0, this->mCurrent.cols() - 1 ) = this->mVoltageValue;
    return TwoPort< T >::GetVoltage();
}

template < typename T >
ScalarUnit VoltageSource< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ElectricalElement< T >::GetValue();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjV" );
#endif
}


template < typename T >
const char* VoltageSource< T >::GetName() const
{
    return "VoltageSource";
}
}    // namespace electrical

template class electrical::VoltageSource< myMatrixType >;
