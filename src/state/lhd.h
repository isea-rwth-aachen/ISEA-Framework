/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lhd.h
* Creation Date : 08-07-2022
* Last Modified : We 20 Jul 2022 13:50 CET
* Created By : Jonas Rinner
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _LHD_
#define _LHD_

// STD
#include <vector>

// ETC
#include "../exceptions/error_proto.h"

#include "../state/crate.h"
#include "../state/soc.h"
#include "../state/thermal_state.h"
#include "dgl_state.h"

#include "../lookup/lookup.h"
#include "../lookup/lookupType.h"
//#include "../electrical/twoport_with_state.h"

#include "../electrical/electricalelement.h"
#include "../object/lookup_obj3d_with_state.h"
#include "../object/object.h"
//#include "../factory/object/objectclasswrapper.h"

#include <limits>
#include <math.h>
#include <vector>
#include <cfloat>
#include <cmath>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../misc/macros.h"
#include "../misc/matrixInclude.h"
#include "../xmlparser/xmlparameter.h"

// class TestCrate;
// class TestXML;

/*namespace electrical
{
template < typename T >
class TwoPort;
}*/

namespace state
{

template < typename T >
class LHDState : public State
{
    public:
    LHDState( boost::shared_ptr< object::LookupObj3dWithState< T > > &f1,
              boost::shared_ptr< object::LookupObj3dWithState< T > > &tau1,
              boost::shared_ptr< object::LookupObj3dWithState< T > > &f2,
              boost::shared_ptr< object::LookupObj3dWithState< T > > &tau2, double step );
    LHDState();
    virtual ~LHDState();

    T GetValue() const;
    T GetLastValue() const;
    void UpdateLHD( const T mSoc, const T mThermalState, const T mCrate );
    void SetValue( const double value );


    T mActualLHD;
    T mLastLHD;
    T mLastCrate;
    double mTimeStep;

    const boost::shared_ptr< object::LookupObj3dWithState< T > > Fraction1;
    const boost::shared_ptr< object::LookupObj3dWithState< T > > TimeConstant1;
    const boost::shared_ptr< object::LookupObj3dWithState< T > > Fraction2;
    const boost::shared_ptr< object::LookupObj3dWithState< T > > TimeConstant2;
};

// standard constructor
template < typename T >
LHDState< T >::LHDState( boost::shared_ptr< object::LookupObj3dWithState< T > > &f1,
                         boost::shared_ptr< object::LookupObj3dWithState< T > > &tau1,
                         boost::shared_ptr< object::LookupObj3dWithState< T > > &f2,
                         boost::shared_ptr< object::LookupObj3dWithState< T > > &tau2, double step )
    : mActualLHD( 0.0 )
    , mLastLHD( 0.0 )
    , mLastCrate( 0.0 )
    , Fraction1( f1 )
    , TimeConstant1( tau1 )
    , Fraction2( f2 )
    , TimeConstant2( tau2 )
    , mTimeStep( 1 / step ){

      };

// standard constructor
template < typename T >
LHDState< T >::LHDState()
    : mActualLHD( 0.0 )
    , mLastLHD( 0.0 )
    , mLastCrate( 0.0 )
    {};

template < typename T >
LHDState< T >::~LHDState(){};

template < typename T >
T LHDState< T >::GetValue() const
{
    return mActualLHD;
}

template < typename T >
T LHDState< T >::GetLastValue() const
{
    return mLastLHD;
}

template < typename T >
void LHDState< T >::SetValue( const double value )
{
    mLastLHD = mActualLHD;
    mActualLHD = value;
}

template < typename T >
void LHDState< T >::UpdateLHD( const T mSoc, const T mThermalState, const T mCrate )
{
    // save the last value temporarily
    double save_value = mActualLHD;
    double dFraction1;
    double dFraction2;
    double dTimeConstant1;
    double dTimeConstant2;
    // Caluclate last target value
    if ( mCrate <= mActualLHD && mCrate > 0 )
    {
        mLastCrate = mCrate;
    }
    else if ( mCrate >= mActualLHD && mCrate < 0 )
    {
        mLastCrate = mCrate;
    }

    // Calculate current LHD value
    if ( mCrate > mActualLHD )
    {
         dFraction1 = Fraction1->GetValue();
         dFraction2 = Fraction2->GetValue();
         dTimeConstant1 = TimeConstant1->GetValue();
         dTimeConstant2 = TimeConstant2->GetValue();

        dFraction1 = dFraction1 / ( dFraction1 + dFraction2 );
        dFraction2 = dFraction2 / ( dFraction1 + dFraction2 );
        double numericError = ( dFraction1 + dFraction2 ) - 1;
        dFraction1 = dFraction1 - numericError / 2;
        dFraction2 = dFraction2 - numericError / 2;

        mActualLHD =
         ( dFraction1 * exp( -1 * dTimeConstant1 * mTimeStep ) + dFraction2 * exp( -1 * dTimeConstant2 * mTimeStep ) ) * mActualLHD +
         mCrate * ( 1 - dFraction1 * exp( -1 * dTimeConstant1 * mTimeStep ) - dFraction2 * exp( -1 * dTimeConstant2 * mTimeStep ) );
        mLastLHD = save_value;
    }
    else if ( mCrate <= mActualLHD )
    {
         dFraction1 = Fraction1->GetValue();
         dFraction2 = Fraction2->GetValue();
         dTimeConstant1 = TimeConstant1->GetValue();
         dTimeConstant2 = TimeConstant2->GetValue();

        dFraction1 = dFraction1 / ( dFraction1 + dFraction2 );
        dFraction2 = dFraction2 / ( dFraction1 + dFraction2 );
        double numericError = ( dFraction1 + dFraction2 ) - 1;
        dFraction1 = dFraction1 - numericError / 2;
        dFraction2 = dFraction2 - numericError / 2;

        mActualLHD =
         ( dFraction1 * exp( -1 * dTimeConstant1 * mTimeStep ) + dFraction2 * exp( -1 * dTimeConstant2 * mTimeStep ) ) * mActualLHD +
         mCrate * ( 1 - dFraction1 * exp( -1 * dTimeConstant1 * mTimeStep ) - dFraction2 * exp( -1 * dTimeConstant2 * mTimeStep ) );
        mLastLHD = save_value;
    }
    else if ( mCrate == 0 && mActualLHD != 0 )
    {
         dFraction1 = Fraction1->GetLastValue();
         dFraction2 = Fraction2->GetLastValue();
         dTimeConstant1 = TimeConstant1->GetLastValue();
        double dTimeConstant2 = TimeConstant2->GetLastValue();       

        dFraction1 = dFraction1 / ( dFraction1 + dFraction2 );
        dFraction2 = dFraction2 / ( dFraction1 + dFraction2 );
        double numericError = ( dFraction1 + dFraction2 ) - 1;
        dFraction1 = dFraction1 - numericError / 2;
        dFraction2 = dFraction2 - numericError / 2;

        mActualLHD =
         ( dFraction1 * exp( -1 * dTimeConstant1 * mTimeStep ) + dFraction2 * exp( -1 * dTimeConstant2 * mTimeStep ) ) * mActualLHD;
    }

    double a = dFraction1;
    double b = dFraction2;
    double c = dTimeConstant1;
    double d = dTimeConstant2;

    if ( std::isnan( mActualLHD ) )
    {
        int a = 1;
    }
}

}    // namespace state

#endif