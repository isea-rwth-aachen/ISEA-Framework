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

template < typename T >
VoltageSource< T >::VoltageSource( boost::shared_ptr< object::Object< double > > obj, const bool observable,
                                   typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( obj, observable, dataValues )
{
}

template < typename T >
VoltageSource< T >::VoltageSource( object::Object< double >* obj, const bool observable, typename TwoPort< T >::DataType dataValues )
    : VoltageSource< T >( boost::shared_ptr< object::Object< double > >( obj ), observable, dataValues )
{
}

template < typename T >
void VoltageSource< T >::CalculateStateDependentValues()
{
    if ( this->mObservable )
        this->CalculateCurrentValue();
}


template < typename T >
T* VoltageSource< T >::GetVoltage()
{
    this->mVoltageValue = this->GetValue();
    this->mVoltage.coeffRef( 0, this->mCurrent.cols() - 1 ) = this->mVoltageValue;
    return TwoPort< T >::GetVoltage();
}

template < typename T >
ScalarUnit VoltageSource< T >::GetValue() const
{
#ifndef _SYMBOLIC_
    return ElectricalElement< T >::GetValue();
#else
    return ScalarUnit( misc::StrCont( "ID" ) + misc::StrCont( TwoPort< T >::mID ) + "_ObjV" );
#endif
}


template < typename T >
const char* VoltageSource< T >::GetName() const
{
    return "VoltageSource";
}

}    // namespace electrical
#endif /* _VOLTAGESOURCE_ */
