/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lookup_obj2d.h
* Creation Date : 31-10-2012
* Last Modified : Fr 28 Nov 2014 11:12:19 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUP_OBJ2_
#define _LOOKUP_OBJ2_

// ETC
//#include "../convert/string2lookup.h"
#include "../lookup/lookup.h"
#include "../operators/vectorOperator.h"

#include "object.h"


namespace object
{

/// This Object has a 2D Lookup which can be called with GetValue(const double lookuppointRow, const double
/// lookuppointColumn) or operator()(const double lookuppointRow, const double lookuppointColumn)
template < typename T >
class LookupObj2D : public Object< T >
{
    public:
    LookupObj2D( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                 const std::vector< T >& measurementPointsColumn, lookup::LookupType type = lookup::LINEAR_INTERPOLATION );
    LookupObj2D( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                 const std::vector< T >& measurementPointsColumn, LookupObj2D< T >* CloneObj,
                 lookup::LookupType type = lookup::LINEAR_INTERPOLATION );

    virtual ~LookupObj2D(){};

    virtual T GetValue() const;      ///< Return the last done lookup
    virtual T operator()() const;    ///< Return the last done lookup

    virtual T GetValue( const T lookuppointRow, const T lookuppointColumn );    ///< Calculate the lookup value and store it for a faster return in the next call with ()
    virtual T operator()( const T lookuppointRow, const T lookuppointColumn );    ///< Calculate the lookup value and store it for a faster return in the next call with ()

    inline virtual T GetMaxValueOfLookup() { return mLookup.GetMaxValueOfLookup(); }
    inline virtual T GetMinValueOfLookup() { return mLookup.GetMinValueOfLookup(); }

    inline const lookup::Lookup2D< T >& GetLookup() const { return mLookup; }

    virtual const char* GetName() const;

    private:
    protected:
    const lookup::Lookup2D< T > mLookup;
};

template < typename T >
LookupObj2D< T >::LookupObj2D( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                               const std::vector< T >& measurementPointsColumn, lookup::LookupType type )
    : Object< T >()
    , mLookup( lookupData, measurementPointsRow, measurementPointsColumn, type )
{
}

template < typename T >
LookupObj2D< T >::LookupObj2D( const std::vector< std::vector< T > >& lookupData, const std::vector< T >& measurementPointsRow,
                               const std::vector< T >& measurementPointsColumn, LookupObj2D< T >* CloneObj, lookup::LookupType type )
    : Object< T >()
    , mLookup( ScalarMatrixMultipication( lookupData, CloneObj->mLookup.GetLookup() ), measurementPointsRow,
               measurementPointsColumn, type )
{
}

template < typename T >
T LookupObj2D< T >::GetValue( const T lookuppointRow, const T lookuppointColumn )
{
    this->mLastValue = mLookup( lookuppointRow, lookuppointColumn ) * this->mFactor;
    return this->mLastValue;
}

template < typename T >
T LookupObj2D< T >::operator()( const T lookuppointRow, const T lookuppointColumn )
{
    this->mLastValue = mLookup( lookuppointRow, lookuppointColumn ) * this->mFactor;
    return this->mLastValue;
}


template < typename T >
T LookupObj2D< T >::GetValue() const
{
    return this->mLastValue;
}

template < typename T >
T LookupObj2D< T >::operator()() const
{
    return this->mLastValue;
}

template < typename T >
const char* LookupObj2D< T >::GetName() const
{
    return "LookupObj2D";
}

} /* END NAMESPACE */
#endif /* _LOOKUP_OBJ_ */
