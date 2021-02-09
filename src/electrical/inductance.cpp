/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : inductance.cpp
* Creation Date : 22-04-2015
* Last Modified : Mi 22 Apr 2015 23:40:52 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "inductance.h"

namespace electrical
{

template < typename T >
Inductance< T >::Inductance( boost::shared_ptr< object::Object< double > > obj, const bool observable,
                             typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
{
}

template < typename T >
Inductance< T >::Inductance( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : Inductance< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
T* Inductance< T >::GetVoltage()
{
    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoSimulationPossible", GetName() );
    return 0;
}

template < typename T >
const char* Inductance< T >::GetName() const
{
    return "Inductance";
}

template < typename T >
void Inductance< T >::SetResistanceFactor( const double factor )
{
    this->mObject->SetFactor( factor );
}

}    // namespace electrical

template class electrical::Inductance< myMatrixType >;
