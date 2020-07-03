/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : soc.cpp
* Creation Date : 01-10-2012
* Last Modified : Di 13 Okt 2015 16:56:19 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "soc.h"

namespace state
{


Soc::Soc( const double initialCapacity, const double actualCapacity, const double actualSoc, const double minimumValue,
          const double maximumValue )
    : State()
    , mInitialCapacity( initialCapacity * 3600 )
    , mActualCapacity( actualCapacity * 3600 )
    , mActualSoc( actualSoc / 100 )
    , mOffset( 0.0 )
    , mMinimumValue( minimumValue / 100 )
    , mMaximumValue( maximumValue / 100 )
    , mInitialValue( actualSoc / 100 )
{
}

double Soc::GetValue() const { return this->GetValue<>(); }

const double& Soc::GetValueRef() const { return this->mActualSoc; }

void Soc::ResetToInitialValue() { mActualSoc = mInitialValue; }

double Soc::GetCapacityFactor() const { return this->mActualCapacity / this->mInitialCapacity; }

}    // Namespace state
