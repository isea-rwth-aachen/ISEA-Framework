#ifndef _CURRENT_PROFILE_
#define _CURRENT_PROFILE_

#ifndef __NO_STRING__
#include <istream>
#include <string>
#endif
#include <algorithm>
#include <functional>
#include <limits>
#include <vector>


#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <cstdlib>

#include "../exceptions/error_proto.h"
#include "eval_linear_interpolation.h"
#include "eval_no_interpolation.h"


class TestTimeSeries;


namespace electrical
{
using std::vector;

enum TimeSeriesType
{
    CURRENT,
    POWER
};


/// TimeSeries handles a time series of values determining values at certain points of time with some extra
/// functionality
template < typename T = double, template < typename > class EVALUATOR = EvalNoInterpolation >
class TimeSeries
{
    friend class ::TestTimeSeries;

    public:
#ifndef __NO_STRING__
    /**
     * @param[in] stream stream contains the data, i.e all values with the assigned points of time.The formate is:
     * time1, value1\n time2, value2\n, ... . The points of time must be in strictly ascending order. Except for double
     * entries (two following lines have the same time and value), these are accepted.
     * @param[in] timeScalingFactor The points of time can be scaled with this parameter, must be bigger than zero
     * @param[in] valueScalingFactor The values can be scaled with this parameter
     */
    explicit TimeSeries( std::istream &stream, T timeScalingFactor = 1.0, T valueScalingFactor = 1.0 );
#endif
    /**
     * A second constructor that uses a vector vector representation, see ResetData
     */
    TimeSeries( const vector< T > &timesData, const vector< T > &valuesData, T timeScalingFactor = 1.0, T valueScalingFactor = 1.0 );


    TimeSeries();
    virtual ~TimeSeries();
    /**
     * ResetData resets the time series with points of time and values, that can be directly injected as vector<T>
     * @param[in] timesData Points of time assigned to valuesData by the same index
     * @param[in] valuesData values assigned to points of time
     * @param[in] timeScalingFactor The points of time can be scaled with this parameter, must be bigger than zero
     * @param[in] valueScalingFactor The values can be scaled with this parameter
     */
    void ResetData( const vector< T > &timesData, const vector< T > &valuesData, T timeScalingFactor = 1.0,
                    T valueScalingFactor = 1.0 );
    /// SetTimeAndTriggerEvaluation sets the internal time and triggers the calculation of value at the newly set time,
    /// that can be gotten with GetValue()
    void SetTimeAndTriggerEvaluation( T time );

    void StartNewCycle();

    /// Gets internal time
    T GetTime() const;
    /// Gets the last time step in the series
    T GetMaxTime() const;

    /// Gets the value corresponding to the internal time
    T GetValue() const;

    /// GetTimeUntilMaxValueDeviation returns the point of time, at which the value would deviate more than
    /// maxDeltaValue from the internal (last calculated) value
    T GetTimeUntilMaxValueDeviation( T maxDeltaValue ) const;

    TimeSeriesType GetType() const;

    private:
#ifndef __NO_STRING__
    void ParseHeader( std::string line );
    void ParseCsvLine( const std::string &line, T &arg1, T &arg2 ) const;
#endif
    void CheckTimesValuesDataValidity() const;
    void ScaleTimesValuesData( T timeScalingFactor, T valueScalingFactor );
    bool inline LoopConditionForIndexIncrement() const;
    vector< T > mTimesData;
    vector< T > mValuesData;
    T mTime;
    T mValue;
    T mCycleLength;
    size_t mIndex;
    EVALUATOR< T > mEvaluator;
    TimeSeriesType mType;
};


#ifndef __NO_STRING__
template < typename T, template < typename > class EVALUATOR >
void TimeSeries< T, EVALUATOR >::ParseHeader( std::string line )
{
    // try to find profile type specified in the format type=<TYPE>
    std::string typeString = "type=";
    size_t typeStart = line.find( typeString );
    if ( typeStart == std::string::npos )
        return;
    typeStart += typeString.length();
    size_t typeEnd = line.find_first_of( "\r\n,; ", typeStart );
    if ( typeEnd == std::string::npos )
        typeEnd = line.length() + 1;
    std::string type = line.substr( typeStart, typeEnd - typeStart );
    // case insensitive comparison
    if ( boost::iequals( type, "current" ) )
        mType = TimeSeriesType::CURRENT;
    else if ( boost::iequals( type, "power" ) )
        mType = TimeSeriesType::POWER;
    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownProfileType", type.c_str() );
}

template < typename T, template < typename > class EVALUATOR >
TimeSeries< T, EVALUATOR >::TimeSeries( std::istream &stream, T timeScalingFactor, T valueScalingFactor )
    : mTime( 0.0 )
    , mIndex( 0 )
    , mType( TimeSeriesType::CURRENT )
{
    std::string line;

    if ( std::getline( stream, line ) )
    {
        if ( line.length() > 0 && line.at( 0 ) == '#' )    // header
        {
            ParseHeader( line );
            std::getline( stream, line );
        }

        if ( std::count( line.begin(), line.end(), ',' ) != 1 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DataFormat" );
        T time, value;
        ParseCsvLine( line, time, value );
        if ( time != 0.0 )
        {
            mTimesData.push_back( 0.0 );
            mValuesData.push_back( 0.0 );
        }
        mTimesData.push_back( time );
        mValuesData.push_back( value );
    }

    while ( std::getline( stream, line ) )
    {
        if ( line.length() == 0 || line.at( 0 ) == '#' )    // skip empty lines and comments
            continue;

        if ( std::count( line.begin(), line.end(), ',' ) != 1 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "DataFormat" );
        T time, value;
        ParseCsvLine( line, time, value );
        if ( time != mTimesData.back() || value != mValuesData.back() )
        {
            mTimesData.push_back( time );
            mValuesData.push_back( value );
        }
    }

    CheckTimesValuesDataValidity();
    ScaleTimesValuesData( timeScalingFactor, valueScalingFactor );
    mEvaluator.CalculateValue( true, mTimesData, mValuesData, mTime, mIndex, mValue );

    mCycleLength = GetMaxTime();
}
#endif

template < typename T, template < typename > class EVALUATOR >
TimeSeries< T, EVALUATOR >::TimeSeries()
    : mTime( 0.0 )
    , mValue( 0.0 )
    , mIndex( 0 )
    , mType( TimeSeriesType::CURRENT )
{
}

template < typename T, template < typename > class EVALUATOR >
TimeSeries< T, EVALUATOR >::TimeSeries( const vector< T > &timesData, const vector< T > &valuesData, T timeScalingFactor, T valueScalingFactor )
    : mTime( 0.0 )
    , mValue( 0.0 )
    , mIndex( 0 )
    , mType( TimeSeriesType::CURRENT )
{
    ResetData( timesData, valuesData, timeScalingFactor, valueScalingFactor );
}


template < typename T, template < typename > class EVALUATOR >
TimeSeries< T, EVALUATOR >::~TimeSeries()
{
}

template < typename T, template < typename > class EVALUATOR >
void TimeSeries< T, EVALUATOR >::ResetData( const vector< T > &timesData, const vector< T > &valuesData,
                                            T timeScalingFactor, T valueScalingFactor )
{
    mTimesData = timesData;
    mValuesData = valuesData;
    CheckTimesValuesDataValidity();
    ScaleTimesValuesData( timeScalingFactor, valueScalingFactor );
    mTime = 0.0;
    mIndex = 0;
    mEvaluator.CalculateValue( true, mTimesData, mValuesData, mTime, mIndex, mValue );
}

template < typename T, template < typename > class EVALUATOR >
T TimeSeries< T, EVALUATOR >::GetValue() const
{
    return mValue;
}

template < typename T, template < typename > class EVALUATOR >
void TimeSeries< T, EVALUATOR >::SetTimeAndTriggerEvaluation( T time )
{
    mTime = time;
    size_t indexSave = mIndex;
    while ( LoopConditionForIndexIncrement() )
        ++mIndex;
    mEvaluator.CalculateValue( indexSave != mIndex, mTimesData, mValuesData, time, mIndex, mValue );
}

template < typename T, template < typename > class EVALUATOR >
void TimeSeries< T, EVALUATOR >::StartNewCycle()
{
    std::vector< T > increaseVector( mTimesData.size(), mCycleLength );

    std::transform( mTimesData.begin(), mTimesData.end(), increaseVector.begin(), mTimesData.begin(), std::plus< T >() );

    ResetData( mTimesData, mValuesData );
}

template < typename T, template < typename > class EVALUATOR >
T TimeSeries< T, EVALUATOR >::GetMaxTime() const
{
    if ( mTimesData.empty() )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoEntry" );
    }
    return mTimesData.back();
}

template < typename T, template < typename > class EVALUATOR >
T TimeSeries< T, EVALUATOR >::GetTime() const
{
    return mTime;
}

template < typename T, template < typename > class EVALUATOR >
T TimeSeries< T, EVALUATOR >::GetTimeUntilMaxValueDeviation( T maxDeltaValue ) const
{
    return mEvaluator.GetTimeUntilMaxValueDeviation( mTimesData, mValuesData, mTime, mValue, mIndex, maxDeltaValue );
}

#ifndef __NO_STRING__
template < typename T, template < typename > class EVALUATOR >
void TimeSeries< T, EVALUATOR >::ParseCsvLine( const std::string &line, T &arg1, T &arg2 ) const
{
    arg1 = atof( line.c_str() );
    arg2 = atof( line.c_str() + line.find( "," ) + 1 );
}
#endif

template < typename T, template < typename > class EVALUATOR >
void TimeSeries< T, EVALUATOR >::CheckTimesValuesDataValidity() const
{
    if ( mTimesData.size() != mValuesData.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnEqualTimeData" );
    if ( mTimesData.size() == 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AtLeasOneElement" );
    if ( mTimesData[0] < 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "FirstPointNonNegative" );
    for ( size_t i = 0; i < mTimesData.size() - 1; ++i )
        if ( mTimesData[i] >= mTimesData[i + 1] )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NonMonotonicData", i, i, i );
}

template < typename T, template < typename > class EVALUATOR >
void TimeSeries< T, EVALUATOR >::ScaleTimesValuesData( T timeScalingFactor, T valueScalingFactor )
{
    if ( timeScalingFactor <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "timeScalingFactorLessZero" );

    if ( timeScalingFactor != 1.0 )
    {
        BOOST_FOREACH ( T &time, mTimesData )
            time *= timeScalingFactor;
    }
    if ( valueScalingFactor != 1.0 )
    {
        BOOST_FOREACH ( T &value, mValuesData )
            value *= valueScalingFactor;
    }
}

template < typename T, template < typename > class EVALUATOR >
bool TimeSeries< T, EVALUATOR >::LoopConditionForIndexIncrement() const
{
    if ( mIndex + 2 > mTimesData.size() )
        return false;
    return mTime >= mTimesData[mIndex + 1];
}

template < typename T, template < typename > class EVALUATOR >
TimeSeriesType TimeSeries< T, EVALUATOR >::GetType() const
{
    return mType;
}


}    // namespace electrical
#endif    // Include guard
