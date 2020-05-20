/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : warburgTanh.h
* Creation Date : 05-02-2014
* Last Modified : Fr 11 Mär 2016 11:08:10 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _WARBURGTA_
#define _WARBURGTA_
// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "serialtwoport.h"

class TestElectricalFactory;

namespace electrical
{

/// The WarburgTanh represents one solution to the Fick's law and is represented by a series of RC elements and an Ohmic
/// resistance in case the sample rate is too low.
/// The class itself is inheriting from a SerialTwoPort. Application is only useful if this class is created via the
/// factory method
template < typename T = myMatrixType >
class WarburgTanh : public SerialTwoPort< T >
{
    friend class ::TestElectricalFactory;

    public:
    explicit WarburgTanh( const bool observable = false,
                          typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    ~WarburgTanh(){};
    virtual const char* GetName() const;
    virtual double GetTotalCapacity() const
    {
        return 0;
    }    ///< If the element has a capacity (e.g. Cellelement) we return  its capacity


    private:
    protected:
};

template < typename T >
WarburgTanh< T >::WarburgTanh( const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
{
}

template < typename T >
const char* WarburgTanh< T >::GetName() const
{
    return "WarburgTanh";
}

}    // namespace electrical
#endif /* _WARBURGTA_ */
