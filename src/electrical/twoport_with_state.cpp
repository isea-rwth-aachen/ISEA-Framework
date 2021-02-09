#include "twoport_with_state.h"

namespace electrical
{

template < typename T >
TwoPortWithState< T >::TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                                         const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                                         const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
    , mSoc( socState )
    , mThermalState( thermalState )
{
}

template < typename T >
const char* TwoPortWithState< T >::GetName() const
{
    return "TwoPortWithState";
}

template < typename T >
double TwoPortWithState< T >::GetSocValue() const
{
    return mSoc->GetValue();
}

template < typename T >
void TwoPortWithState< T >::CalculateStateDependentValues()
{
    SerialTwoPort< T >::CalculateStateDependentValues();

    // for observable TwoPorts, the the values are already calculated
    if ( !this->IsObservable() )
    {
        this->CalculateCurrentValue();
        this->CalculateVoltageValue();
    }

#ifndef _SYMBOLIC_
    // Update the Soc state. Therefore calculate the charge which is stored/released in the previous simulation step
    // period
    mSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( this->mCurrentValue * this->mStateSystemGroup->mDt );
#endif
}

}    // namespace electrical

template class electrical::TwoPortWithState< myMatrixType >;
