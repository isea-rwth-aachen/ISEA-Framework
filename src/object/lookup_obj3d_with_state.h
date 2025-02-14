/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lookup_obj3d_with_state.h
* Creation Date : 10-07-2022
* Last Modified : So 10 Jul 2022 15:30:15 CET
* Created By : Jonas Rinner
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUP_OBJ3D_WI_STATE_
#define _LOOKUP_OBJ3D_WI_STATE_

// BOOST
#include "../state/dgl_state.h"
#include "lookup_obj3d.h"
#include <boost/shared_ptr.hpp>

namespace object
{

/// Lookup state object  that requsts the current Value of the state to use it for the lookup
template < typename T >
class LookupObj3dWithState : public LookupObj3D< T >
{

    public:
    LookupObj3dWithState( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                          const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
                          boost::shared_ptr< state::State > stateRow, boost::shared_ptr< state::State > stateCol,
                          boost::shared_ptr< state::State > stateSli, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj3dWithState( const std::vector< std::vector< T > >& lookupData,
                          const std::vector< T >& measurementPointsRow, const std::vector< T >& measurementPointsColumn,
                          const std::vector< T >& measurementPointsSlice, boost::shared_ptr< state::State > stateRow,
                          boost::shared_ptr< state::State > stateCol, boost::shared_ptr< state::State > stateSli,
                          LookupObj3dWithState< T >* CloneObj, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj3dWithState( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                          const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
                          boost::shared_ptr< state::State > stateRow, boost::shared_ptr< state::State > stateCol,
                          boost::shared_ptr< state::State > stateSli, boost::shared_ptr< state::State > stateLHD,
                          LookupObj3dWithState< T >* CloneObj, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj3dWithState( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                          const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
                          boost::shared_ptr< state::State > stateRow, boost::shared_ptr< state::State > stateCol,
                          boost::shared_ptr< state::State > stateSli, boost::shared_ptr< state::State > stateLHD,
                          lookup::LookupType type = lookup::LINEAR_INTERPOLATION );

    virtual ~LookupObj3dWithState(){};

    virtual T GetValue() const;
    virtual T GetLastValue() const;    ///< Return the last done lookup
    virtual T operator()() const;      ///< Return the last done lookup

    virtual T GetValue( const T val1, const T val2, const T val3 );
    virtual T operator()( const T val, const T val2, const T val3 );

    virtual T GetValue( const T val1, const T val2, const T val3, const T val4 );
    virtual T operator()( const T val, const T val2, const T val3, const T val4 );

    virtual T GetValue( const T val1, const T val2, const T val3, const T val4, const T val5 );
    virtual T operator()( const T val, const T val2, const T val3, const T val4, const T val5 );

    virtual T GetLastValue( const T val1, const T val2, const T val3 );

    const state::State* GetStateRow() const { return mStateRow.get(); }
    const state::State* GetStateCol() const { return mStateCol.get(); }
    const state::State* GetStateSli() const { return mStateSli.get(); }
    const state::State* GetStateLHD() const { return mStateLHD.get(); }

    boost::shared_ptr< state::State > GetBoostStateRow() const { return mStateRow; }
    boost::shared_ptr< state::State > GetBoostStateCol() const { return mStateCol; }
    boost::shared_ptr< state::State > GetBoostStateSli() const { return mStateSli; }
    boost::shared_ptr< state::State > GetBoostStateLHD() const { return mStateLHD; }

    virtual const char* GetName() const;

    private:
    boost::shared_ptr< state::State > mStateRow;
    boost::shared_ptr< state::State > mStateCol;
    boost::shared_ptr< state::State > mStateSli;
    boost::shared_ptr< state::State > mStateLHD;

    protected:
    bool LHDDefined;
};

template < typename T >
LookupObj3dWithState< T >::LookupObj3dWithState( const std::vector< std::vector< T > >& lookupData,
                                                 const std::vector< T >& measurementPointsRow,
                                                 const std::vector< T >& measurementPointsColumn,
                                                 const std::vector< T >& measurementPointsSlice,
                                                 boost::shared_ptr< state::State > stateRow, boost::shared_ptr< state::State > stateCol,
                                                 boost::shared_ptr< state::State > stateSli, lookup::LookupType type )
    : LookupObj3D< T >( lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice, type )
    , mStateRow( stateRow )
    , mStateCol( stateCol )
    , mStateSli( stateSli )
    , LHDDefined( false )
{
}

template < typename T >
LookupObj3dWithState< T >::LookupObj3dWithState(
 const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
 const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
 boost::shared_ptr< state::State > /* stateRow */, boost::shared_ptr< state::State > /* stateCol */,
 boost::shared_ptr< state::State > /* stateCrate */, LookupObj3dWithState< T >* CloneObj, lookup::LookupType type )
    : LookupObj3D< T >( lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice, CloneObj, type )
    , mStateRow( CloneObj->mStateRow )
    , mStateCol( CloneObj->mStateCol )
    , mStateSli( CloneObj->mStateSli )
    , LHDDefined( false )
{
}

template < typename T >
LookupObj3dWithState< T >::LookupObj3dWithState(
 const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
 const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
 boost::shared_ptr< state::State > stateRow, boost::shared_ptr< state::State > stateCol,
 boost::shared_ptr< state::State > stateSli, boost::shared_ptr< state::State > stateLHD, lookup::LookupType type )
    : LookupObj3D< T >( lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice, type )
    , mStateRow( stateRow )
    , mStateCol( stateCol )
    , mStateSli( stateSli )
    , mStateLHD( stateLHD )
    , LHDDefined( true )
{
}

template < typename T >
LookupObj3dWithState< T >::LookupObj3dWithState(
 const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
 const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
 boost::shared_ptr< state::State > /* stateRow */, boost::shared_ptr< state::State > /* stateCol */,
 boost::shared_ptr< state::State > /* stateCrate */, boost::shared_ptr< state::State > /* stateLHD */,
 LookupObj3dWithState< T >* CloneObj, lookup::LookupType type )
    : LookupObj3D< T >( lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice, CloneObj, type )
    , mStateRow( CloneObj->mStateRow )
    , mStateCol( CloneObj->mStateCol )
    , mStateSli( CloneObj->mStateSli )
    , mStateLHD( CloneObj->mStateLHD )
    , LHDDefined( true )
{
}

template < typename T >
T LookupObj3dWithState< T >::GetValue() const
{
    if ( this->LHDDefined )
    {
        return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(),
                              mStateLHD->GetValue(), mStateSli->GetLastValue() ) *
               this->mFactor;
    }
    else
    {
        return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue() ) * this->mFactor;
    }
}

template < typename T >
T LookupObj3dWithState< T >::operator()() const
{
    if ( this->LHDDefined )
    {
        return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(),
                              mStateLHD->GetValue(), mStateSli->GetLastValue() ) *
               this->mFactor;
    }
    else
    {
        return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue() ) * this->mFactor;
    }
}

template < typename T >
T LookupObj3dWithState< T >::GetValue( const T /* val1 */, const T /* val2 */, const T /* val3 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue() ) * this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue() ) * this->mFactor;
}

template < typename T >
T LookupObj3dWithState< T >::operator()( const T /* val */, const T /* val2 */, const T /* val3 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue() ) * this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue() ) * this->mFactor;
};

// LHD
template < typename T >
T LookupObj3dWithState< T >::GetValue( const T /* val1 */, const T /* val2 */, const T /* val3 */, const T /* val4 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(),
                                      mStateLHD->GetValue(), mStateSli->GetLastValue() ) *
                       this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(), mStateLHD->GetValue(),
                          mStateSli->GetLastValue() ) *
           this->mFactor;
}

template < typename T >
T LookupObj3dWithState< T >::operator()( const T /* val */, const T /* val2 */, const T /* val3 */, const T /* val4 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(),
                                      mStateLHD->GetValue(), mStateSli->GetLastValue() ) *
                       this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(), mStateLHD->GetValue(),
                          mStateSli->GetLastValue() ) *
           this->mFactor;
};

template < typename T >
T LookupObj3dWithState< T >::GetValue( const T /* val1 */, const T /* val2 */, const T /* val3 */, const T /* val4 */, const T /* val5 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(),
                                      mStateLHD->GetValue(), mStateSli->GetLastValue() ) *
                       this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(), mStateLHD->GetValue(),
                          mStateSli->GetLastValue() ) *
           this->mFactor;
}

template < typename T >
T LookupObj3dWithState< T >::operator()( const T /* val */, const T /* val2 */, const T /* val3 */, const T /* val4 */, const T /* val5 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(),
                                      mStateLHD->GetValue(), mStateSli->GetLastValue() ) *
                       this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetValue(), mStateLHD->GetValue(),
                          mStateSli->GetLastValue() ) *
           this->mFactor;
};

template < typename T >
T LookupObj3dWithState< T >::GetLastValue() const
{
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetLastValue() ) * this->mFactor;
}

template < typename T >
T LookupObj3dWithState< T >::GetLastValue( const T /* val1 */, const T /* val2 */, const T /* val3 */ )
{
    this->mLastValue = this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetLastValue() ) * this->mFactor;
    return this->mLookup( mStateRow->GetValue(), mStateCol->GetValue(), mStateSli->GetLastValue() ) * this->mFactor;
}

template < typename T >
const char* LookupObj3dWithState< T >::GetName() const
{
    return "LookupObj3dWithState";
}

}    // namespace object
#endif /* _LOOKUP_OBJ3D_WI_STATE_ */
