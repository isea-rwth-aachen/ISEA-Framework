#include "cyclical_aging.h"
#include <fstream>
#include <iostream>

namespace aging
{
CyclicalAging::CyclicalAging( const double agingStepTime, const double minBetaCapacity, const double minBetaResistance,
                              const std::string& formulaBetaCapacity, const std::string& formulaBetaResistance,
                              const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled,
                              const double chargeThroughputExponentCapacity, const double chargeThroughputExponentResistance )
    : EmpiricalAging( agingStepTime, minBetaCapacity, minBetaResistance, formulaBetaCapacity, formulaBetaResistance,
                      initialCapacityFactor, initialResistanceFactor, isEnabled )
    , mChargeThroughputExponentCapacity( chargeThroughputExponentCapacity )
    , mChargeThroughputExponentResistance( chargeThroughputExponentResistance )
    , mActualDod( 0.0 )
    , mActualVoltage( 0.0 )
    , mActualSoc( 0.0 )
    , mTimeSinceLastAgingStep( 0.0 )
{
    std::vector< std::pair< const char*, double& > > expressionParserContent;
    expressionParserContent.push_back( std::pair< const char*, double& >( "deltaDOD", mActualDod ) );
    expressionParserContent.push_back( std::pair< const char*, double& >( "meanV", mActualVoltage ) );
    expressionParserContent.push_back( std::pair< const char*, double& >( "meanSOC", mActualSoc ) );

    this->mExpressionParserStressFactorCapacity.AddContent( formulaBetaCapacity, expressionParserContent );
    this->mExpressionParserStressFactorResistance.AddContent( formulaBetaResistance, expressionParserContent );
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

    for ( size_t i = 0; i < cycles.size(); i++ )
    {
        mActualDod = cycles[i].mDepth;
        mActualSoc = cycles[i].mMeanValue;
        mActualVoltage = GetAverageVoltage( this->mTimeValues[cycles[i].mStartIndex], this->mTimeValues[cycles[i].mEndIndex] );
        double betaCapacity = this->mExpressionParserStressFactorCapacity.GetSolution();
        double betaResistance = this->mExpressionParserStressFactorResistance.GetSolution();
        if ( betaCapacity < this->mMinStressFactorCapacity )
            betaCapacity = this->mMinStressFactorCapacity;
        if ( betaResistance < this->mMinStressFactorResistance )
            betaResistance = this->mMinStressFactorResistance;
        double cycleChargeThroughput = mActualDod * capacity * cycles[i].mCycleCount;
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

        this->mCapacityFactor =
         1 - pow( pow( 1 - this->mCapacityFactor, 1 / this->mChargeThroughputExponentCapacity ) +
                   ( additionalChargeThroughput * pow( this->mStressFactorCapacity, 1 / this->mChargeThroughputExponentCapacity ) ),
                  this->mChargeThroughputExponentCapacity );

        this->mResistanceFactor =
         1 + pow( pow( this->mResistanceFactor - 1, 1 / this->mChargeThroughputExponentResistance ) +
                   ( additionalChargeThroughput * pow( this->mStressFactorResistance, 1 / this->mChargeThroughputExponentResistance ) ),
                  this->mChargeThroughputExponentResistance );

        this->CalculateChargeLoss( twoportState );
    }

    this->mTimeValues.clear();
    this->mVoltageValues.clear();
    this->mSocValues.clear();
    this->mTimeSinceLastAgingStep = 0.0;
}

void CyclicalAging::CollectData( const TwoportState& twoportState, const TwoportState& cellState, double timestep )
{
    // Needed for for the calculation of the total charge throughput
    mTimeSinceLastAgingStep += timestep;
    mTimeValues.push_back( this->mTimeSinceLastAgingStep );
    mSocValues.push_back( twoportState.mSocState->GetValue() / 100 );
    mVoltageValues.push_back( cellState.mElectricalData->mVoltageValue );
}

void CyclicalAging::ResetToPointInTime( double time )
{
    auto firstDelete = std::upper_bound( mTimeValues.begin(), mTimeValues.end(), time );
    std::ptrdiff_t firstDeleteIndex = firstDelete - mTimeValues.begin();
    mTimeValues.erase( firstDelete, mTimeValues.end() );
    mVoltageValues.erase( mVoltageValues.begin() + firstDeleteIndex, mVoltageValues.end() );
    mSocValues.erase( mSocValues.begin() + firstDeleteIndex, mSocValues.end() );

    mTimeSinceLastAgingStep = time;
}

double CyclicalAging::GetAverageVoltage( double startTime, double endTime )
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
