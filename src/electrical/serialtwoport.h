/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : serialtwoport.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 16:05:55 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SERIALTWOPORT_
#define _SERIALTWOPORT_

// ETC
#include "twoport_withchild.h"

class TestTwoPortsWithChildren;
class TestDaeSystem;

namespace electrical
{
/// Serial Connector for all Elements. Can have any TwoPort as a child
template < typename T = myMatrixType >
class SerialTwoPort : public TwoPortWithChild< T >
{
    friend class ::TestTwoPortsWithChildren;
    friend class ::TestDaeSystem;

    public:
    explicit SerialTwoPort( const bool observable = false,
                            typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~SerialTwoPort(){};

    virtual T* GetVoltage();

    virtual double GetTotalCapacity() const;

    virtual bool IsSerialTwoPort() const;
    virtual const char* GetName() const;
};

}    // namespace electrical
#endif /* _SERIALTWOPORT_ */
