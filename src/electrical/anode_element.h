// File: anode_element.h
// Author: Christopher Abele

#ifndef _ANODE_ELEMENT_
#define _ANODE_ELEMENT_

#include "electrode_element.h"

namespace electrical
{

/// Basic behaviour of the an anode element with an electrical circuit which is represented through a SerialTwoPort
template < typename T = myMatrixType >
class AnodeElement : public ElectrodeElement< T >
{
    public:
    AnodeElement( const boost::shared_ptr< state::Soc >& socObject,
                  const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable = false );

    virtual ~AnodeElement(){};

    virtual const char* GetName() const;

    virtual size_t GetElementType() const;
};

}    // End namespace electrical

#endif    // _ANODE_ELEMENT_
