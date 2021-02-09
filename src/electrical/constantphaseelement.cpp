/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : constantphaseelement.cpp
* Creation Date : 22-04-2015
* Last Modified : Mi 22 Apr 2015 23:20:48 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "constantphaseelement.h"

namespace electrical
{

template < typename T >
ConstantPhaseElement< T >::ConstantPhaseElement( boost::shared_ptr< object::Object< double > > obj,
                                                 const bool observable, typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
{
}

template < typename T >
ConstantPhaseElement< T >::ConstantPhaseElement( object::Object< double >* obj, const bool observable,
                                                 typename TwoPort< T >::DataType dataValues )
    : ConstantPhaseElement< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
T* ConstantPhaseElement< T >::GetVoltage()
{
    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoSimulationPossible", GetName() );
    return 0;
}

template < typename T >
const char* ConstantPhaseElement< T >::GetName() const
{
    return "ConstantPhaseElement";
}

template < typename T >
void ConstantPhaseElement< T >::SetResistanceFactor( const double factor )
{
    this->mObject->SetFactor( factor );
}

}    // namespace electrical

template class electrical::ConstantPhaseElement< myMatrixType >;
