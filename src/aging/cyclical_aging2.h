// cyclical_aging.h
// Autor: Christopher Abele, Andreas Hase

#ifndef _CYCLICAL_AGING2_
#define _CYCLICAL_AGING2_

#include "../electrical/cellelement.h"
#include "../state/soc.h"
#include "empirical_aging.h"

#include <algorithm>

class TestCyclicalAging;

namespace factory
{
template < typename T >
class AgingClassWrapper;
}

namespace aging
{

/// This class calculates the relative capacity loss and resistance increase due to the cyclical aging of the underlying
/// cell
class CyclicalAging2 : public EmpiricalAging
{
    friend factory::AgingClassWrapper< CyclicalAging2 >;
    friend TestCyclicalAging;

    public:
    /// Constructor
    CyclicalAging2( const double agingStepTime, const double minBetaCapacity, const double minBetaResistance, const double maxBetaCapacity,
                   const double maxBetaResistance, const boost::shared_ptr< object::Object< double > >& alphaCapacity,
                   const boost::shared_ptr< object::Object< double > >& alphaResistance,
                   const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled,
                   const double EFCExponentCapacity, const double EFCExponentResistance );

    /// Returns AgingType::CYCLICAL
    /// Derived from AgingBase
    virtual size_t GetType() const;

    /// Calculates the relative capacity loss and resistance increase
    /// Derived from AgingBase
    virtual void CalculateAging( const TwoportState& twoportState, double timestep, double scaleFactor );

    /// Collects the necessary data for the calculation of the relative capacity loss and resistance increase
    virtual void CollectData( const TwoportState& twoportState, const TwoportState& cellState, double timestep );

    virtual void ResetToPointInTime( double time );

    virtual void AcceptVisitor( visitor::Visitor< AgingBase >& visitor ) { visitor.Visit( *this ); }

    private:
    double GetAverageVoltage( double startTime, double endTime );

    const double mEFCExponentCapacity;
    const double mEFCExponentResistance;

    double mActualDod;                 // [%/100]
    double mActualVoltage;             // [V]
    double mActualSoc;                 // [%/100]
    double mActualCurrent;             // [A]
    double mTimeSinceLastAgingStep;    // [s]

    std::vector< double > mTimeValues;       // [s]
    std::vector< double > mSocValues;        // [%]
    std::vector< double > mVoltageValues;    // [V]
};
}    // namespace aging

#endif    // _CYCLICAL_AGING2_
