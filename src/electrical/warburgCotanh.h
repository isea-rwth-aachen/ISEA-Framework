/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : warburgCotanh.h
* Creation Date : 05-02-2014
* Last Modified : Fr 11 Mär 2016 11:08:03 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _WARBURGCOTA_
#define _WARBURGCOTA_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "serialtwoport.h"
class TestElectricalFactory;

namespace electrical
{
/// The WarburgCotanh represents one solution to the Fick's law and is represented by a series of RC elements,  an Ohmic
/// resistance in case the sample rate is too low and a capacitance. The capacitance can be toggled via the xml-file.
/// The class itself is inheriting from a SerialTwoPort. Application is only useful if this class is created via the
/// factory method

template < typename T = myMatrixType >
class WarburgCotanh : public SerialTwoPort< T >
{
    friend class ::TestElectricalFactory;

    public:
    explicit WarburgCotanh( const bool observable = false,
                            typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    ~WarburgCotanh(){};
    virtual const char* GetName() const;

    virtual double GetTotalCapacity() const
    {
        return 0;
    }    ///< If the element has a capacity (e.g. Cellelement) we return  its capacity


    private:
    protected:
};

}    // namespace electrical
#endif /* _WARBURGCOTA_ */
