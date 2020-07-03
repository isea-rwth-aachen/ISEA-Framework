// File: cathode_element.h
// Author: Christopher Abele

#ifndef _CATHODE_ELEMENT_
#define _CATHODE_ELEMENT_

#include "electrode_element.h"

namespace electrical
{

/// Basic behaviour of the cathode element with an electrical circuit which is represented through a SerialTwoPort
template < typename T = myMatrixType >
class CathodeElement : public ElectrodeElement< T >
{
    public:
    CathodeElement( const boost::shared_ptr< state::Soc >& socObject,
                    const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable = false );

    virtual ~CathodeElement(){};

    virtual const char* GetName() const;

    virtual size_t GetElementType() const;
};

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


}    // End namespace electrical

#endif    // _CATHODE_ELEMENT_
