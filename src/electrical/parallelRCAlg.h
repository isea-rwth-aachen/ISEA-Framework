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

template < typename T >
ParallelRCAlg< T >::ParallelRCAlg( boost::shared_ptr< object::Object< double > > objR,
                                   boost::shared_ptr< object::Object< double > > objC, const bool observable,
                                   typename TwoPort< T >::DataType dataValues )
    : ElectricalElement< T >( objR, observable, dataValues )
    , mObjectC( objC )
{
}

template < typename T >
ParallelRCAlg< T >::ParallelRCAlg( object::Object< double >* objR, object::Object< double >* objC,
                                   const bool observable, typename TwoPort< T >::DataType dataValues )
    : ParallelRCAlg< T >( boost::shared_ptr< object::Object< double > >( objR ),
                          boost::shared_ptr< object::Object< double > >( objC ), observable, dataValues )
{
}

template < typename T >
void ParallelRCAlg< T >::CalculateStateDependentValues()
{
    this->CalculateCurrentValue();

    this->CalculateVoltageValue();
    this->mPowerValue = ::std::pow( this->mVoltageValue, 2 ) / this->GetValue();
}

template < typename T >
T* ParallelRCAlg< T >::GetVoltage()
{
#ifndef _SYMBOLIC_
    const double c = mObjectC->GetValue();
    const double r = this->GetValue();

    this->mVoltageValue = ( this->mVoltageValue * ( 1 - this->mStateSystemGroup->mDt / ( 2 * r * c ) ) +
                            this->mCurrentValue / c * this->mStateSystemGroup->mDt ) /
                          ( 1 + this->mStateSystemGroup->mDt / ( 2 * r * c ) );

    this->mVoltage.coeffRef( 0, this->mCurrent.cols() - 1 ) = this->mVoltageValue;
#endif
    return TwoPort< T >::GetVoltage();
}

template < typename T >
const char* ParallelRCAlg< T >::GetName() const
{
    return "ParallelRCAlg";
}

template < typename T >
void ParallelRCAlg< T >::SetResistanceFactor( const double factor )
{
    this->mObject->SetFactor( factor );
}

}    // namespace electrical
#endif /* _ParallelRU_ */
