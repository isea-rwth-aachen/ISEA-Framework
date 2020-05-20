#include "generic_aging.h"

namespace aging
{
GenericAging::GenericAging( const double agingStepTime, double capacityFactor, double socOffset, double resistanceFactor )
    : AgingBase( agingStepTime, true )
    , mCapacityFactor( capacityFactor )
    , mSocOffset( socOffset )
    , mResistanceFactor( resistanceFactor )
{
}

size_t GenericAging::GetType() const { return AgingType::GENERIC; }

void GenericAging::CalculateAging( const TwoportState&, double timestep, double scaleFactor ) {}

void GenericAging::CollectData( const TwoportState&, const TwoportState&, double ) {}

void GenericAging::ResetToPointInTime( double time ) {}

}    // namespace aging
