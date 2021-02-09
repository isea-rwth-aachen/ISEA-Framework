#ifdef BUILD_AGING_SIMULATION

#ifndef _AGING_TWOPORT_
#define _AGING_TWOPORT_

#include "../aging/aging_base.h"
#include "../electrical/cellelement.h"
#include "../electrical/twoport_with_state.h"
#include "../state/aging_state.h"

namespace aging
{
template < typename MatrixType >
class AgingTwoPort
{
    public:
    AgingTwoPort( const std::vector< boost::shared_ptr< AgingBase > > &agingEffects,
                  const boost::shared_ptr< electrical::TwoPortWithState< MatrixType > > &electricalTwoPort,
                  const boost::shared_ptr< electrical::Cellelement< MatrixType > > &cell,
                  const boost::shared_ptr< state::AgingState > &agingState = nullptr );
    virtual void CollectData( const double &timestep );
    virtual void ResetToPointInTime( const double &time );
    virtual void CalculateAging( const double &timestep, const double &scaleFactor );
    virtual const boost::shared_ptr< state::AgingState > &GetAgingState() const;
    virtual const boost::shared_ptr< electrical::TwoPortWithState< MatrixType > > &GetTwoPort() const;
    virtual const std::vector< boost::shared_ptr< AgingBase > > &GetAgingEffects() const;
    virtual void SetAgingEffects( const std::vector< boost::shared_ptr< AgingBase > > &agingEffects );
    virtual void SetChildren( const std::vector< boost::shared_ptr< AgingTwoPort > > &children );
    virtual void AddChild( const boost::shared_ptr< AgingTwoPort > &child );
    /// ignore the aging effects and force a new aging state
    virtual void SetAging( const double &capacityFactor, const double &resistanceFactor, const double &socOffset,
                           const double &timestep );
    virtual const std::vector< boost::shared_ptr< AgingTwoPort > > &GetChildren() const;

    private:
    virtual void BuildTwoportState( const electrical::TwoPortWithState< MatrixType > &twoPort,
                                    const electrical::Cellelement< MatrixType > &cell );
    virtual void ApplyAging( const double &timestep, const double &scaleFactor );
    virtual void CalculateCellCapacity();

    std::vector< boost::shared_ptr< AgingBase > > mAgingEffects;
    /// aging of child elements, e. g. electrodes, that must be calculated first
    std::vector< boost::shared_ptr< AgingTwoPort > > mChildren;
    boost::shared_ptr< electrical::TwoPortWithState< MatrixType > > mElectricalTwoPort;
    aging::TwoportState mTwoportState;
    aging::TwoportState mCellState;
    boost::shared_ptr< state::AgingState > mAgingState;

    /// used to calculate the charge throughput
    double mPreviousStoredCharge;
    double mChargeThroughputSinceAgingStep;
};

template < typename MatrixType >
AgingTwoPort< MatrixType >::AgingTwoPort( const std::vector< boost::shared_ptr< AgingBase > > &agingEffects,
                                          const boost::shared_ptr< electrical::TwoPortWithState< MatrixType > > &electricalTwoPort,
                                          const boost::shared_ptr< electrical::Cellelement< MatrixType > > &cell,
                                          const boost::shared_ptr< state::AgingState > &agingState )
    : mAgingEffects( agingEffects )
    , mChildren()
    , mElectricalTwoPort( electricalTwoPort )
    , mAgingState( agingState )
    , mPreviousStoredCharge( mElectricalTwoPort->GetSoc()->template GetValue< state::SocGetFormat::AS >() )
    , mChargeThroughputSinceAgingStep( 0.0 )
{
    if ( !mAgingState )
    {
        mAgingState = boost::make_shared< state::AgingState >( 0, 0 );
    }
    BuildTwoportState( *mElectricalTwoPort, *cell );
};

template < typename MatrixType >
void AgingTwoPort< MatrixType >::BuildTwoportState( const electrical::TwoPortWithState< MatrixType > &twoPort,
                                                    const electrical::Cellelement< MatrixType > &cell )
{
    mTwoportState.mElectricalData = twoPort.GetElectricalData();
    mTwoportState.mSocState = twoPort.GetSoc();
    mTwoportState.mThermalState = twoPort.GetThermalState();
    mTwoportState.mCellAge = mAgingState->mCellAge;
    mTwoportState.mChargeThroughput = mAgingState->mChargeThroughput;

    mCellState.mElectricalData = cell.GetElectricalData();
    mCellState.mSocState = cell.GetSoc();
    mCellState.mThermalState = cell.GetThermalState();
    mCellState.mCellAge = mAgingState->mCellAge;
    mCellState.mChargeThroughput = mAgingState->mChargeThroughput;
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::CollectData( const double &timestep )
{
    for ( auto &child : mChildren )
    {
        child->CollectData( timestep );
    }

    double charge = mTwoportState.mElectricalData->mCurrentValue * timestep;
    if ( charge < 0 )
        mChargeThroughputSinceAgingStep -= charge;

    for ( auto &agingEffect : mAgingEffects )
    {
        agingEffect->CollectData( mTwoportState, mCellState, timestep );
    }
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::ResetToPointInTime( const double &time )
{
    for ( auto &child : mChildren )
    {
        child->ResetToPointInTime( time );
    }
    for ( auto &agingEffect : mAgingEffects )
    {
        agingEffect->ResetToPointInTime( time );
    }
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::CalculateAging( const double &timestep, const double &scaleFactor )
{
    for ( auto &child : mChildren )
    {
        child->CalculateAging( timestep, scaleFactor );
    }

    // if this is a cell with aging electrodes, the new capacity is calculated by the cell using the voltage limits
    if ( mElectricalTwoPort->IsCellelement() && !mChildren.empty() )
    {
        CalculateCellCapacity();
    }
    else
    {
        mTwoportState.mCellAge = mAgingState->GetCellAge() + timestep * scaleFactor;
        mTwoportState.mChargeThroughput = mAgingState->GetChargeThroughput() + mChargeThroughputSinceAgingStep * scaleFactor;

        double capacityFactor = 1;
        double socOffset = 0;
        double resistanceFactor = 1;

        for ( auto &agingEffect : mAgingEffects )
        {
            agingEffect->CalculateAging( mTwoportState, timestep, scaleFactor );
            capacityFactor -= ( 1 - agingEffect->GetCapacityFactor() );
            socOffset += agingEffect->GetSocOffset();
            resistanceFactor -= ( 1 - agingEffect->GetResistanceFactor() );
        }

        mAgingState->mCapacityFactor = capacityFactor;
        mAgingState->mSocOffset = socOffset;
        mAgingState->mResistanceFactor = resistanceFactor;
    }

    ApplyAging( timestep, scaleFactor );
}

template < typename MatrixType >
const boost::shared_ptr< state::AgingState > &AgingTwoPort< MatrixType >::GetAgingState() const
{
    return mAgingState;
}

template < typename MatrixType >
const boost::shared_ptr< electrical::TwoPortWithState< MatrixType > > &AgingTwoPort< MatrixType >::GetTwoPort() const
{
    return mElectricalTwoPort;
}

template < typename MatrixType >
const std::vector< boost::shared_ptr< AgingBase > > &AgingTwoPort< MatrixType >::GetAgingEffects() const
{
    return mAgingEffects;
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::SetAgingEffects( const std::vector< boost::shared_ptr< AgingBase > > &agingEffects )
{
    mAgingEffects = agingEffects;
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::ApplyAging( const double &timestep, const double &scaleFactor )
{
    mElectricalTwoPort->GetSoc()->template SetCapacityFactor< state::SocSetFormat::ABSOLUT >( this->mAgingState->GetCapacityFactor() );
    mElectricalTwoPort->SetResistanceFactor( this->mAgingState->GetResistanceFactor() );
    mElectricalTwoPort->GetSoc()->template SetOffset< state::SocSetFormat::ABSOLUT >( this->mAgingState->GetSocOffset() );

    this->mAgingState->SetChargeThroughput( mChargeThroughputSinceAgingStep * scaleFactor, state::AgingStateSetFormat::DELTA );
    this->mAgingState->SetCellAge( timestep * scaleFactor, state::AgingStateSetFormat::DELTA );
    mChargeThroughputSinceAgingStep = 0;
    mTwoportState.mCellAge = mAgingState->mCellAge;
    mTwoportState.mChargeThroughput = mAgingState->mChargeThroughput;
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::SetChildren( const std::vector< boost::shared_ptr< AgingTwoPort > > &children )
{
    this->mChildren = children;
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::AddChild( const boost::shared_ptr< AgingTwoPort > &child )
{
    this->mChildren.push_back( child );
}

template < typename MatrixType >
const std::vector< boost::shared_ptr< AgingTwoPort< MatrixType > > > &AgingTwoPort< MatrixType >::GetChildren() const
{
    return mChildren;
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::CalculateCellCapacity()
{
    if ( !mElectricalTwoPort->IsCellelement() )
        return;

    auto cell = boost::static_pointer_cast< electrical::Cellelement< MatrixType > >( mElectricalTwoPort );

    if ( !cell->HasHalfcells() )
        return;

    auto voltageRange = cell->GetVoltageRange();
    double minVoltage = voltageRange.first;
    double maxVoltage = voltageRange.second;

    boost::shared_ptr< state::Soc > anodeSoc = cell->GetAnodeElements().front()->GetSoc();
    boost::shared_ptr< state::Soc > cathodeSoc = cell->GetCathodeElements().front()->GetSoc();
    boost::shared_ptr< state::Soc > cellSoc = cell->GetSoc();

    double storedChargeAnode = anodeSoc->GetValue< state::SocGetFormat::AS >();
    double storedChargeCathode = cathodeSoc->GetValue< state::SocGetFormat::AS >();
    double anodeChargeUntilMinimum = storedChargeAnode - anodeSoc->GetMinimumValue< state::SocGetFormat::AS >();
    double cathodeChargeUntilMinimum = storedChargeCathode - cathodeSoc->GetMinimumValue< state::SocGetFormat::AS >();

    // set the charge to the lowest possible value and then increase it until the minimum voltage is reached
    double chargeUntilMinimum = std::min( anodeChargeUntilMinimum, cathodeChargeUntilMinimum );
    anodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeUntilMinimum );
    cathodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeUntilMinimum );

    // Add charge to the electrodes until the minimum voltage is reached. Then revert charge to the last value
    // before the voltage was reached and repeat with a smaller charge step.
    double initialChargeStep = cellSoc->template GetInitialCapacity< state::SocGetFormat::AS >() / 100;
    double minimumChargeStep = 1;
    for ( double chargeStep = initialChargeStep; chargeStep >= minimumChargeStep; chargeStep /= 10 )
    {
        do
        {
            anodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( chargeStep );
            cathodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( chargeStep );
        } while ( cell->GetOcvVoltageValue() < minVoltage );
        anodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeStep );
        cathodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeStep );
    }
    double minAnodeCharge = anodeSoc->GetValue< state::SocGetFormat::AS >();

    // set the charge to the highest possible value and then decrease it until the maximum voltage is reached
    double anodeChargeUntilMaximum = anodeSoc->GetMaximumValue< state::SocGetFormat::AS >() - storedChargeAnode;
    double cathodeChargeUntilMaximum = cathodeSoc->GetMaximumValue< state::SocGetFormat::AS >() - storedChargeCathode;
    double chargeUntilMaximum = std::min( anodeChargeUntilMaximum, cathodeChargeUntilMaximum );
    anodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeAnode + chargeUntilMaximum );
    cathodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeCathode + chargeUntilMaximum );

    for ( double chargeStep = initialChargeStep; chargeStep >= minimumChargeStep; chargeStep /= 10 )
    {
        do
        {
            anodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeStep );
            cathodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( -chargeStep );
        } while ( cell->GetOcvVoltageValue() > maxVoltage );
        anodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( chargeStep );
        cathodeSoc->SetStoredEnergy< state::SocSetFormat::DELTA >( chargeStep );
    }
    double maxAnodeCharge = anodeSoc->GetValue< state::SocGetFormat::AS >();

    // restore electrode SOCs and system state
    anodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeAnode );
    cathodeSoc->SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeCathode );

    double cellCapacity = maxAnodeCharge - minAnodeCharge;
    double cellSocValue = storedChargeAnode - minAnodeCharge;
    double cellSocOffset = cellSocValue - cellSoc->GetValue< state::SocGetFormat::AS >();

    // finally set the new cell capacity
    // cellSoc->template SetStoredEnergy< state::SocSetFormat::ABSOLUT >( storedChargeAnode - minAnodeCharge );
    this->mAgingState->SetCapacityFactor( cellCapacity / cellSoc->GetInitialCapacity< state::SocGetFormat::AS >() );
    this->mAgingState->SetSocOffset( cellSocOffset, state::AgingStateSetFormat::DELTA );
}

template < typename MatrixType >
void AgingTwoPort< MatrixType >::SetAging( const double &capacityFactor, const double &resistanceFactor,
                                           const double &socOffset, const double &timestep )
{
    this->mAgingState->mCapacityFactor = capacityFactor;
    this->mAgingState->mResistanceFactor = resistanceFactor;
    this->mAgingState->mSocOffset = socOffset;
    ApplyAging( timestep, 1.0 );
}


}    // namespace aging

#endif
#endif