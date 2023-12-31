// cyclical_aging.h
// Autor: Christopher Abele, Andreas Hase

#ifndef _CYCLICAL_AGING_
#define _CYCLICAL_AGING_

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
class CyclicalAging : public EmpiricalAging
{
    friend factory::AgingClassWrapper< CyclicalAging >;
    friend TestCyclicalAging;

    public:
    /// Constructor
    CyclicalAging( const double agingStepTime, const double minBetaCapacity, const double minBetaResistance, const double maxBetaCapacity,
                   const double maxBetaResistance, const boost::shared_ptr< object::Object< double > >& alphaCapacity,
                   const boost::shared_ptr< object::Object< double > >& alphaResistance,
                   const double initialCapacityFactor, const double initialResistanceFactor, const bool isEnabled,
                   const double chargeThroughputExponentCapacity, const double chargeThroughputExponentResistance,
                   const std::string chargeCounting);

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
    double GetAverageValue(  double startTime, double endTime,  std::vector< double > valuesToAvg  );

    const double mChargeThroughputExponentCapacity;
    const double mChargeThroughputExponentResistance;

    double mActualDod;                 // [%]
    double mActualVoltage;             // [V]
    double mActualSoc;                 // [%]
    double mActualCurrent;             // [A]
    double mTimeSinceLastAgingStep;    // [s]
    double mActualTemperature;          // [K]

    std::vector< double > mTimeValues;       // [s]
    std::vector< double > mSocValues;        // [%]
    std::vector< double > mVoltageValues;    // [V]
    std::vector< double > mTemperatureValues;    // [V]
    const std::string mChargeCounting;
};
}    // namespace aging

#endif    // _CYCLICAL_AGING_
