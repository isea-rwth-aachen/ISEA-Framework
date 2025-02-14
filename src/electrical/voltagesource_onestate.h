#ifndef _VOLTAGESOURCEONESTATE_
#define _VOLTAGESOURCEONESTATE_

//#pragma once
// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../object/Object.h"
#include "../state/soc.h"
#include "electricalelement.h"
#include <vector>

class TestSurfaceSoc;

namespace electrical
{

/// This TwoPort describes a VoltageSource with the hysteresis using the Discrete Preisach model
template < typename T = myMatrixType >
class VoltageSource_OneState : public ElectricalElement< T >
{

    friend class ::TestSurfaceSoc;

    public:
   /* explicit VoltageSource_OneState(
     boost::shared_ptr< object::Object< double > > obj1, boost::shared_ptr< object::Object< double > > obj2,
     boost::shared_ptr< object::Object< double > > obj3, double double1, double double2,
     boost::shared_ptr< state::State > , const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ));

    explicit VoltageSource_OneState( object::Object< double >* obj1, object::Object< double >* obj2, object::Object< double >* obj3, double double1,
     double double2, boost::shared_ptr< state::State > , const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) ); */
    explicit VoltageSource_OneState(
     boost::shared_ptr< object::Object< double > > obj1, boost::shared_ptr< object::Object< double > > obj2,
     boost::shared_ptr< object::Object< double > > obj3, double double1, double double2,
     const bool observable = false,  typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    explicit VoltageSource_OneState(
     object::Object< double >* obj1, object::Object< double >* obj2, object::Object< double >* obj3, double double1,
     double double2, const bool observable = false,
     typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~VoltageSource_OneState(){};
    virtual void CalculateStateDependentValues();
    virtual T* GetVoltage();
    virtual ScalarUnit GetValue() const;    ///< Get Value of Object

    double StateVariableCal();    // calculates state variable, h[k]

    virtual const char* GetName() const { return "VoltageSource_OneState"; };

    boost::shared_ptr< state::State > mCrate;

    boost::shared_ptr< object::Object< double > > mHysteresisWidth;    // M (SoC,Temp)
    boost::shared_ptr< object::Object< double > > mTimeConstant;    // time constant (SOC, Temp)
    double initialcapacity;       //  initial capacity
    double coulombefficiency = 1;    //  coulombefficiency, usually 1      
    double previousState = 0;    // state variable h[k-1], initialized at 0

    private:
    protected:
    
};


}    // namespace electrical
#endif /* _VOLTAGESOURCEONESTATE_ */