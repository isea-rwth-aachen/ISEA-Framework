// File: electrode_element.h
// Author: Christopher Abele

#ifndef _ELECTRODE_ELEMENT_
#define _ELECTRODE_ELEMENT_

#include "twoport_with_state.h"
#include "voltagesource.h"

namespace electrical
{

enum ElementType
{
    ELECTRODE_ELEMENT,
    ANODE_ELEMENT,
    CATHODE_ELEMENT
};

/// Basic behaviour of a cell electrode element with an electrical circuit which is represented through a serial TwoPort
/// with an Soc
template < typename T = myMatrixType >
class ElectrodeElement : public TwoPortWithState< T >
{
    public:
    ElectrodeElement( const boost::shared_ptr< state::Soc >& soc,
                      const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable = false );

    virtual ~ElectrodeElement(){};

    virtual const char* GetName() const;

    virtual size_t GetElementType() const;

    virtual ScalarUnit GetElectrodePotentialValue() const;

    virtual void AddChild( boost::shared_ptr< TwoPort< T > > newport );

    protected:
    boost::shared_ptr< electrical::VoltageSource< T > > mElectrodePotential;
};

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

}    // End namespace electrical

#endif    // _ELECTRODE_ELEMENT_
