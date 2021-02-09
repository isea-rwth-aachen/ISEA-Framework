/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : parallelru.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 15:51:13 CET
* Created By : Lee
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ParallelRU_
#define _ParallelRU_

// STD
#include <cmath>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../object/object.h"
#include "electricalelement.h"

class TestFactoryLookUpCellElements;

namespace electrical
{

/// This TwoPort describes a ParallelRU e.g. OCV
template < typename T = myMatrixType >
class ParallelRCAlg : public ElectricalElement< T >
{
    friend class ::TestFactoryLookUpCellElements;

    public:
    explicit ParallelRCAlg( boost::shared_ptr< object::Object< double > > objR,
                            boost::shared_ptr< object::Object< double > > objC, const bool observable = false,
                            typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit ParallelRCAlg( object::Object< double >* objR, object::Object< double >* objC, const bool observable = false,
                            typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~ParallelRCAlg(){};

    virtual T* GetVoltage();
    virtual void CalculateStateDependentValues();
    virtual void SetResistanceFactor( const double factor );

    const object::Object< double >* GetCObject() const { return mObjectC.get(); };
    virtual const char* GetName() const;

    private:
    boost::shared_ptr< object::Object< double > > mObjectC;    ///< This object decides the behaviour of the class, wheter it returns constant values or does a lookup for capacity


    protected:
};

}    // namespace electrical
#endif /* _ParallelRU_ */
