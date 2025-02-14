/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : surface_soc.cpp
* Creation Date : 30-06-2014
* Last Modified : Mo 07 Jul 2014 11:53:31 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

// STD
#include <iostream>

// ETC
#include "../electrical/twoport.h"
#include "surface_soc.h"

#include "../misc/macros.h"


namespace state
{

SurfaceSoc::SurfaceSoc()
    : mCurrentValue( 0 )
    , mLastValue(0)
{
}

void SurfaceSoc::SetElementsForLookup( std::vector< electrical::TwoPort< myMatrixType >* > vec ) { mVecElements = vec; }

void SurfaceSoc::UpdateLookUp()
{
    double ocvValue( 0 );
#ifndef _SYMBOLIC_
    for ( size_t i = 0; i < mVecElements.size(); ++i )
        ocvValue += mVecElements[i]->GetVoltageValue();
#endif
    mLastValue = mCurrentValue;
    mCurrentValue = mReverseLookup->GetValue( ocvValue );
}

void SurfaceSoc::SetReverseLookup( boost::shared_ptr< object::Object< double > > reverseLookup )
{
    mReverseLookup = reverseLookup;
}

double SurfaceSoc::GetValue() const { return mCurrentValue; }

double SurfaceSoc::GetLastValue() const { return mLastValue; }

}    // Namespace state
