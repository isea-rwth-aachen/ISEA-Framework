#ifndef _GENERIC_AGING__
#define _GENERIC_AGING__

#include "../electrical/cellelement.h"
#include "../states/thermal_state.h"
#include "../visitor/visitor.h"
#include "aging_base.h"

namespace aging
{

/// This values of this class can be set freely to impose any kind of aging on the cell, e. g. for unittests
class GenericAging : public AgingBase
{
    public:
    /// Constructor
    GenericAging( const double agingStepTime, double capacityFactor = 1, double socOffset = 0, double resistanceFactor = 1 );

    virtual size_t GetType() const;

    virtual void CalculateAging( const TwoportState& twoportState, double timestep, double scaleFactor );

    virtual void CollectData( const TwoportState& twoportState, const TwoportState& cellState, const double timestep );

    virtual void ResetToPointInTime( double time );

    virtual double GetCapacityFactor() { return mCapacityFactor; };
    virtual double GetSocOffset() { return mSocOffset; };    // in As
    virtual double GetResistanceFactor() { return mResistanceFactor; };

    virtual void SetCapacityFactor( double value ) { mCapacityFactor = value; };
    virtual void SetSocOffset( double value ) { mSocOffset = value; };    // in As
    virtual void SetResistanceFactor( double value ) { mResistanceFactor = value; };

    virtual void AcceptVisitor( visitor::Visitor< AgingBase >& visitor ) { visitor.VisitGenericAging( *this ); }

    private:
    double mCapacityFactor;
    double mSocOffset;
    double mResistanceFactor;
};
}    // namespace aging

#endif    // _GENERIC_AGING__
