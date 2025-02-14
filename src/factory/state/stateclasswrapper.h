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
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>

// ETC
#include "../classwrapper.h"
#include "../factory.h"

#include "../../electrical/electrical_data_struct.h"

#include "../../state/aging_state.h"
#include "../../state/soc.h"
#include "../../state/crate.h"
#include "../../state/cdirection.h"
#include "../../state/surface_soc.h"
#include "../../state/thermal_state.h"
#include "../../state/lhd.h"
#include "../../state/valueStateWrapper.h"

#include "../../object/lookup_obj3d_with_state.h"

using namespace state;

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

/// Classwrapper for state namespace. This template class has to be
/// specialized in order to create an instance of a
/// particular class.
template < class ValueT >
class StateClassWrapperBase : public ClassWrapperBase< State, ArgumentTypeState >
{
    public:
    StateClassWrapperBase( Factory< state::State, ArgumentTypeState > *stateFactory )
        : mStateFactory( stateFactory ){};

    protected:
    /// Get a state Factory.
    Factory< state::State, ArgumentTypeState > *GetStateFactory() { return mStateFactory; }

    private:
    Factory< state::State, ArgumentTypeState > *const mStateFactory;
};

template < class TConcrete >
class StateClassWrapper : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
};

/// Classwrapper for state::Soc
template <>
class StateClassWrapper< state::Soc > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
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
            return boost::shared_ptr< State >( new state::Soc( initialCapacity, actualCapacity, initialSoc ) );
        }
        else
        {
            double minimumValue = param->GetElementDoubleValue( "MinimumValue" );
            double maximumValue = param->GetElementDoubleValue( "MaximumValue" );
            return boost::shared_ptr< State >(
             new state::Soc( initialCapacity, actualCapacity, initialSoc, minimumValue, maximumValue ) );
        }
    }
};

/// Classwrapper for state::ThermalState
template <>
class StateClassWrapper< state::ThermalState< double > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
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
            return boost::shared_ptr< State >( new state::ThermalState< double >( initTemp ) );
        }
        else
            return boost::shared_ptr< State >( new state::ThermalState< double >() );
    }
};

/// Classwrapper for state::CRateState
template <>
class StateClassWrapper< state::CRateState< double > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};

    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeState * /* arg */ = 0 )
    {
        return boost::shared_ptr< State >( new state::CRateState< double >() );
    }
};

/// Classwrapper for state::LHDState

template < class ValueT >
class StateClassWrapper< state::LHDState< ValueT > > : public StateClassWrapperBase< ValueT >
{
    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};

    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeState *arg )
    {
        if ( param->HasElementDirectChild( "TimeConstantLHD" ) )
        {
            double step = param->GetElementDoubleValue( "SampleRate" );
            boost::shared_ptr< xmlparser::XmlParameter > param_new = param->GetElementChild( "TimeConstantLHD" );
            // const xmlparser::XmlParameter *param_new = param->GetElementChild( "TimeConstantLHD" ).get(); //.get();

            boost::shared_ptr< state::State > soc;
            boost::shared_ptr< state::State > crate;
            ArgumentTypeState stateArg;

            //// First Fraction
            // Row
            const auto rowStateNode1 = param_new->GetElementChild( "LookupTCFraction1" )->GetElementChild( "RowState" );

            boost::shared_ptr< state::State > rowstate1;

            rowstate1 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCFraction1" )->GetElementChild( "RowState" ),
                                                      &stateArg );

            // Column
            const auto colStateNode1 = param_new->GetElementChild( "LookupTCFraction1" )->GetElementChild( "ColState" );

            boost::shared_ptr< state::State > colstate1;

            colstate1 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCFraction1" )->GetElementChild( "ColState" ),
                                                      &stateArg );

            // Slice
            const auto sliStateNode1 = param_new->GetElementChild( "LookupTCFraction1" )->GetElementChild( "SliState" );

            boost::shared_ptr< state::State > slistate1;

            slistate1 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCFraction1" )->GetElementChild( "SliState" ),
                                                      &stateArg );

            std::vector< std::vector< ValueT > > lookupData1 =
             param_new->GetElementChild( "LookupTCFraction1" )->GetElementDoubleVecVecValue( "LookupData" );
            std::vector< ValueT > measurementPointsRow1 =
             param_new->GetElementChild( "LookupTCFraction1" )->GetElementDoubleVecValue( "MeasurementPointsRow" );
            std::vector< ValueT > measurementPointsColumn1 =
             param_new->GetElementChild( "LookupTCFraction1" )->GetElementDoubleVecValue( "MeasurementPointsColumn" );
            std::vector< ValueT > measurementPointsSlice1 =
             param_new->GetElementChild( "LookupTCFraction1" )->GetElementDoubleVecValue( "MeasurementPointsSlice" );

            boost::shared_ptr< object::LookupObj3dWithState< ValueT > > obj1;
            obj1 = boost::make_shared< LookupObj3dWithState< ValueT > >( lookupData1, measurementPointsRow1, measurementPointsColumn1,
                                                                         measurementPointsSlice1, rowstate1, colstate1,
                                                                         slistate1, lookup::LookupType::LINEAR_INTERPOLATION );

            /// First Time Constant
            // Row
            const auto rowStateNode2 = param_new->GetElementChild( "LookupTCTau1" )->GetElementChild( "RowState" );

            boost::shared_ptr< state::State > rowstate2;

            rowstate2 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCTau1" )->GetElementChild( "RowState" ),
                                                      &stateArg );

            // Column
            const auto colStateNode2 = param_new->GetElementChild( "LookupTCTau1" )->GetElementChild( "ColState" );

            boost::shared_ptr< state::State > colstate2;

            colstate2 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCTau1" )->GetElementChild( "ColState" ),
                                                      &stateArg );

            // Slice
            const auto sliStateNode2 = param_new->GetElementChild( "LookupTCTau1" )->GetElementChild( "SliState" );

            boost::shared_ptr< state::State > slistate2;

            slistate2 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCTau1" )->GetElementChild( "SliState" ),
                                                      &stateArg );

            std::vector< std::vector< ValueT > > lookupData2 =
             param_new->GetElementChild( "LookupTCTau1" )->GetElementDoubleVecVecValue( "LookupData" );
            std::vector< ValueT > measurementPointsRow2 =
             param_new->GetElementChild( "LookupTCTau1" )->GetElementDoubleVecValue( "MeasurementPointsRow" );
            std::vector< ValueT > measurementPointsColumn2 =
             param_new->GetElementChild( "LookupTCTau1" )->GetElementDoubleVecValue( "MeasurementPointsColumn" );
            std::vector< ValueT > measurementPointsSlice2 =
             param_new->GetElementChild( "LookupTCTau1" )->GetElementDoubleVecValue( "MeasurementPointsSlice" );

            boost::shared_ptr< object::LookupObj3dWithState< ValueT > > obj2;
            obj2 = boost::make_shared< LookupObj3dWithState< ValueT > >( lookupData2, measurementPointsRow2, measurementPointsColumn2,
                                                                         measurementPointsSlice2, rowstate2, colstate2,
                                                                         slistate2, lookup::LookupType::LINEAR_INTERPOLATION );

            //// Second Fraction
            // Row
            const auto rowStateNode3 = param_new->GetElementChild( "LookupTCFraction2" )->GetElementChild( "RowState" );

            boost::shared_ptr< state::State > rowstate3;

            rowstate3 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCFraction2" )->GetElementChild( "RowState" ),
                                                      &stateArg );

            // Column
            const auto colStateNode3 = param_new->GetElementChild( "LookupTCFraction2" )->GetElementChild( "ColState" );

            boost::shared_ptr< state::State > colstate3;

            colstate3 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCFraction2" )->GetElementChild( "ColState" ),
                                                      &stateArg );

            // Slice
            const auto sliStateNode3 = param_new->GetElementChild( "LookupTCFraction2" )->GetElementChild( "SliState" );

            boost::shared_ptr< state::State > slistate3;

            slistate3 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCFraction2" )->GetElementChild( "SliState" ),
                                                      &stateArg );

            std::vector< std::vector< ValueT > > lookupData3 =
             param_new->GetElementChild( "LookupTCFraction2" )->GetElementDoubleVecVecValue( "LookupData" );
            std::vector< ValueT > measurementPointsRow3 =
             param_new->GetElementChild( "LookupTCFraction2" )->GetElementDoubleVecValue( "MeasurementPointsRow" );
            std::vector< ValueT > measurementPointsColumn3 =
             param_new->GetElementChild( "LookupTCFraction2" )->GetElementDoubleVecValue( "MeasurementPointsColumn" );
            std::vector< ValueT > measurementPointsSlice3 =
             param_new->GetElementChild( "LookupTCFraction2" )->GetElementDoubleVecValue( "MeasurementPointsSlice" );

            boost::shared_ptr< object::LookupObj3dWithState< ValueT > > obj3;
            obj3 = boost::make_shared< LookupObj3dWithState< ValueT > >( lookupData3, measurementPointsRow3, measurementPointsColumn3,
                                                                         measurementPointsSlice3, rowstate3, colstate3,
                                                                         slistate3, lookup::LookupType::LINEAR_INTERPOLATION );

            /// Second Time Constant
            // Row
            const auto rowStateNode4 = param_new->GetElementChild( "LookupTCTau2" )->GetElementChild( "RowState" );

            boost::shared_ptr< state::State > rowstate4;

            rowstate4 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCTau2" )->GetElementChild( "RowState" ),
                                                      &stateArg );

            // Column
            const auto colStateNode4 = param_new->GetElementChild( "LookupTCTau2" )->GetElementChild( "ColState" );

            boost::shared_ptr< state::State > colstate4;

            colstate4 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCTau2" )->GetElementChild( "ColState" ),
                                                      &stateArg );

            // Slice
            const auto sliStateNode4 = param_new->GetElementChild( "LookupTCTau2" )->GetElementChild( "SliState" );

            boost::shared_ptr< state::State > slistate4;

            slistate4 =
             this->GetStateFactory()->CreateInstance( param_new->GetElementChild( "LookupTCTau2" )->GetElementChild( "SliState" ),
                                                      &stateArg );

            std::vector< std::vector< ValueT > > lookupData4 =
             param_new->GetElementChild( "LookupTCTau2" )->GetElementDoubleVecVecValue( "LookupData" );
            std::vector< ValueT > measurementPointsRow4 =
             param_new->GetElementChild( "LookupTCTau2" )->GetElementDoubleVecValue( "MeasurementPointsRow" );
            std::vector< ValueT > measurementPointsColumn4 =
             param_new->GetElementChild( "LookupTCTau2" )->GetElementDoubleVecValue( "MeasurementPointsColumn" );
            std::vector< ValueT > measurementPointsSlice4 =
             param_new->GetElementChild( "LookupTCTau2" )->GetElementDoubleVecValue( "MeasurementPointsSlice" );

            boost::shared_ptr< object::LookupObj3dWithState< ValueT > > obj4;
            obj4 = boost::make_shared< LookupObj3dWithState< ValueT > >( lookupData4, measurementPointsRow4, measurementPointsColumn4,
                                                                         measurementPointsSlice4, rowstate4, colstate4,
                                                                         slistate4, lookup::LookupType::LINEAR_INTERPOLATION );

            return boost::shared_ptr< State >( new state::LHDState< double >( obj1, obj2, obj3, obj4, step ) );
        }
        else
        {
            return boost::shared_ptr< State >( new state::LHDState< double >() );
        }
    };
};

/// Classwrapper for state::CDirection
template <>
class StateClassWrapper< state::CDirection< double > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};

    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeState * /* arg */ = 0 )
    {
        return boost::shared_ptr< State >( new state::CDirection< double >() );
    }
};

/// Classwrapper for state::SurfaceSoc
template <>
class StateClassWrapper< state::SurfaceSoc > : public StateClassWrapperBase< double >
{

    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
        : StateClassWrapperBase< double >( stateFactory ){};
    boost::shared_ptr< State > CreateInstance( const xmlparser::XmlParameter * /* param */, const ArgumentTypeState * /* arg */ = 0 )
    {
        return boost::shared_ptr< State >( new state::SurfaceSoc() );
    }
};

/// Classwrapper for state::ValueStateWrapper

template <>
class StateClassWrapper< state::ValueStateWrapper< ScalarUnit > > : public StateClassWrapperBase< double >
{
    public:
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
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
             new state::ValueStateWrapper< ScalarUnit >( &( arg->mElectricalDataStruct->mCurrentValue ) ) );
        }
        else if ( stateType.find( "voltage" ) != std::string::npos )
        {
            return boost::shared_ptr< State >(
             new state::ValueStateWrapper< ScalarUnit >( &( arg->mElectricalDataStruct->mVoltageValue ) ) );
        }
        else if ( stateType.find( "power" ) != std::string::npos )
        {
            return boost::shared_ptr< State >(
             new state::ValueStateWrapper< ScalarUnit >( &( arg->mElectricalDataStruct->mPowerValue ) ) );
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
    StateClassWrapper( Factory< state::State, ArgumentTypeState > *stateFactory )
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
