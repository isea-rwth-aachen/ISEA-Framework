#include "aging_state.h"

namespace state
{

AgingState::AgingState( double cellAge, double chargeThroughput )
    : mCapacityFactor( 1 )
    , mSocOffset( 0 )
    , mResistanceFactor( 1 )
    , mCellAge( cellAge )
    , mChargeThroughput( chargeThroughput )
{
}

double AgingState::GetCellAge() const { return mCellAge; }

double AgingState::GetChargeThroughput() const { return mChargeThroughput; }

double AgingState::GetCapacityFactor() const { return mCapacityFactor; }

double AgingState::GetLastCapacityFactor() const { return mLastCapacityFactor; }

double AgingState::GetSocOffset() const { return mSocOffset; }

double AgingState::GetResistanceFactor() const { return mResistanceFactor; }

double AgingState::GetLastResistanceFactor() const { return mLastResistanceFactor; }

void AgingState::ChangeInternalValue( double &value, const double &newValue, AgingStateSetFormat format )
{
    switch ( format )
    {
        case AgingStateSetFormat::ABSOLUT:
            value = newValue;
            break;
        case AgingStateSetFormat::DELTA:
            value += newValue;
            break;
        case AgingStateSetFormat::FACTOR:
            value *= newValue;
    }
}

double AgingState::GetValue() const { return this->GetCapacityFactor(); }

double AgingState::GetLastValue() const { return this->GetLastCapacityFactor(); }

void AgingState::SetCellAge( const double &value, AgingStateSetFormat format )
{
    ChangeInternalValue( mCellAge, value, format );
}

void AgingState::SetChargeThroughput( const double &value, AgingStateSetFormat format )
{
    ChangeInternalValue( mChargeThroughput, value, format );
}

void AgingState::SetCapacityFactor( const double &value, AgingStateSetFormat format )
{
    ChangeInternalValue( mCapacityFactor, value, format );
}

void AgingState::SetSocOffset( const double &value, AgingStateSetFormat format )
{
    ChangeInternalValue( mSocOffset, value, format );
}

void AgingState::SetResistanceFactor( const double &value, AgingStateSetFormat format )
{
    ChangeInternalValue( mResistanceFactor, value, format );
}

}    // namespace state
