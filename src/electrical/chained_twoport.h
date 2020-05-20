// File: chained_twoport.h
// Author: Christopher Abele

#ifndef _CHAINED_TWOPORT_
#define _CHAINED_TWOPORT_

#include "serialtwoport.h"

namespace electrical
{

template < typename T = myMatrixType >
class ChainedTwoPort : public SerialTwoPort< T >
{
    public:
    explicit ChainedTwoPort( const bool observable = false, typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType(new ElectricalDataStruct< ScalarUnit >));
    virtual ~ChainedTwoPort(){};

    const char* GetName() const;

};

template < typename T >
ChainedTwoPort< T >::ChainedTwoPort( const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
{
}

template < typename T >
const char* ChainedTwoPort< T >::GetName() const
{
    return "ChainedTwoport";
}

} // End namespace eletrical

#endif // _CHAINED_TWOPORT_
