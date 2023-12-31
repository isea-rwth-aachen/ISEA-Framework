// File: electrode_element.cpp
// Author: Christopher Abele

#include "electrode_element.h"

namespace electrical
{

template < typename T >
ElectrodeElement< T >::ElectrodeElement( const boost::shared_ptr< state::Soc >& soc,
                                         const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable )
    : TwoPortWithState< T >( soc, thermalState, observable )
{
}

template < typename T >
const char* ElectrodeElement< T >::GetName() const
{
    return "ElectrodeElement";
}

template < typename T >
size_t ElectrodeElement< T >::GetElementType() const
{
    return ElementType::ELECTRODE_ELEMENT;
}

template < typename T >
ScalarUnit ElectrodeElement< T >::GetElectrodePotentialValue() const
{
    if ( !mElectrodePotential )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException",
                                             "mElectrodePotential", "ElectrodeElement" );

    mElectrodePotential->GetVoltage();    // update voltage value
    return mElectrodePotential->GetVoltageValue();
}

template < typename T >
void ElectrodeElement< T >::AddChild( boost::shared_ptr< TwoPort< T > > newport )
{
    SerialTwoPort< T >::AddChild( newport );

    if ( strcmp( newport->GetName(), "VoltageSource" ) == 0 && !mElectrodePotential )
        mElectrodePotential = boost::static_pointer_cast< electrical::VoltageSource< T > >( newport );
}


}    // namespace electrical

template class electrical::ElectrodeElement< myMatrixType >;
