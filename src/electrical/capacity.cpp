#include "capacity.h"

namespace electrical
{

template < typename T >
Capacity< T >::Capacity( boost::shared_ptr< object::Object< double > > obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
    , mUID( 0 )
    , mVoltageValue( T() )
    , mVoltageSet( false )
{
}

template < typename T >
Capacity< T >::Capacity( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : Capacity< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
void Capacity< T >::SetVoltage()
{
    mVoltageValue.resize( this->mCurrent.rows(), this->mCurrent.cols() );
    mVoltageValue.setZero();
    mVoltageValue.coeffRef( 0, mUID ) = 1;
    mVoltageSet = true;
}

template < typename T >
T* Capacity< T >::GetVoltage()
{
    if ( !mVoltageSet )
        SetVoltage();

    if ( this->HasSystem() )
        this->mStateSystemGroup->mDglStateSystem.AddEquations( mUID, this->mCurrent / this->GetValue() );

    this->mVoltage = mVoltageValue;
    return TwoPort< T >::GetVoltage();
}

template < typename T >
ScalarUnit Capacity< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ElectricalElement< T >::GetValue();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjC" );
#endif
}


template < typename T >
void Capacity< T >::SetSystem( systm::StateSystemGroup< T >* stateSystemGroup )
{
    mUID = stateSystemGroup->mDglStateSystem.GetNewEquation();
    TwoPort< T >::SetSystem( stateSystemGroup );
}
template < typename T >
const char* Capacity< T >::GetName() const
{
    return "Capacity";
}

}    // namespace electrical

template class electrical::Capacity< myMatrixType >;
