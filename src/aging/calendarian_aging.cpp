// calendarian_aging.cpp
// Autor: Christopher Abele, Andreas Hase

#include "calendarian_aging.h"

namespace aging
{
CalendarianAging::CalendarianAging( const double agingStepTime, const double minAlphaCapacity, const double minAlphaResistance,
                                    const boost::shared_ptr< object::Object< double > >& alphaCapacity,
                                    const boost::shared_ptr< object::Object< double > >& alphaResistance,
                                    const double initialCapacityFactor, const double initialResistanceFactor,
                                    const bool isEnabled, const double timeExponent )
    : EmpiricalAging( agingStepTime, minAlphaCapacity, minAlphaResistance, alphaCapacity, alphaResistance,
                      initialCapacityFactor, initialResistanceFactor, isEnabled )
    , mTimeExponent( timeExponent )
    , mActualVoltage( 0.0 )
    , mActualTemperature( 0.0 )
    , mActualSoc( 0.0 )
    , mTimeSinceLastAgingStep( 0.0 )
{
}

size_t CalendarianAging::GetType() const { return AgingType::CALENDARIAN; }

void CalendarianAging::CalculateAging( const TwoportState& twoportState, double timestep, double scaleFactor )
{
    // Calculates aging only if calendarian aging is enabled for the underlying cell
    if ( this->mIsEnabled && this->mTimeSinceLastAgingStep > 0 && !mTimeValues.empty() )
    {
        double previousTime = 0.0;
        double alphaCapSum = 0.0;
        double alphaResSum = 0.0;
        double dt, alphaCap, alphaRes;
        for ( size_t i = 0; i < mTimeValues.size(); ++i )
        {
            mActualVoltage = mVoltageValues[i];
            mActualTemperature = mTemperatureValues[i];
            mActualSoc = mSocValues[i];
            dt = mTimeValues[i] - previousTime;
            alphaCap = this->mCapacityStressFactor->GetValue();
            alphaRes = this->mResistanceStressFactor->GetValue();
            if ( alphaCap < this->mMinStressFactorCapacity )
                alphaCap = this->mMinStressFactorCapacity;
            if ( alphaRes < this->mMinStressFactorResistance )
                alphaRes = this->mMinStressFactorResistance;
            alphaCapSum += alphaCap * dt;
            alphaResSum += alphaRes * dt;
            previousTime = mTimeValues[i];
        }
        // mAlphaCapacitySum = ( alphaCap1 * timeStep1 + alphaCap2 * timeStep2 ... ) / totalCalculationTime
        this->mStressFactorCapacity = alphaCapSum / timestep;
        this->mStressFactorResistance = alphaResSum / timestep;

        double timestepDays = timestep * scaleFactor / 86400;

        this->mCapacityFactor = 1 - pow( pow( 1 - this->mCapacityFactor, 1 / this->mTimeExponent ) +
                                          ( timestepDays * pow( this->mStressFactorCapacity, 1 / this->mTimeExponent ) ),
                                         this->mTimeExponent );

        this->mResistanceFactor = 1 + pow( pow( this->mResistanceFactor - 1, 1 / this->mTimeExponent ) +
                                            ( timestepDays * pow( this->mStressFactorResistance, 1 / this->mTimeExponent ) ),
                                           this->mTimeExponent );

        this->CalculateChargeLoss( twoportState );

        mTimeValues.clear();
        mVoltageValues.clear();
        mTemperatureValues.clear();
        mSocValues.clear();
        this->mTimeSinceLastAgingStep = 0.0;
    }
}

void CalendarianAging::CollectData( const TwoportState& twoportState, const TwoportState& cellState, double timestep )
{
    // Calculates aging only if calendarian aging is enabled for the underlying cell
    if ( this->mIsEnabled )
    {
        // Avoids useless calculation
        if ( timestep == 0 )
            return;

        mTimeSinceLastAgingStep += timestep;
        mTimeValues.push_back( mTimeSinceLastAgingStep );
        mVoltageValues.push_back( cellState.mElectricalData->mVoltageValue );
        mTemperatureValues.push_back( twoportState.mThermalState->GetValue< TemperatureGetFormat::KELVIN >() );
        mSocValues.push_back( twoportState.mSocState->GetValue< state::SocGetFormat::PERCENT >() / 100 );
    }
}

void CalendarianAging::ResetToPointInTime( double time )
{
    auto firstDelete = std::upper_bound( mTimeValues.begin(), mTimeValues.end(), time );
    std::ptrdiff_t firstDeleteIndex = firstDelete - mTimeValues.begin();
    mTimeValues.erase( firstDelete, mTimeValues.end() );
    mVoltageValues.erase( mVoltageValues.begin() + firstDeleteIndex, mVoltageValues.end() );
    mTemperatureValues.erase( mTemperatureValues.begin() + firstDeleteIndex, mTemperatureValues.end() );
    mSocValues.erase( mSocValues.begin() + firstDeleteIndex, mSocValues.end() );

    mTimeSinceLastAgingStep = time;
}

}    // namespace aging
