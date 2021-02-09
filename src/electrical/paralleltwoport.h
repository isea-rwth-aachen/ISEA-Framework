/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : paralleltwoport.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 15:48:42 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _PARALLELTWOPORT_
#define _PARALLELTWOPORT_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "twoport_withchild.h"

class TestTwoPortsWithChildren;

namespace electrical
{

/// This TwoPort represents the parallel branch of a circuit and can have children. For every child above 1, we have to
/// add anoterh equation to our algebraic system
template < typename T = myMatrixType >
class ParallelTwoPort : public TwoPortWithChild< T >
{
    friend class ::TestDaeSystem;
    friend class ::TestTwoPortsWithChildren;

    public:
    explicit ParallelTwoPort( const bool observable = false,
                              typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~ParallelTwoPort(){};

    virtual void SetCurrent( const T current );    ///< Set current for all the children. For "n" branchee in the parallel circuit we need "n-1" Variables to represent the current
    virtual void SetCurrent( const ScalarUnit currentval );    ///< Sets the current value

    size_t GetParallelChildren() const;    ///< Count the children of this TwoPort

    virtual T* GetVoltage();

    virtual void SetSystem( systm::StateSystemGroup< T >* stateSystemGroup );    ///< For every child above 1 Increase the count of parallel UIDs

    virtual const char* GetName() const;

    virtual double GetTotalCapacity() const;

    private:
    protected:
    std::vector< size_t > mUIDs;
};

}    // namespace electrical
#endif /* _PARALLELTWOPORT_ */
