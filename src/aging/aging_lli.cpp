#include "aging_lli.h"
#include "../misc/rainflow.h"

#include <numeric>

namespace aging
{

AgingLLI::AgingLLI( const double agingStepTime, const bool isEnabled, const boost::shared_ptr< object::Object< double > > &value )
    : AgingBase( agingStepTime, isEnabled )
    , mValue( value )
    , mTotalLli( 0.0 )
    , mCycles( 0.0 )
{
}

size_t AgingLLI::GetType() const { return AgingType::LLI; }

void AgingLLI::CalculateAging( const TwoportState &twoportState, double, double scaleFactor )
{
    double initialCapacity = twoportState.mSocState->GetInitialCapacity< state::SocGetFormat::AS >();
    mCycles = twoportState.mChargeThroughput / initialCapacity;
    mTotalLli = mValue->GetValue() * initialCapacity;
}

void AgingLLI::CollectData( const TwoportState &twoportState, const TwoportState &, double ) {}

void AgingLLI::ResetToPointInTime( double ) {}
double AgingLLI::GetCapacityFactor() { return 1; }
double AgingLLI::GetSocOffset() { return -mTotalLli; }    // in As
double AgingLLI::GetResistanceFactor() { return 0; }

void AgingLLI::AcceptVisitor( visitor::Visitor< AgingBase > &visitor ) { visitor.Visit( *this ); }

}    // namespace aging