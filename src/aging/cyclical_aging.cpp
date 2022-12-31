#include <fstream>
#include <iostream>
#include <numeric>
#include "cyclical_aging.h"
#include "../misc/rainflow.h"

namespace aging
{
CyclicalAging::CyclicalAging( const double agingStepTime, const double minBetaCapacity, const double minBetaResistance,
                              const double maxBetaCapacity, const double maxBetaResistance,
                              const boost::shared_ptr< object::Object< double > >& alphaCapacity,
                              const boost::shared_ptr< object::Object< double > >& alphaResistance,
                              const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled,
                              const double chargeThroughputExponentCapacity, const double chargeThroughputExponentResistance,  
                              const std::string chargeCounting)
    : EmpiricalAging( agingStepTime, minBetaCapacity, minBetaResistance, maxBetaCapacity, maxBetaResistance,
                      alphaCapacity, alphaResistance, initialCapacityFactor, initialResistanceFactor, isEnabled )
    , mChargeThroughputExponentCapacity( chargeThroughputExponentCapacity )
    , mChargeThroughputExponentResistance( chargeThroughputExponentResistance )
    , mChargeCounting( chargeCounting )
    , mActualDod( 0.0 )
    , mActualVoltage( 0.0 )
    , mActualSoc( 0.0 )
    , mActualCurrent( 0.0 )
    , mTimeSinceLastAgingStep( 0.0 )
    , mActualTemperature( 0.0 )
{
}

size_t CyclicalAging::GetType() const { return AgingType::CYCLICAL; }

void CyclicalAging::CalculateAging( const TwoportState& twoportState, double timestep, double scaleFactor )
{
    double additionalChargeThroughput;

    if ( !this->mIsEnabled || mSocValues.size() < 2 || this->mTimeSinceLastAgingStep == 0 )
        return;

    std::vector< rainflow::Cycle > cycles = rainflow::countCycles( this->mSocValues );
    double capacity = twoportState.mSocState->template GetActualCapacity< state::SocGetFormat::AH >();
    double chargeThroughput = 0;    // in Ah
    this->mStressFactorCapacity = 0.0;
    this->mStressFactorResistance = 0.0;

    for ( const rainflow::Cycle& cycle : cycles )
    {
        double cycleStart = this->mTimeValues[cycle.mStartIndex];
        double cycleEnd = this->mTimeValues[cycle.mEndIndex];
        double cycleChargeThroughput = 0;
        if ( this->mChargeCounting == "EFC" )
        {
            cycleChargeThroughput = cycle.mDepth / 100 * cycle.mCycleCount;
        }    
        else if ( this->mChargeCounting == "Ah" )
        {
            cycleChargeThroughput = cycle.mDepth / 100 * capacity * cycle.mCycleCount;
        }

       
        mActualDod = cycle.mDepth;
        mActualSoc = cycle.mMeanValue;
        // convert CTP to As and double it to get the integral over the absolute value of the current
        mActualCurrent = ( cycleChargeThroughput * 2 * 3600 ) / ( cycleEnd - cycleStart );
        mActualVoltage = GetAverageValue( cycleStart, cycleEnd, mVoltageValues );
        mActualTemperature = GetAverageValue( cycleStart, cycleEnd, mTemperatureValues );
        double betaCapacity = this->mCapacityStressFactor->GetValue();
        double betaResistance = this->mResistanceStressFactor->GetValue();
        betaCapacity = clamp( betaCapacity, this->mMinStressFactorCapacity, this->mMaxStressFactorCapacity );
        betaResistance = clamp( betaResistance, this->mMinStressFactorResistance, this->mMaxStressFactorResistance );
        if ( betaCapacity < this->mMinStressFactorCapacity )
            betaCapacity = this->mMinStressFactorCapacity;
        if ( betaResistance < this->mMinStressFactorResistance )
            betaResistance = this->mMinStressFactorResistance;
        this->mStressFactorCapacity += betaCapacity * cycleChargeThroughput;
        this->mStressFactorResistance += betaResistance * cycleChargeThroughput;
        chargeThroughput += cycleChargeThroughput;
    }
    if ( chargeThroughput > 0 )
    {
        this->mStressFactorCapacity /= chargeThroughput;
        this->mStressFactorResistance /= chargeThroughput;

        // The current profile is only simulated once. The chargethroughput is then assumed to be the same for the
        // rest of the aging step.
        additionalChargeThroughput = chargeThroughput * scaleFactor;

        double previousCapFactor = this->mCapacityFactor;
        this->mCapacityFactor =
         1 - pow( pow( 1 - this->mCapacityFactor, 1 / this->mChargeThroughputExponentCapacity ) +
                   ( additionalChargeThroughput * pow( this->mStressFactorCapacity, 1 / this->mChargeThroughputExponentCapacity ) ),
                  this->mChargeThroughputExponentCapacity );

        this->mResistanceFactor =
         1 + pow( pow( this->mResistanceFactor - 1, 1 / this->mChargeThroughputExponentResistance ) +
                   ( additionalChargeThroughput * pow( this->mStressFactorResistance, 1 / this->mChargeThroughputExponentResistance ) ),
                  this->mChargeThroughputExponentResistance );

        this->CalculateChargeLoss( twoportState, previousCapFactor );
    }

    this->mTimeValues.clear();
    this->mVoltageValues.clear();
    this->mSocValues.clear();
    this->mTemperatureValues.clear();
    this->mTimeSinceLastAgingStep = 0.0;
}

void CyclicalAging::CollectData( const TwoportState& twoportState, const TwoportState& cellState, double timestep )
{
    // Needed for for the calculation of the total charge throughput
    mTimeSinceLastAgingStep += timestep;
    mTimeValues.push_back( this->mTimeSinceLastAgingStep );
    mSocValues.push_back( twoportState.mSocState->GetValue< state::SocGetFormat::PERCENT >());
    mVoltageValues.push_back( cellState.mElectricalData->mVoltageValue );
    mTemperatureValues.push_back( cellState.mThermalState->GetValue< TemperatureGetFormat::KELVIN >() );
}

void CyclicalAging::ResetToPointInTime( double time )
{
    auto firstDelete = std::upper_bound( mTimeValues.begin(), mTimeValues.end(), time );
    std::ptrdiff_t firstDeleteIndex = firstDelete - mTimeValues.begin();
    mTimeValues.erase( firstDelete, mTimeValues.end() );
    mVoltageValues.erase( mVoltageValues.begin() + firstDeleteIndex, mVoltageValues.end() );
    mSocValues.erase( mSocValues.begin() + firstDeleteIndex, mSocValues.end() );
    mTemperatureValues.erase( mTemperatureValues.begin() + firstDeleteIndex, mTemperatureValues.end() );

    mTimeSinceLastAgingStep = time;
}

double CyclicalAging::GetAverageValue( double startTime, double endTime,  std::vector< double > valuesToAvg )
{
    std::vector< double >::iterator startIterator = std::lower_bound( mTimeValues.begin(), mTimeValues.end(), startTime );
    std::vector< double >::iterator endIterator = std::upper_bound( mTimeValues.begin(), mTimeValues.end(), endTime );
    size_t startIndex = startIterator - mTimeValues.begin();
    size_t endIndex = endIterator - mTimeValues.begin();
    double valueSum = std::accumulate< std::vector< double >::iterator, double >( valuesToAvg.begin() + startIndex,
                                                                                    valuesToAvg.begin() + endIndex, 0.0 );
    return valueSum / ( endIndex - startIndex );
}
}    // namespace aging
