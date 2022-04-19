// calendarian_aging.h
// Autor: Christopher Abele, Andreas Hase

#ifndef _CALENDARIAN_AGING_
#define _CALENDARIAN_AGING_

#include "../electrical/cellelement.h"
#include "../state/thermal_state.h"
#include "empirical_aging.h"

class TestCalendarianAging;

namespace factory
{
template < typename T >
class AgingClassWrapper;
}

namespace aging
{

/// This class calculates the relative capacity loss and resistance increase due to the calendarian aging of the
/// underlying cell
class CalendarianAging : public EmpiricalAging
{
    friend factory::AgingClassWrapper< CalendarianAging >;
    friend TestCalendarianAging;

    public:
    /// Constructor
    CalendarianAging( const double agingStepTime, const double minAlphaCapacity, const double minAlphaResistance,
                      const double maxAlphaCapacity, const double maxAlphaResistance,
                      const boost::shared_ptr< object::Object< double > >& alphaCapacity,
                      const boost::shared_ptr< object::Object< double > >& alphaResistance, const double initialCapacityFactor,
                      const double initialResistanceFactor, const bool isEnabled, const double timeExponent );

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
    const double mTimeExponent;

    double mActualVoltage;             // [V]
    double mActualTemperature;         // [K]
    double mActualSoc;                 // [%/100]
    double mTimeSinceLastAgingStep;    // [s]
    std::vector< double > mTimeValues;
    std::vector< double > mVoltageValues;
    std::vector< double > mTemperatureValues;
    std::vector< double > mSocValues;
};

}    // namespace aging

#endif    // _CALENDARIAN_AGING_
