
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : parallelRCAlg.cpp
* Creation Date : 20-03-2013
* Last Modified : Di 01 Okt 2013 23:20:14 CEST
* Created By : Lee
_._._._._._._._._._._._._._._._._._._._._.*/
#include "parallelRCAlg.h"

namespace electrical
{

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

template class electrical::ParallelRCAlg< myMatrixType >;