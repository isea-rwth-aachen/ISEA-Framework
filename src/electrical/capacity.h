/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : capacity.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 15:49:04 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CAPACITY_
#define _CAPACITY_

// ETC
#include "electricalelement.h"

class TestDaeSystem;
class TestXML;
class TestTwoPortsWithChildren;

/// The electrical namespace contains all electrical elements and other classes that define an electrical element, e.g
/// SOC
namespace electrical
{
/// This class describes the the behavour of a capacity.  The differential equation it mimics dU  = 1 / C current
template < typename T = myMatrixType >
class Capacity : public ElectricalElement< T >
{
    friend class ::TestDaeSystem;
    friend class ::TestTwoPortsWithChildren;
    friend class ::TestXML;

    public:
    explicit Capacity( boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
                       typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit Capacity( object::Object< double >* obj, const bool observable = false,
                       typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~Capacity()= default;;

    virtual T* GetVoltage();    /// Returns Uc = 1

    virtual ScalarUnit GetValue() const;    ///< Get Value of Object
    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );

    virtual const char* GetName() const;

    private:
    void SetVoltage();

    size_t mUID;    /// Row number for DGL (DE)  system

    T mVoltageValue;

    bool mVoltageSet;

    protected:
};

}    // namespace electrical
#endif /* _CAPACITY_ */
