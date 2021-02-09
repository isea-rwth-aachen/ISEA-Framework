/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : parallelrc.h
* Creation Date : 10-11-2012
* Last Modified : Do 10 Mär 2016 12:36:32 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _PARALLELRC_
#define _PARALLELRC_

// STD
#include <cmath>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "electricalelement.h"

// friend
class TestElectricalElement;
class TestElectricalFactory;
class TestXML;

namespace electrical
{
/// This is an optimzed version of a parallel RC circuit. It mimics the equation  dU = ib/c - Uc /(RC).  This is one
/// equation less than through a parallel R and C Element
template < typename T = myMatrixType >
class ParallelRC : public ElectricalElement< T >
{
    friend class ::TestElectricalElement;
    friend class ::TestXML;
    friend class ::TestElectricalFactory;

    public:
    explicit ParallelRC( boost::shared_ptr< object::Object< double > > objR,
                         boost::shared_ptr< object::Object< double > > objC, const bool observable = false,
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit ParallelRC( object::Object< double >* objR, object::Object< double >* objC, const bool observable = false,
                         typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~ParallelRC(){};

    virtual T* GetVoltage();                                   ///< returns the voltage over the RC element  Urc  = 1
    virtual void SetCurrent( const T current );                ///< Sets the current
    virtual void SetCurrent( const ScalarUnit currentval );    ///< Sets the current value
    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );
    virtual void CalculateStateDependentValues();
    virtual ScalarUnit GetValueC() const;    ///< Get Value of Object
    virtual ScalarUnit GetValue() const;     ///< Get Value of Object

    const object::Object< double >* GetTauObject() const;
    virtual ScalarUnit GetTauValue() const;    ///< Get value of tau Object
    virtual void SetResistanceFactor( const double factor );

    virtual const char* GetName() const;

    private:
    ParallelRC();
    void SetVoltage();

    size_t mUID;

    boost::shared_ptr< object::Object< double > > mObjectTau;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup for capacity

    T mConstVoltageVector;
    T mDGLPart;

    protected:
};

}    // namespace electrical
#endif /* _PARALLELRC_ */
