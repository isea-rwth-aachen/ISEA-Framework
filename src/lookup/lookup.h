/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : lookup.h
* Creation Date : 22-05-2012
* Last Modified : Di 13 Okt 2015 18:15:43 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _LOOKUP_
#define _LOOKUP_

// TODO: This should be deleted when the RT-Board port of the STL correctly contains std::runtime
#ifdef __EXCEPTIONS__
#include <stdexcept>
#include <string>
#endif


// STD
#include <vector>
#include <algorithm>

// BOOST
#include <boost/scoped_ptr.hpp>

#include "lookupType.h"
#include "../exceptions/error_proto.h"

#ifdef __EXCEPTIONS__
#include "lookupException.h"
#endif

namespace lookup
{
using namespace std;

/// This class defines the behaviour of a 1D  lookup
template < typename T = double >
class Lookup1D
{
    public:
    Lookup1D( std::vector< T > lookupData, std::vector< T > measurementPoints, LookupType type = LINEAR_INTERPOLATION );

    virtual ~Lookup1D(){};

    T operator()( const T lookupPointRow ) const;

    T DoLookup( const T lookupPoint ) const;
    inline T GetMaxValueOfLookup() const { return mLookupType->GetMaxValueOfLookup(); }
    inline T GetMinValueOfLookup() const { return mLookupType->GetMinValueOfLookup(); }

    inline const boost::scoped_ptr< LookupType1D< T > >& GetLookupType() const { return mLookupType; };
    const std::vector< T >& GetLookup() const { return mLookupType->GetLookup(); };
    LookupType mType = LINEAR_INTERPOLATION;

    private:
    protected:
    typename boost::scoped_ptr< LookupType1D< T > > mLookupType;
};

template < typename T >
Lookup1D< T >::Lookup1D( std::vector< T > lookupData, std::vector< T > measurementPoints, LookupType type )
    : mType( type )
{
    if ( lookupData.size() != measurementPoints.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnequalSize", lookupData.size(),
                                             measurementPoints.size() );

    if ( lookupData.empty() || measurementPoints.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyLookupData" );

    for ( size_t i = 0; i < measurementPoints.size() - 1; ++i )
    {
        if ( measurementPoints.at( i ) > measurementPoints.at( i + 1 ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NonSortedLookup" );
    }

    if ( type == LINEAR_INTERPOLATION )
        mLookupType.reset( new Linear1DInterpolation< T >( lookupData, measurementPoints ) );
    else if ( type == NEAREST_NEIGHBOUR )
        mLookupType.reset( new NearestNeighbour1D< T >( lookupData, measurementPoints ) );
    else if ( type == CONSTANT_UNTIL_THRESHOLD )
        mLookupType.reset( new ConstantUntilThreshold1D< T >( lookupData, measurementPoints ) );
    else if ( type == SPLINE_INTERPOLATION )
        mLookupType.reset( new SplineInterpolation1D< T >( lookupData, measurementPoints ) );
    else
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownLookupType" );
    }
};

template < typename T >
T Lookup1D< T >::operator()( const T lookupPointRow ) const
{
    return DoLookup( lookupPointRow );
}

template < typename T >
T Lookup1D< T >::DoLookup( const T lookupPoint ) const
{
    return mLookupType->DoLookup( lookupPoint );
};

//---------------------------------------------------//

/// Lookup2D
template < typename T = double >
class Lookup2D
{
    public:
    Lookup2D( std::vector< std::vector< T > > lookupData, std::vector< T > measurementPointsRow,
              std::vector< T > measurementPointsColumn, LookupType type = LINEAR_INTERPOLATION );

    virtual ~Lookup2D(){};
    T operator()( const T lookupPointRow, const T lookupPointColumn ) const;

    T DoLookup( const T lookupPointRow, const T lookupPointColumn ) const;

    inline T GetMaxValueOfLookup() const { return mLookupType->GetMaxValueOfLookup(); }
    inline T GetMinValueOfLookup() const { return mLookupType->GetMinValueOfLookup(); }

    inline const boost::scoped_ptr< LookupType2D< T > >& GetLookupType() const { return mLookupType; };
    const std::vector< std::vector< T > >& GetLookup() const { return mLookupType->GetLookup(); };
    LookupType mType = LINEAR_INTERPOLATION;

    private:
    protected:
    typename boost::scoped_ptr< LookupType2D< T > > mLookupType;
};


template < typename T >
Lookup2D< T >::Lookup2D( std::vector< std::vector< T > > lookupData, std::vector< T > measurementPointsRow,
                         std::vector< T > measurementPointsColumn, LookupType type )
    : mType( type )
{

    for ( size_t i = 0; i < lookupData.size(); ++i )
    {
        if ( lookupData.at( i ).size() != measurementPointsRow.size() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnequalSize",
                                                 lookupData.at( i ).size(), measurementPointsRow.size() );
    }
    if ( lookupData.size() != measurementPointsColumn.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnequalSize", lookupData.size(),
                                             measurementPointsColumn.size() );

    for ( size_t i = 0; i < lookupData.size(); ++i )
    {
        if ( lookupData.at( i ).empty() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyLookupData" );
    }

    if ( lookupData.empty() || measurementPointsRow.empty() || measurementPointsColumn.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyLookupData" );

    size_t end = measurementPointsRow.size() - 1;
    for ( size_t i = 0; i < end; ++i )
    {
        if ( measurementPointsRow.at( i ) > measurementPointsRow.at( i + 1 ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NonSortedLookup" );
    }

    end = measurementPointsColumn.size() - 1;

    for ( size_t i = 0; i < end; ++i )
    {
        if ( measurementPointsColumn.at( i ) > measurementPointsColumn.at( i + 1 ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NonSortedLookup" );
    }

    if ( type == NEAREST_NEIGHBOUR )
        mLookupType.reset( new NearestNeighbour2D< T >( lookupData, measurementPointsRow, measurementPointsColumn ) );
    else if ( type == LINEAR_INTERPOLATION )
        mLookupType.reset( new Linear2DInterpolation< T >( lookupData, measurementPointsRow, measurementPointsColumn ) );
    else if ( type == CONSTANT_UNTIL_THRESHOLD )
        mLookupType.reset( new ConstantUntilThreshold2D< T >( lookupData, measurementPointsRow, measurementPointsColumn ) );
    else if ( type == SPLINE_INTERPOLATION )
        mLookupType.reset( new BicubicInterpolation2D< T >( lookupData, measurementPointsRow, measurementPointsColumn ) );
    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownLookupType" );
}

template < typename T >
T Lookup2D< T >::operator()( const T lookupPointRow, const T lookupPointColumn ) const
{
    return DoLookup( lookupPointRow, lookupPointColumn );
}

template < typename T >
T Lookup2D< T >::DoLookup( const T lookupPointRow, const T lookupPointColumn ) const
{
    return mLookupType->DoLookup( lookupPointRow, lookupPointColumn );
}

//---------------------------------------------------//

/// Lookup3D
template < typename T = double >
class Lookup3D
{
    public:
    Lookup3D( std::vector< std::vector< T > > lookupData, std::vector< T > measurementPointsRow, std::vector< T > measurementPointsColumn,
              std::vector< T > measurementPointsSlice, LookupType type = LINEAR_INTERPOLATION );

    virtual ~Lookup3D(){};
    T operator()( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice ) const;
    T DoLookup( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice ) const;

    T operator()( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice, const T lookuppointLHD ) const;
    T DoLookup( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice, const T lookuppointLHD ) const;

    T operator()( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice, const T lookuppointLHD,
                  const T lookuppointLastSlice ) const;
    T DoLookup( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice, const T lookuppointLHD,
                const T lookuppointLastSlice ) const;

    inline T GetMaxValueOfLookup() const { return mLookupType->GetMaxValueOfLookup(); }
    inline T GetMinValueOfLookup() const { return mLookupType->GetMinValueOfLookup(); }

    inline const boost::scoped_ptr< LookupType3D< T > >& GetLookupType() const { return mLookupType; };
    const std::vector< std::vector< std::vector< T > > >& GetLookup() const { return mLookupType->GetLookup(); };
    LookupType mType = LINEAR_INTERPOLATION;

    private:
    protected:
    typename boost::scoped_ptr< LookupType3D< T > > mLookupType;
};


template < typename T >
Lookup3D< T >::Lookup3D( std::vector< std::vector< T > > lookupData, std::vector< T > measurementPointsRow,
                         std::vector< T > measurementPointsColumn, std::vector< T > measurementPointsSlice, LookupType type )
    : mType( type )
{

    for ( size_t i = 0; i < lookupData.size(); ++i )
    {
        if ( lookupData.at( i ).size() != measurementPointsRow.size() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnequalSize",
                                                 lookupData.at( i ).size(), measurementPointsRow.size() );
    }
    if ( lookupData.size() != measurementPointsColumn.size() * measurementPointsSlice.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnequalSize", lookupData.size(),
                                             measurementPointsColumn.size() );

    for ( size_t i = 0; i < lookupData.size(); ++i )
    {
        if ( lookupData.at( i ).empty() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyLookupData" );
    }

    if ( lookupData.empty() || measurementPointsRow.empty() || measurementPointsColumn.empty() || measurementPointsSlice.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyLookupData" );

    size_t end = measurementPointsRow.size() - 1;
    for ( size_t i = 0; i < end; ++i )
    {
        if ( measurementPointsRow.at( i ) > measurementPointsRow.at( i + 1 ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NonSortedLookup" );
    }

    end = measurementPointsColumn.size() - 1;

    for ( size_t i = 0; i < end; ++i )
    {
        if ( measurementPointsColumn.at( i ) > measurementPointsColumn.at( i + 1 ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NonSortedLookup" );
    }

    end = measurementPointsSlice.size() - 1;

    for ( size_t i = 0; i < end; ++i )
    {
        if ( measurementPointsSlice.at( i ) > measurementPointsSlice.at( i + 1 ) )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NonSortedLookup" );
    }

    /*if ( type == NEAREST_NEIGHBOUR )
        mLookupType.reset( new NearestNeighbour3D< T >( lookupData, measurementPointsRow, measurementPointsColumn ) );
    else */

    if ( type == LINEAR_INTERPOLATION )
        mLookupType.reset( new Linear3DInterpolation< T >( lookupData, measurementPointsRow, measurementPointsColumn,
                                                           measurementPointsSlice ) );
    // else if ( type == CONSTANT_UNTIL_THRESHOLD )
    //    mLookupType.reset( new ConstantUntilThreshold3D< T >( lookupData, measurementPointsRow, measurementPointsColumn ) );
    // else if ( type == SPLINE_INTERPOLATION )
    //   mLookupType.reset( new BicubicInterpolation3D< T >( lookupData, measurementPointsRow, measurementPointsColumn ) );
    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownLookupType" );
    // mLookupType.reset( new Linear3DInterpolation< T >( lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice ) );
}

template < typename T >
T Lookup3D< T >::operator()( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice ) const
{
    return DoLookup( lookupPointRow, lookupPointColumn, lookuppointSlice );
}

template < typename T >
T Lookup3D< T >::DoLookup( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice ) const
{
    return mLookupType->DoLookup( lookupPointRow, lookupPointColumn, lookuppointSlice );
}

template < typename T >
T Lookup3D< T >::operator()( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice, const T lookuppointLHD ) const
{
    return DoLookup( lookupPointRow, lookupPointColumn, lookuppointSlice, lookuppointLHD );
}

template < typename T >
T Lookup3D< T >::DoLookup( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice, const T lookuppointLHD ) const
{
    return mLookupType->DoLookup( lookupPointRow, lookupPointColumn, lookuppointSlice, lookuppointLHD );
}

template < typename T >
T Lookup3D< T >::operator()( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice,
                             const T lookuppointLHD, const T lookuppointLastSlice ) const
{
    return DoLookup( lookupPointRow, lookupPointColumn, lookuppointSlice, lookuppointLHD, lookuppointLastSlice );
}

template < typename T >
T Lookup3D< T >::DoLookup( const T lookupPointRow, const T lookupPointColumn, const T lookuppointSlice,
                           const T lookuppointLHD, const T lookuppointLastSlice ) const
{
    return mLookupType->DoLookup( lookupPointRow, lookupPointColumn, lookuppointSlice, lookuppointLHD, lookuppointLastSlice );
}

};    // namespace lookup

#endif /* _LOOKUP_ */
