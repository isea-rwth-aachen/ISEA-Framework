/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lookup_obj3d.h
* Creation Date : 09-07-2022
* Last Modified : Sa 09 Sa 2022 11:54 CET
* Created By : Jonas Rinner
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUP_OBJ3_
#define _LOOKUP_OBJ3_

// ETC
//#include "../convert/string2lookup.h"
#include "../lookup/lookup.h"
#include "../operators/vectorOperator.h"

#include "object.h"


namespace object
{

/// This Object has a 3D Lookup which can be called with GetValue(const double lookuppointRow, const double
/// lookuppointColumn) or operator()(const double lookuppointRow, const double lookuppointColumn)
template < typename T >
class LookupObj3D : public Object< T >
{
    public:
    LookupObj3D( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                 const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
                 lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj3D( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                 const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
                 LookupObj3D< T >* CloneObj, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );

    virtual ~LookupObj3D(){};

    virtual T GetValue() const;      ///< Return the last done lookup
    virtual T operator()() const;    ///< Return the last done lookup

    virtual T GetValue( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice );    ///< Calculate the lookup value and store it for a faster return in the next call with ()
    virtual T operator()( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice );    ///< Calculate the lookup value and store it for a faster return in the next call with ()

    virtual T GetValue( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice,
                        const T lookuppointLHD );    ///< Calculate the lookup value and store it for a faster return in the next call with ()
    virtual T operator()( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice, const T lookuppointLHD );

    inline virtual T GetMaxValueOfLookup() { return mLookup.GetMaxValueOfLookup(); }
    inline virtual T GetMinValueOfLookup() { return mLookup.GetMinValueOfLookup(); }

    inline const lookup::Lookup3D< T >& GetLookup() const { return mLookup; }

    virtual const char* GetName() const;

    private:
    protected:
    const lookup::Lookup3D< T > mLookup;
};

template < typename T >
LookupObj3D< T >::LookupObj3D( const std::vector< std::vector< T > >& lookupData,
                               const std::vector< T >& measurementPointsRow, const std::vector< T >& measurementPointsColumn,
                               const std::vector< T >& measurementPointsSlice, lookup::LookupType type )
    : Object< T >()
    , mLookup( lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice, type )
{
}

template < typename T >
LookupObj3D< T >::LookupObj3D( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                               const std::vector< T >& measurementPointsColumn, const std::vector< T >& measurementPointsSlice,
                               LookupObj3D< T >* CloneObj, lookup::LookupType type )
    : Object< T >()
    , mLookup( ScalarMatrixMultipication( lookupData, CloneObj->mLookup.GetLookup() ), measurementPointsRow,
               measurementPointsColumn, measurementPointsSlice, type )
{
}

template < typename T >
T LookupObj3D< T >::GetValue( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice )
{
    this->mLastValue = mLookup( lookuppointRow, lookuppointColumn, lookuppointSlice ) * this->mFactor;
    return this->mLastValue;
}

template < typename T >
T LookupObj3D< T >::operator()( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice )
{
    this->mLastValue = mLookup( lookuppointRow, lookuppointColumn, lookuppointSlice ) * this->mFactor;
    return this->mLastValue;
}

template < typename T >
T LookupObj3D< T >::GetValue( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice, const T lookuppointLHD )
{
    this->mLastValue = mLookup( lookuppointRow, lookuppointColumn, lookuppointSlice, lookuppointLHD ) * this->mFactor;
    return this->mLastValue;
}

template < typename T >
T LookupObj3D< T >::operator()( const T lookuppointRow, const T lookuppointColumn, const T lookuppointSlice, const T lookuppointLHD )
{
    this->mLastValue = mLookup( lookuppointRow, lookuppointColumn, lookuppointSlice, lookuppointLHD ) * this->mFactor;
    return this->mLastValue;
}

template < typename T >
T LookupObj3D< T >::GetValue() const
{
    return this->mLastValue;
}

template < typename T >
T LookupObj3D< T >::operator()() const
{
    return this->mLastValue;
}

template < typename T >
const char* LookupObj3D< T >::GetName() const
{
    return "LookupObj3D";
}

}    // namespace object
#endif /* _LOOKUP_OBJ_ */
