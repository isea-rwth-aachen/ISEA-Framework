#include "empirical_aging.h"
namespace aging
{
EmpiricalAging::EmpiricalAging( const double agingStepTime, const double minStressFactorCapacity, const double minStressFactorResistance,
                                const double maxStressFactorCapacity, const double maxStressFactorResistance,
                                const boost::shared_ptr< object::Object< double > >& capacityStressFactor,
                                const boost::shared_ptr< object::Object< double > >& resistanceStressFactor,
                                const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled )
    : AgingBase( agingStepTime, isEnabled )
    , mCapacityFactor( initialCapacityFactor )
    , mResistanceFactor( initialResistanceFactor )
    , mChargeLoss( 0.0 )
    , mMinStressFactorCapacity( minStressFactorCapacity )
    , mMinStressFactorResistance( minStressFactorResistance )
    , mMaxStressFactorCapacity( maxStressFactorCapacity )
    , mMaxStressFactorResistance( maxStressFactorResistance )
    , mStressFactorCapacity( 0.0 )
    , mStressFactorResistance( 0.0 )
    , mCapacityStressFactor( capacityStressFactor )
    , mResistanceStressFactor( resistanceStressFactor )
{
}

void EmpiricalAging::CalculateChargeLoss( const TwoportState& twoportState, double previousCapFactor )
{
    this->mChargeLoss += ( previousCapFactor - this->mCapacityFactor ) *
                         twoportState.mSocState->template GetInitialCapacity< state::SocGetFormat::AS >() *
                         twoportState.mSocState->template GetValue< state::SocGetFormat::PERCENT >() / 100;
}

double EmpiricalAging::clamp( double value, double min, double max )
{
    if ( value < min )
        return min;
    else if ( value > max )
        return max;
    return value;
}


}    // namespace aging
