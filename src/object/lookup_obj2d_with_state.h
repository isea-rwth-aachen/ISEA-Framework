/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lookup_obj2d_with_state.h
* Creation Date : 12-11-2012
* Last Modified : Mo 24 Nov 2014 18:22:15 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUP_OBJ2D_WI_STATE_
#define _LOOKUP_OBJ2D_WI_STATE_

// BOOST
#include "../state/dgl_state.h"
#include "lookup_obj2d.h"
#include <boost/shared_ptr.hpp>

namespace object
{

/// Lookup state object  that requsts the current Value of the state to use it for the lookup
template < typename T >
class LookupObj2dWithState : public LookupObj2D< T >
{

    public:
    LookupObj2dWithState( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                          const std::vector< T >& measurementPointsColumn, boost::shared_ptr< state::State > stateRow,
                          boost::shared_ptr< state::State > stateCol, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj2dWithState( const std::vector< std::vector< T > >& lookupData,
                          const std::vector< T >& measurementPointsRow, const std::vector< T >& measurementPointsColumn,
                          boost::shared_ptr< state::State > stateRow, boost::shared_ptr< state::State > stateCol,
                          LookupObj2dWithState< T >* CloneObj, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );

    virtual ~LookupObj2dWithState(){};

    virtual T GetValue() const;      ///< Return the last done lookup
    virtual T operator()() const;    ///< Return the last done lookup

    virtual T GetValue( const T val1, const T val2 );
    virtual T operator()( const T val, const T val2 );

    const state::State* GetStateRow() const { return mStateRow.get(); }
    const state::State* GetStateCol() const { return mStateCol.get(); }

    boost::shared_ptr< state::State > GetBoostStateRow() const { return mStateRow; }
    boost::shared_ptr< state::State > GetBoostStateCol() const { return mStateCol; }


    virtual const char* GetName() const;

    private:
    boost::shared_ptr< state::State > mStateRow;
    boost::shared_ptr< state::State > mStateCol;

    protected:
};

template < typename T >
LookupObj2dWithState< T >::LookupObj2dWithState( const std::vector< std::vector< T > >& lookupData,
                                                 const std::vector< T >& measurementPointsRow,
                                                 const std::vector< T >& measurementPointsColumn,
                                                 boost::shared_ptr< state::State > stateRow,
                                                 boost::shared_ptr< state::State > stateCol, lookup::LookupType type )
    : LookupObj2D< T >( lookupData, measurementPointsRow, measurementPointsColumn, type )
    , mStateRow( stateRow )
    , mStateCol( stateCol )
{
}

template < typename T >
LookupObj2dWithState< T >::LookupObj2dWithState( const std::vector< std::vector< T > >& lookupData,
                                                 const std::vector< T >& measurementPointsRow,
                                                 const std::vector< T >& measurementPointsColumn,
                                                 boost::shared_ptr< state::State > /* stateRow */,
                                                 boost::shared_ptr< state::State > /* stateCol */,
                                                 LookupObj2dWithState< T >* CloneObj, lookup::LookupType type )
    : LookupObj2D< T >( lookupData, measurementPointsRow, measurementPointsColumn, CloneObj, type )
    , mStateRow( CloneObj->mStateRow )
    , mStateCol( CloneObj->mStateCol )
{
}

template < typename T >
T LookupObj2dWithState< T >::GetValue() const
{
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue() ) * this->mFactor;
}

template < typename T >
T LookupObj2dWithState< T >::operator()() const
{
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue() ) * this->mFactor;
}

template < typename T >
T LookupObj2dWithState< T >::GetValue( const T /* val1 */, const T /* val2 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue() ) * this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue() ) * this->mFactor;
}

template < typename T >
T LookupObj2dWithState< T >::operator()( const T /* val */, const T /* val2 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue() ) * this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue() ) * this->mFactor;
};

template < typename T >
const char* LookupObj2dWithState< T >::GetName() const
{
    return "LookupObj2dWithState";
}

}    // namespace object
#endif /* _LOOKUP_OBJ2D_WI_STATE_ */
