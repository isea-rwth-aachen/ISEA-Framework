#include "empirical_aging.h"
namespace aging
{
EmpiricalAging::EmpiricalAging( const double agingStepTime, const double minStressFactorCapacity, const double minStressFactorResistance,
                                const boost::shared_ptr< object::Object< double > >& capacityStressFactor,
                                const boost::shared_ptr< object::Object< double > >& resistanceStressFactor,
                                const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled )
    : AgingBase( agingStepTime, isEnabled )
    , mCapacityFactor( initialCapacityFactor )
    , mResistanceFactor( initialResistanceFactor )
    , mChargeLoss( 0.0 )
    , mMinStressFactorCapacity( minStressFactorCapacity )
    , mMinStressFactorResistance( minStressFactorResistance )
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
}    // namespace aging
