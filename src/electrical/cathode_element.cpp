// File: cathode_element.cpp
// Author: Christopher Abele

#include "cathode_element.h"

namespace electrical
{

template < typename T >
CathodeElement< T >::CathodeElement( const boost::shared_ptr< state::Soc >& socObject,
                                     const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable )
    : ElectrodeElement< T >( socObject, thermalState, observable )
{
}

template < typename T >
const char* CathodeElement< T >::GetName() const
{
    return "CathodeElement";
}

template < typename T >
size_t CathodeElement< T >::GetElementType() const
{
    return ElementType::CATHODE_ELEMENT;
}

}    // namespace electrical

template class electrical::CathodeElement< myMatrixType >;
