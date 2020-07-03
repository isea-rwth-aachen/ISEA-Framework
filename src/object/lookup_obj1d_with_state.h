/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lookup_obj1d_with_state.h
* Creation Date : 12-11-2012
* Last Modified : Mo 24 Nov 2014 18:19:20 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUP_OBJ1D_WI_STATE_
#define _LOOKUP_OBJ1D_WI_STATE_
// BOOST
#include <boost/shared_ptr.hpp>

#include "../state/dgl_state.h"
#include "lookup_obj1d.h"

/// This namespace contains all classes which define the behaviour of different classes
namespace object
{
/// LookupObj1dWithState is an object that contains a lookup and has access to a state which is evaluated for the lookup
template < typename T >
class LookupObj1dWithState : public LookupObj1D< T >
{
    public:
    LookupObj1dWithState( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                          boost::shared_ptr< state::State > state, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj1dWithState( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                          boost::shared_ptr< state::State > state, LookupObj1dWithState* CloneObj,
                          lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj1dWithState( const char* name, const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                          boost::shared_ptr< state::State > state, LookupObj1dWithState* CloneObj );
    virtual ~LookupObj1dWithState(){};
    virtual T GetValue() const;      ///< Return the last done lookup
    virtual T operator()() const;    ///< Return the last done lookup

    virtual T operator()( const T val );
    virtual T GetValue( const T val );

    const state::State* GetState() const { return mState.get(); }
    boost::shared_ptr< state::State > GetBoostState() const { return mState; }

    virtual const char* GetName() const;

    private:
    boost::shared_ptr< state::State > mState;

    protected:
};

template < typename T >
LookupObj1dWithState< T >::LookupObj1dWithState( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                                                 boost::shared_ptr< state::State > state, lookup::LookupType type )
    : LookupObj1D< T >( lookupData, measurementPoints, type )
    , mState( state )
{
}


template < typename T >
LookupObj1dWithState< T >::LookupObj1dWithState( const ::std::vector< T >& lookupData, const ::std::vector< T >& measurementPoints,
                                                 boost::shared_ptr< state::State > /* state */,
                                                 LookupObj1dWithState* CloneObj, lookup::LookupType type )
    : LookupObj1D< T >( lookupData, measurementPoints, CloneObj, type )
    , mState( CloneObj->mState )
{
}

template < typename T >
T LookupObj1dWithState< T >::GetValue() const
{
    return this->mLookup( mState->GetValue() ) * this->mFactor;
}

template < typename T >
T LookupObj1dWithState< T >::operator()() const
{
    return this->mLookup( mState->GetValue() ) * this->mFactor;
}

template < typename T >
T LookupObj1dWithState< T >::operator()( const T /* val */ )
{
    this->mLastValue = this->mLookup( mState->GetValue() ) * this->mFactor;
    return this->mLastValue;
}

template < typename T >
T LookupObj1dWithState< T >::GetValue( const T /* val */ )
{
    this->mLastValue = this->mLookup( mState->GetValue() ) * this->mFactor;
    return this->mLastValue;
}

template < typename T >
const char* LookupObj1dWithState< T >::GetName() const
{
    return "LookupObj1dWithState";
}
}    // namespace object
#endif /* _LOOKUP_OBJ1D_WI_STATE_ */
