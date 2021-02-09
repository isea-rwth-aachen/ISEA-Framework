/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : voltagesource.h
* Creation Date : 30-10-2012
* Last Modified : Di 08 Mär 2016 15:52:27 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VOLTAGESOURCE_
#define _VOLTAGESOURCE_

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../object/object.h"
#include "electricalelement.h"

class TestSurfaceSoc;

namespace electrical
{

/// This TwoPort describes a VoltageSource e.g. OCV
template < typename T = myMatrixType >
class VoltageSource : public ElectricalElement< T >
{
    friend class ::TestSurfaceSoc;

    public:
    explicit VoltageSource( boost::shared_ptr< object::Object< double > > obj, const bool observable = false,
                            typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    explicit VoltageSource( object::Object< double >* obj, const bool observable = false,
                            typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );
    virtual ~VoltageSource(){};

    virtual ScalarUnit GetValue() const;    ///< Get Value of Object
    virtual T* GetVoltage();
    virtual void CalculateStateDependentValues();
    virtual const char* GetName() const;

    private:
    protected:
};


}    // namespace electrical
#endif /* _VOLTAGESOURCE_ */
