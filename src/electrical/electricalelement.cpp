#include "electricalelement.h"

namespace electrical
{

template < typename T >
ElectricalElement< T >::ElectricalElement( boost::shared_ptr< object::Object< double > > obj, const bool observable,
                                           typename TwoPort< T >::DataType dataValues )
    : TwoPort< T >( observable, dataValues )
    , mObject( obj )
{
}

template < typename T >
ElectricalElement< T >::ElectricalElement( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
ScalarUnit ElectricalElement< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ( *mObject )();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjBase" );
#endif
}
template < typename T >
object::Object< double >* ElectricalElement< T >::operator()()
{
    return mObject.get();
}

template < typename T >
const object::Object< double >* ElectricalElement< T >::GetElementObject() const
{
    return mObject.get();
}

template < typename T >
const char* ElectricalElement< T >::GetName() const
{
    return "ElectricalElement";
}

}    // namespace electrical

template class electrical::ElectricalElement< myMatrixType >;
