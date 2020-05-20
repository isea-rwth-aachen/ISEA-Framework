// aging_base.cpp
// Autor: Christopher Abele, Andreas Hase

#include "aging_base.h"

namespace aging
{
AgingBase::AgingBase( const double agingStepTime, const bool isEnabled )
    : mAgingStepTime( agingStepTime )
    , mIsEnabled( isEnabled )
{
}
}    // namespace aging
