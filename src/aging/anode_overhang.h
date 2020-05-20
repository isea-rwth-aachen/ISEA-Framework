#ifndef _ANODE_OVERHANG_
#define _ANODE_OVERHANG_

#include "../electrical/capacity.h"
#include "../electrical/cellelement.h"
#include "../electrical/ohmicresistance.h"
#include "../states/soc.h"
#include "../states/thermal_state.h"
#include "aging_base.h"

#include <math.h>

namespace aging
{

class AnodeOverhang : public AgingBase
{
    public:
    /// Constructor
    AnodeOverhang( const double agingStepTime, const boost::shared_ptr< object::Object< double > >& voltage,
                   const double& activCoef, const double& tauCoef, const boost::shared_ptr< electrical::state::Soc >& socState,
                   const double socWhereOffsetIsZero, bool isEnabled = true );

    virtual size_t GetType() const;

    virtual void CalculateAging( const TwoportState& twoportState, double timestep, double scaleFactor );

    virtual void CollectData( const TwoportState& twoportState, const TwoportState& cellState, double timestep );

    virtual void ResetToPointInTime( double time );

    virtual const boost::shared_ptr< electrical::state::Soc >& GetSoc() const;

    virtual double GetVoltage() const { return mVoltage->GetValue(); }

    virtual double GetCapacityFactor() { return 1; };
    virtual double GetSocOffset() { return mAnodeOffset; };    // in As
    virtual double GetResistanceFactor() { return 1; };

    virtual void AcceptVisitor( visitor::Visitor< AgingBase >& visitor ) { visitor.VisitAnodeOverhang( *this ); }

    private:
    boost::shared_ptr< object::Object< double > > mVoltage;
    const double mActivCoef;
    const double mTauCoef;
    boost::shared_ptr< electrical::state::Soc > mSocState;

    std::vector< double > mTimeValues;            //[s]
    std::vector< double > mAnodeVoltageValues;    //[V]
    std::vector< double > mTemperatureValues;     //[K]

    double mAnodeOffset;
    double mTotalCalculationTime;
};
}    // namespace aging

#endif    // _CALENDARIAN_AGING_
