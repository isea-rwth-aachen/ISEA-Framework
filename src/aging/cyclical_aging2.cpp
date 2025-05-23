#include <fstream>
#include <iostream>
#include <numeric>
#include "cyclical_aging2.h"
#include "../misc/rainflow.h"

namespace aging
{
CyclicalAging2::CyclicalAging2( const double agingStepTime, const double minBetaCapacity, const double minBetaResistance,
                              const double maxBetaCapacity, const double maxBetaResistance,
                              const boost::shared_ptr< object::Object< double > >& alphaCapacity,
                              const boost::shared_ptr< object::Object< double > >& alphaResistance,
                              const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled,
                              const double EFCExponentCapacity, const double EFCExponentResistance )
    : EmpiricalAging( agingStepTime, minBetaCapacity, minBetaResistance, maxBetaCapacity, maxBetaResistance,
                      alphaCapacity, alphaResistance, initialCapacityFactor, initialResistanceFactor, isEnabled )
    , mEFCExponentCapacity( EFCExponentCapacity )
    , mEFCExponentResistance( EFCExponentResistance )
    , mActualDod( 0.0 )
    , mActualVoltage( 0.0 )
    , mActualSoc( 0.0 )
    , mActualCurrent( 0.0 )
    , mTimeSinceLastAgingStep( 0.0 )
{
}

size_t CyclicalAging2::GetType() const { return AgingType::CYCLICAL; }

void CyclicalAging2::CalculateAging( const TwoportState& twoportState, double timestep, double scaleFactor )
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
        double cycleChargeThroughput = cycle.mDepth * capacity * cycle.mCycleCount;

        mActualDod = cycle.mDepth;
        mActualSoc = cycle.mMeanValue;
        // convert CTP to As and double it to get the integral over the absolute value of the current
        mActualCurrent = ( cycleChargeThroughput * 2 * 3600 ) / ( cycleEnd - cycleStart );
        mActualVoltage = GetAverageVoltage( cycleStart, cycleEnd );
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
         1 - pow( pow( 100 - this->mCapacityFactor, 1 / this->mEFCExponentCapacity ) +
                   ( ( additionalChargeThroughput / 2 * capacity ) * pow( this->mStressFactorCapacity, 1 / this->mEFCExponentCapacity ) ),
                  this->mEFCExponentCapacity ) / 100;

        this->mResistanceFactor =
         1 + pow( pow( this->mResistanceFactor - 100, 1 / this->mEFCExponentResistance ) +
                    ( ( additionalChargeThroughput / 2 * capacity ) * pow( this->mStressFactorResistance, 1 / this->mEFCExponentResistance ) ),
                  this->mEFCExponentResistance ) / 100;

        this->CalculateChargeLoss( twoportState, previousCapFactor );
    }

    this->mTimeValues.clear();
    this->mVoltageValues.clear();
    this->mSocValues.clear();
    this->mTimeSinceLastAgingStep = 0.0;
}

void CyclicalAging2::CollectData( const TwoportState& twoportState, const TwoportState& cellState, double timestep )
{
    // Needed for for the calculation of the total charge throughput
    mTimeSinceLastAgingStep += timestep;
    mTimeValues.push_back( this->mTimeSinceLastAgingStep );
    mSocValues.push_back( twoportState.mSocState->GetValue() / 100 );
    mVoltageValues.push_back( cellState.mElectricalData->mVoltageValue );
}

void CyclicalAging2::ResetToPointInTime( double time )
{
    auto firstDelete = std::upper_bound( mTimeValues.begin(), mTimeValues.end(), time );
    std::ptrdiff_t firstDeleteIndex = firstDelete - mTimeValues.begin();
    mTimeValues.erase( firstDelete, mTimeValues.end() );
    mVoltageValues.erase( mVoltageValues.begin() + firstDeleteIndex, mVoltageValues.end() );
    mSocValues.erase( mSocValues.begin() + firstDeleteIndex, mSocValues.end() );

    mTimeSinceLastAgingStep = time;
}

double CyclicalAging2::GetAverageVoltage( double startTime, double endTime )
{
    std::vector< double >::iterator startIterator = std::lower_bound( mTimeValues.begin(), mTimeValues.end(), startTime );
    std::vector< double >::iterator endIterator = std::upper_bound( mTimeValues.begin(), mTimeValues.end(), endTime );
    size_t startIndex = startIterator - mTimeValues.begin();
    size_t endIndex = endIterator - mTimeValues.begin();
    double voltageSum = std::accumulate< std::vector< double >::iterator, double >( mVoltageValues.begin() + startIndex,
                                                                                    mVoltageValues.begin() + endIndex, 0.0 );
    return voltageSum / ( endIndex - startIndex );
}
}    // namespace aging
