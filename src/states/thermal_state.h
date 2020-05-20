#ifndef _THERMAL_STATE_
#define _THERMAL_STATE_

#include "../exceptions/error_proto.h"
#include "../states/dgl_state.h"
#include <boost/foreach.hpp>
#include <limits>
#include <vector>


#include "../misc/macros.h"
#include "../misc/matrixInclude.h"


enum TemperatureGetFormat
{
    CELSIUS,
    KELVIN,
};


namespace electrical
{
template < typename T >
class TwoPort;
}

/// The state namespace contains classes that manage physical values used for evaluating look up tables and for updating
/// the equation system
namespace state
{

/// ThermalState enables the communication between the thermal and the electrical part of the model
template < typename T = double >
class ThermalState : public State
{
    struct PowerDissipationData
    {
        T mValue;
        T mTime;
    };

    public:
    explicit ThermalState( T initialTemperature = 0.0 );
    virtual ~ThermalState();
    double GetMaxDt() const;
    /// Returns temperature value, sum of all weighted temperature data if AddTemperature has been called, initial
    /// temperature if AddTemperature has not been called yet
    template < TemperatureGetFormat format = CELSIUS >
    T GetValue() const;
    T GetValue() const;
    const T& GetValueRef() const;
    /// Recalculates average temperature by adding new temperature weighted with volume
    void AddTemperature( T temperature, T volume );
    /// Forces a temperature value without calculating an average
    void SetTemperature( T temperature );
    /// Resets internal temperature, so temperature calculation with AddTemperature() can start anew
    void ResetTemperature();
    /// Get average heat generation during time intervall starting at time and lasting dt secs
    T GetPowerDissipation( T time, T dt ) const;
    /// Add heat generated at time
    void AddPowerDissipation( T value, T time );
    /// Deletes all heat generation data except for the last
    void ResetPowerDissipation();
    /// Deletes all heat generation data after time
    void ResetPowerDissipationToTime( T time );
    /// Sets a heat generation that will be given out at any time
    void SetFixedPowerDissipation( T value );

    private:
    std::vector< PowerDissipationData > mPowerDissipations;
    T mFixedPowerDissipation;
    T mTemperature;
    T mTotalVolume;
};


template < typename T >
ThermalState< T >::ThermalState( T initialTemperature )
    : mFixedPowerDissipation( 0.0 )
    , mTemperature( initialTemperature )
    , mTotalVolume( 0.0 )
{
    const PowerDissipationData data = {0.0, 0.0};
    mPowerDissipations.push_back( data );
}

template < typename T >
ThermalState< T >::~ThermalState()
{
}

template < typename T >
T ThermalState< T >::GetValue() const
{
    return GetValue<>();
}
template < typename T >
template < TemperatureGetFormat getFormat >
T ThermalState< T >::GetValue() const
{
    double returnValue = 0;

    switch ( getFormat )
    {
        case TemperatureGetFormat::CELSIUS:
            returnValue = mTemperature;
            break;

        case TemperatureGetFormat::KELVIN:
            returnValue = mTemperature + 273.15;
            break;

        default:
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedFormat", "ThermalState" );
    }

    return returnValue;
}

template < typename T >
const T& ThermalState< T >::GetValueRef() const
{
    return mTemperature;
}

template < typename T >
void ThermalState< T >::AddTemperature( T temperature, T volume )
{
    if ( volume <= 0.0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorVolZero" );

    T mNewTotalVolume = mTotalVolume + volume;
    mTemperature = ( temperature * volume + mTemperature * mTotalVolume ) / mNewTotalVolume;
    mTotalVolume = mNewTotalVolume;
}

template < typename T >
void ThermalState< T >::SetTemperature( T temperature )
{
    mTemperature = temperature;
}

template < typename T >
void ThermalState< T >::ResetTemperature()
{
    mTemperature = 0.0;
    mTotalVolume = 0.0;
}

template < typename T >
void ThermalState< T >::AddPowerDissipation( T value, T time )
{
    const PowerDissipationData data = {value, time};
    mPowerDissipations.push_back( data );
}

template < typename T >
void ThermalState< T >::ResetPowerDissipation()
{
    if ( mPowerDissipations.empty() )
        return;

    typename std::vector< PowerDissipationData >::iterator it = mPowerDissipations.end();
    --it;
    mPowerDissipations.erase( mPowerDissipations.begin(), it );
}

template < typename T >
void ThermalState< T >::ResetPowerDissipationToTime( T time )
{
    if ( mPowerDissipations.empty() )
        return;

    typename std::vector< PowerDissipationData >::iterator it = mPowerDissipations.begin();

    for ( ; it != mPowerDissipations.end(); ++it )
    {
        if ( it->mTime > time )
            break;
    }

    mPowerDissipations.erase( it, mPowerDissipations.end() );
}

template < typename T >
void ThermalState< T >::SetFixedPowerDissipation( T value )
{
    mPowerDissipations.clear();
    mFixedPowerDissipation = value;
}

template < typename T >
T ThermalState< T >::GetPowerDissipation( T time, T dt ) const
{
    if ( mPowerDissipations.empty() )
        return mFixedPowerDissipation;


    if ( time - dt < mPowerDissipations[0].mTime )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "errorTimePowerDissipation" );
    if ( dt == 0 )
        return 0.0;


    size_t indexBeforeTimeMinusDt = 0;
    size_t indexAfterTime = 0;
    size_t i = 1;
    for ( ; i < mPowerDissipations.size(); ++i )
    {
        if ( mPowerDissipations[i].mTime > time - dt )
        {
            indexBeforeTimeMinusDt = i - 1;
            break;
        }
    }
    for ( ; i < mPowerDissipations.size(); ++i )
    {
        if ( mPowerDissipations[i].mTime >= time )
        {
            indexAfterTime = i;
            break;
        }
    }
    if ( indexAfterTime == 0 )
        indexAfterTime = mPowerDissipations.size();


    T integral = 0.0;
    T startPointOfTime = 0.0;
    T endPointOfTime = 0.0;
    T valueDuringTime = 0.0;
    for ( size_t j = indexBeforeTimeMinusDt; j < indexAfterTime; ++j )
    {
        if ( j == indexBeforeTimeMinusDt )
            startPointOfTime = time - dt;
        else
            startPointOfTime = mPowerDissipations[j].mTime;

        if ( j == indexAfterTime - 1 )
            endPointOfTime = time;
        else
            endPointOfTime = mPowerDissipations[j + 1].mTime;

        valueDuringTime = mPowerDissipations[j].mValue;

        integral += valueDuringTime * ( endPointOfTime - startPointOfTime );
    }


    return integral / dt;
}
}    // namespace state
#endif
