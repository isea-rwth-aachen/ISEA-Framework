/*
 * stateclasswrapper.h
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#ifndef STATECLASSWRAPPER_H_
#define STATECLASSWRAPPER_H_

// STD
#include <algorithm>
#include <cctype>
#include <exception>
#include <string>

#include <boost/make_shared.hpp>

// ETC
#include "../classwrapper.h"
#include "../factory.h"

#include "../../electrical/electrical_data_struct.h"

#include "../../states/aging_state.h"
#include "../../states/soc.h"
#include "../../states/surface_soc.h"
#include "../../states/thermal_state.h"
#include "../../states/valueStateWrapper.h"

using namespace ::state;

namespace factory
{

struct ArgumentTypeState
{
    ArgumentTypeState()
        : mCellDiscretization( 1 )
        , mCapacityFactor( 1 ){};

    size_t mCellDiscretization = 1;
    boost::shared_ptr< ElectricalDataStruct< ScalarUnit > > mElectricalDataStruct =
     boost::shared_ptr< ElectricalDataStruct< ScalarUnit > >( new ElectricalDataStruct< ScalarUnit > );
    double mCapacityFactor;
};

/// Classwrapper for ::state namespace. This template class has to be
/// specialized in order to create an instance of a
/// particular class.
template < class ValueT >
class StateClassWrapperBase : public ClassWrapperBase< State, ArgumentTypeState >
{
    public:
    StateClassWrapperBase( Factory< ::state::State, ArgumentTypeState > *stateFactory )
        : mStateFactory( stateFactory ){};

    protected:
    /// Get a ::state Factory.
    Factory< ::state::State, ArgumentTypeState > *GetStateFactory() { return mStateFactory; }

    private:
    Factory< ::state::State, ArgumentTypeState > *const mStateFactory;
};

template < class TConcrete >
class StateClassWrapper : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
};

/// Classwrapper for electrical::state::Soc
template <>
class StateClassWrapper< electrical::state::Soc > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};

    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeState *arg = 0 )
    {
        double initialCapacity = param->GetElementDoubleValue( "InitialCapacity" );
        double actualCapacity = initialCapacity;
        if ( param->HasElementDirectChild( "ActualCapacity" ) )
            actualCapacity = param->GetElementDoubleValue( "ActualCapacity" );
        double initialSoc = param->GetElementDoubleValue( "InitialSoc" );

        if ( arg )
        {
            initialCapacity /= arg->mCellDiscretization;
            actualCapacity = initialCapacity * arg->mCapacityFactor;
        }

        if ( !param->HasElementDirectChild( "MinimumValue" ) )
        {
            return boost::shared_ptr< State >( new ::electrical::state::Soc( initialCapacity, actualCapacity, initialSoc ) );
        }
        else
        {
            double minimumValue = param->GetElementDoubleValue( "MinimumValue" );
            double maximumValue = param->GetElementDoubleValue( "MaximumValue" );
            return boost::shared_ptr< State >(
             new ::electrical::state::Soc( initialCapacity, actualCapacity, initialSoc, minimumValue, maximumValue ) );
        }
    }
};

/// Classwrapper for ::state::ThermalState
template <>
class StateClassWrapper< ::state::ThermalState< double > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeState * /* arg */ = 0 )
    {
        if ( param->HasElementDirectChild( "InitialThermalState" ) )
        {
#ifndef _DS1006
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                                 "Warning deprecated Parameter found "
                                                 "\"InitialThermalState\". \"InitialTemperature\" is "
                                                 "expected. Aborting." );
#endif /*_DS1006*/
        }
        if ( param->HasElementDirectChild( "InitialTemperature" ) )
        {
            double initTemp = param->GetElementDoubleValue( "InitialTemperature" );
            return boost::shared_ptr< State >( new ::state::ThermalState< double >( initTemp ) );
        }
        else
            return boost::shared_ptr< State >( new ::state::ThermalState< double >() );
    }
};

/// Classwrapper for electrical::state::SurfaceSoc
template <>
class StateClassWrapper< electrical::state::SurfaceSoc > : public StateClassWrapperBase< double >
{

    public:
    StateClassWrapper( Factory< ::state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter * /* param */, const ArgumentTypeState * /* arg */ = 0 )
    {
        return boost::shared_ptr< State >( new electrical::state::SurfaceSoc() );
    }
};

/// Classwrapper for state::ValueStateWrapper

template <>
class StateClassWrapper< ::state::ValueStateWrapper< ScalarUnit > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeState *arg = 0 )
    {
        if ( !arg )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoArgumentForStateFactory" );
        }

        std::string stateType( param->GetElementAttributeCharPtrValue( "StateType" ) );
        std::transform( stateType.begin(), stateType.end(), stateType.begin(), ::tolower );

        if ( stateType.find( "current" ) != std::string::npos )
        {
            return boost::shared_ptr< State >(
             new ::state::ValueStateWrapper< ScalarUnit >( &( arg->mElectricalDataStruct->mCurrentValue ) ) );
        }
        else if ( stateType.find( "voltage" ) != std::string::npos )
        {
            return boost::shared_ptr< State >(
             new ::state::ValueStateWrapper< ScalarUnit >( &( arg->mElectricalDataStruct->mVoltageValue ) ) );
        }
        else if ( stateType.find( "power" ) != std::string::npos )
        {
            return boost::shared_ptr< State >(
             new ::state::ValueStateWrapper< ScalarUnit >( &( arg->mElectricalDataStruct->mPowerValue ) ) );
        }
        // ERROR
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UndefinedStateType", stateType.c_str() );

        // should never reach here, but for compiler warnings this line is added
        return boost::shared_ptr< State >();
    }
};

/// Classwrapper for state::AgingState
template <>
class StateClassWrapper< state::AgingState > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< ::state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};

    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeState * /*arg*/ = 0 )
    {
        double cellAge = param->GetElementDoubleValue( "CellAge" ) * 3600 * 24;
        double chargeThroughput = param->GetElementDoubleValue( "ChargeThroughput" ) * 3600;

        return boost::make_shared< state::AgingState >( cellAge, chargeThroughput );
    }
};
} /* namespace factory */
#endif /* STATECLASSWRAPPER_H_ */
