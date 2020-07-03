#ifndef _TWOPORT_WITH_STATE_
#define _TWOPORT_WITH_STATE_

#include "../state/soc.h"
#include "../state/thermal_state.h"
#include "serialtwoport.h"

extern template class electrical::SerialTwoPort< myMatrixType >;

namespace electrical
{
template < typename T = myMatrixType >
class TwoPortWithState : public SerialTwoPort< T >
{
    friend class ::TestTwoPortsWithChildren;

    public:
    TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                      const boost::shared_ptr< state::ThermalState< double > >& thermalState, const bool observable = false,
                      typename TwoPort< T >::DataType dataValues = typename TwoPort< T >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    virtual ~TwoPortWithState(){};

    virtual const char* GetName() const;

    virtual double GetSocValue() const;

    virtual void CalculateStateDependentValues();

    virtual double GetTotalCapacity() const
    {
        return this->mSoc->template GetActualCapacity< state::SocGetFormat::AH >();
    }

    const boost::shared_ptr< state::Soc >& GetSoc() const { return this->mSoc; }
    const boost::shared_ptr< state::ThermalState< double > >& GetThermalState() const { return mThermalState; }

    protected:
    const boost::shared_ptr< state::Soc > mSoc;
    const boost::shared_ptr< state::ThermalState< double > > mThermalState;
};

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

    // for observable TwoPorts, the current is already calculated
    if ( !this->IsObservable() )
        this->CalculateCurrentValue();

#ifndef _SYMBOLIC_
    // Update the Soc state. Therefore calculate the charge which is stored/released in the previous simulation step
    // period
    mSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( this->mCurrentValue * this->mStateSystemGroup->mDt );
#endif
}
}    // namespace electrical
#endif
