#include "empirical_aging.h"
namespace aging
{
EmpiricalAging::EmpiricalAging( const double agingStepTime, const double minStressFactorCapacity,
                                const double minStressFactorResistance, const std::string& formulaStressFactorCapacity,
                                const std::string& formulaStressFactorResistance, const double initialCapacityFactor,
                                const double initialResistanceFactor, const bool isEnabled )
    : AgingBase( agingStepTime, isEnabled )
    , mCapacityFactor( initialCapacityFactor )
    , mResistanceFactor( initialResistanceFactor )
    , mChargeLoss( 0.0 )
    , mMinStressFactorCapacity( minStressFactorCapacity )
    , mMinStressFactorResistance( minStressFactorResistance )
    , mFormulaStressFactorCapacity( formulaStressFactorCapacity )
    , mFormulaStressFactorResistance( formulaStressFactorResistance )
    , mStressFactorCapacity( 0.0 )
    , mStressFactorResistance( 0.0 )
{
}

void EmpiricalAging::CalculateChargeLoss( const TwoportState& twoportState )
{
    this->mChargeLoss = ( 1 - this->mCapacityFactor ) *
                        twoportState.mSocState->template GetInitialCapacity< electrical::state::SocGetFormat::AS >() *
                        twoportState.mSocState->template GetValue< electrical::state::SocGetFormat::PERCENT >() / 100;
}
}    // namespace aging
