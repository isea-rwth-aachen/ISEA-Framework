#include "twoport_with_state.h"
#include <cmath>

namespace electrical
{

template < typename T >
TwoPortWithState< T >::TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                                         const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                                         const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
    , mSoc( socState )
    , mThermalState( thermalState )
    , CrateDefined( false )
    , CdirectionDefined( false )
    , LHDDefined( false )
{
}

template < typename T >
TwoPortWithState< T >::TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                                         const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                                         const boost::shared_ptr< state::CRateState< double > >& CrateState,
                                         const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
    , mSoc( socState )
    , mThermalState( thermalState )
    , mCrate( CrateState )
    , CrateDefined( true )
    , CdirectionDefined( false )
    , LHDDefined( false )
{
}

template < typename T >
TwoPortWithState< T >::TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                                         const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                                         const boost::shared_ptr< state::CDirection< double > >& cdirectionState,
                                         const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
    , mSoc( socState )
    , mThermalState( thermalState )
    , mCdirection( cdirectionState )
    , CrateDefined( false )
    , CdirectionDefined( true )
    , LHDDefined( false )
{
}

template < typename T >
TwoPortWithState< T >::TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                                         const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                                         const boost::shared_ptr< state::CRateState< double > >& CrateState,
                                         const boost::shared_ptr< state::LHDState< double > >& lhdState,
                                         const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
    , mSoc( socState )
    , mThermalState( thermalState )
    , mCrate( CrateState )
    , mLHD( lhdState )
    , CrateDefined( true )
    , CdirectionDefined( false )
    , LHDDefined( true )
{
}

template < typename T >
TwoPortWithState< T >::TwoPortWithState( const boost::shared_ptr< state::Soc >& socState,
                                         const boost::shared_ptr< state::ThermalState< double > >& thermalState,
                                         const boost::shared_ptr< state::CDirection< double > >& cdirectionState,
                                         const boost::shared_ptr< state::LHDState< double > >& lhdState,
                                         const bool observable, typename TwoPort< T >::DataType dataValues )
    : SerialTwoPort< T >( observable, dataValues )
    , mSoc( socState )
    , mThermalState( thermalState )
    , mCdirection( cdirectionState )  
    , mLHD( lhdState )
    , CrateDefined( false )
    , CdirectionDefined( true )
    , LHDDefined( true )
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
double TwoPortWithState< T >::GetCrateValue() const
{
    return mCrate->GetValue();
}

template < typename T >
double TwoPortWithState< T >::GetCdirectionValue() const
{
    return mCdirection->GetValue();
}

template < typename T >
double TwoPortWithState< T >::GetLHDValue() const
{
    return mLHD->GetValue();
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
    if ( CrateDefined )
    {
        mCrate->SetValue( this->mCurrentValue / this->GetTotalCapacity() );
    }

    if ( CdirectionDefined )
    {
        if ( std::signbit( this->mCurrentValue ) == 1)
        {
            mCdirection->SetValue( -1 );
        }
        else 
        {
            mCdirection->SetValue( 1 );
        }
        //std::cout << mCdirection->GetValue() << std::endl;
    }

    if ( LHDDefined )
    {
        mLHD->UpdateLHD( mSoc->GetValue(), mThermalState->GetValue(), mCrate->GetValue() );
    }
}

}    // namespace electrical

template class electrical::TwoPortWithState< myMatrixType >;
