// File: anode_element.cpp
// Author: Christopher Abele

#include "anode_element.h"

namespace electrical
{
template < typename T >
AnodeElement< T >::AnodeElement( const boost::shared_ptr< state::Soc >& socObject,
                                 const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable )
    : ElectrodeElement< T >( socObject, thermalState, observable )
{
}

template < typename T >
const char* AnodeElement< T >::GetName() const
{
    return "AnodeElement";
}

template < typename T >
size_t AnodeElement< T >::GetElementType() const
{
    return ElementType::ANODE_ELEMENT;
}
}    // namespace electrical

template class electrical::AnodeElement< myMatrixType >;