#include "aging_lam.h"
#include "../misc/rainflow.h"

#include <numeric>

namespace aging
{

AgingLAM::AgingLAM( const double agingStepTime, const bool isEnabled, const boost::shared_ptr< object::Object< double > > &value )
    : AgingBase( agingStepTime, isEnabled )
    , mValue( value )
    , mTotalLam( 0.0 )
    , mCycles( 0.0 )
{
}

size_t AgingLAM::GetType() const { return AgingType::LAM; }

void AgingLAM::CalculateAging( const TwoportState &twoportState, double, double scaleFactor )
{
    mCycles = twoportState.mChargeThroughput / twoportState.mSocState->GetInitialCapacity< state::SocGetFormat::AS >();
    mTotalLam = mValue->GetValue();
}

void AgingLAM::CollectData( const TwoportState &twoportState, const TwoportState &, double ) {}

void AgingLAM::ResetToPointInTime( double ) {}
double AgingLAM::GetCapacityFactor() { return 1 - mTotalLam; }
double AgingLAM::GetSocOffset() { return 0; }
double AgingLAM::GetResistanceFactor() { return 0; }

void AgingLAM::AcceptVisitor( visitor::Visitor< AgingBase > &visitor ) { visitor.Visit( *this ); }

}    // namespace aging