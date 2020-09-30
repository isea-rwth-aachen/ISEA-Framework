#ifndef _EMPIRICAL_AGING_
#define _EMPIRICAL_AGING_

#include "../electrical/cellelement.h"
#include "../state/soc.h"
#include "aging_base.h"

namespace aging
{

/// Base class for calendarian and cyclical aging
class EmpiricalAging : public AgingBase
{
    public:
    /// Constructor
    EmpiricalAging( const double agingStepTime, const double minStressFactorCapacity, const double minStressFactorResistance,
                    const boost::shared_ptr< object::Object< double > >& capacityStressFactor,
                    const boost::shared_ptr< object::Object< double > >& resistanceStressFactor,
                    const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled );

    virtual double GetCapacityFactor() { return mCapacityFactor; };
    virtual double GetSocOffset() { return -mChargeLoss; };    // in As
    virtual double GetResistanceFactor() { return mResistanceFactor; };

    virtual double GetStressFactorCapacity() { return mStressFactorCapacity; };
    virtual double GetStressFactorResistance() { return mStressFactorResistance; };

    protected:
    virtual void CalculateChargeLoss( const TwoportState& twoportState, double previousCapFactor );

    double mCapacityFactor;
    double mResistanceFactor;
    double mChargeLoss;

    const double mMinStressFactorCapacity;
    const double mMinStressFactorResistance;

    const std::string mFormulaStressFactorCapacity;
    const std::string mFormulaStressFactorResistance;

    double mStressFactorCapacity;
    double mStressFactorResistance;

    boost::shared_ptr< object::Object< double > > mCapacityStressFactor;
    boost::shared_ptr< object::Object< double > > mResistanceStressFactor;
};
}    // namespace aging

#endif    // _EMPIRICAL_AGING_
