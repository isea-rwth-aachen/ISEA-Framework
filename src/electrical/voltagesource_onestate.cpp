#include "voltagesource_onestate.h"
#include <math.h>

// TODO: change mSOC to mSOC at time t
namespace electrical
{

template < typename T >
VoltageSource_OneState< T >::VoltageSource_OneState( boost::shared_ptr< object::Object< double > > obj1,
                                                     boost::shared_ptr< object::Object< double > > obj2,
                                                     boost::shared_ptr< object::Object< double > > obj3, double double1,
                                                     double double2, const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj1, observable, dataValues )
    , mHysteresisWidth( obj2 )
    , mTimeConstant( obj3 )
    , initialcapacity( double1 )
    , coulombefficiency( double2 )
{
}

template < typename T >
VoltageSource_OneState< T >::VoltageSource_OneState( object::Object< double >* obj1, object::Object< double >* obj2,
                                                     object::Object< double >* obj3, double double1, double double2,
                                                     const bool observable, typename TwoPort< T >::DataType dataValues )
    : VoltageSource_OneState< T >( boost::shared_ptr< object::Object< double > >( obj1 ),
                                   boost::shared_ptr< object::Object< double > >( obj2 ),
                                   boost::shared_ptr< object::Object< double > >( obj3 ), double1, double2, observable, dataValues )
{
}

template < typename T >
void VoltageSource_OneState< T >::CalculateStateDependentValues()
{
    this->CalculateCurrentValue();
    StateVariableCal();
    this->mVoltageValue = ( this->mObject->GetValue() ) + ( ( *mHysteresisWidth )() * previousState / 2 );
    this->mVoltage.coeffRef( 0, this->mCurrent.cols() - 1 ) = this->mVoltageValue;
    this->mPowerValue = this->mCurrentValue * ( this->mVoltageValue - this->mObject->GetValue() );
}

template < typename T >
T* VoltageSource_OneState< T >::GetVoltage()
{
    return (TwoPort< T >::GetVoltage());
}

template < typename T >
double VoltageSource_OneState< T >::StateVariableCal()
{
    double i = this->mCurrentValue / initialcapacity;
    double oneState = 0;
    double diffEqSol = exp( -1 * mTimeConstant->GetValue() * this->mStateSystemGroup->mDt * abs( i ) * coulombefficiency/3600 );
    if ( i >= 0 )
        oneState = diffEqSol * ( previousState - 1 )  + 1;
    else
        oneState = diffEqSol * ( previousState + 1 ) - 1;
    if ( oneState < -1 )
        oneState = -1;
    if ( oneState > 1 )
        oneState = 1;
    previousState = oneState;
    return oneState;
}

template < typename T >
ScalarUnit VoltageSource_OneState< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ElectricalElement< T >::GetValue();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjV" );
#endif
}

}    // namespace electrical

template class electrical::VoltageSource_OneState< myMatrixType >;