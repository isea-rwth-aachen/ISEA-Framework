#include "TestCalendarianAging.h"
#include "../../aging/calendarian_aging.h"
#include "../../object/expression_obj.h"
#include "../../state/valueStateWrapper.h"
#include <boost/make_shared.hpp>

void TestCalendarianAging::testNoAging()
{
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >();
    auto socState = boost::make_shared< state::Soc >( 10.0, 10.0, 50.0 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );
    std::vector< typename object::ExpressionObject< double >::ParameterT > objParams;
    const auto obj = boost::make_shared< object::ExpressionObject< double > >( "0.0001", objParams );
    {    // calculation time of zero seconds
        aging::CalendarianAging aging( 10, 0, 0, obj, obj, 1, 1, true, 1 );
        aging.CalculateAging( tpState, 3.0, 1.0 );
        TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
        TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
        TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );

        aging.CollectData( tpState, tpState, 0 );
        aging.CalculateAging( tpState, 3.0, 1.0 );

        TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
        TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
        TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
    }
    {    // aging disabled
        aging::CalendarianAging aging( 10, 0, 0, obj, obj, 1, 1, false, 1 );
        aging.CollectData( tpState, tpState, 10 );
        aging.CalculateAging( tpState, 3.0, 1.0 );

        TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
        TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
        TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
    }
}

void TestCalendarianAging::testAgingCalculation()
{
    double initialCap = 0.8;
    double initialRes = 1.1;
    double steptime = 10.0;
    double exponent = 0.8;
    double totalCap = 10.0;
    double soc = 50.0;

    auto voltageValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto temperatureValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto socValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    std::vector< typename object::ExpressionObject< double >::ParameterT > objParams{{"V", voltageValueState},
                                                                                     {"T", temperatureValueState},
                                                                                     {"SOC", socValueState}};
    const auto objCap = boost::make_shared< object::ExpressionObject< double > >( "0.0001 * (V + T - 300)", objParams );
    const auto objRes = boost::make_shared< object::ExpressionObject< double > >( "0.00015 * (V + T - 300)", objParams );

    aging::CalendarianAging aging( steptime, 0, 0, objCap, objRes, initialCap, initialRes, true, exponent );
    *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
    *temperatureValueState = state::ValueStateWrapper< double >( &aging.mActualTemperature );
    *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, 3.8, 0 );
    auto socState = boost::make_shared< state::Soc >( totalCap, totalCap, soc );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 300 - 273.15 );
    aging::TwoportState tpState( electricalData, socState, thermalState );
    aging.CollectData( tpState, tpState, 3.0 );
    electricalData->mVoltageValue = 4.1;
    thermalState->ResetTemperature();
    thermalState->AddTemperature( 296 - 273.15, 1 );
    aging.CollectData( tpState, tpState, 4.2 );
    aging.CalculateAging( tpState, 7.2, steptime * 86400 / 7.2 );
    double alphaCap = ( ( 0.0001 * ( 3.8 + 300 - 300 ) ) * 3.0 + ( 0.0001 * ( 4.1 + 296 - 300 ) ) * 4.2 ) / ( 3.0 + 4.2 );
    double alphaRes = ( ( 0.00015 * ( 3.8 + 300 - 300 ) ) * 3.0 + ( 0.00015 * ( 4.1 + 296 - 300 ) ) * 4.2 ) / ( 3.0 + 4.2 );
    double cap = 1 - pow( pow( 1 - initialCap, 1 / exponent ) + ( steptime * pow( alphaCap, 1 / exponent ) ), exponent );
    double res = 1 + pow( pow( initialRes - 1, 1 / exponent ) + ( steptime * pow( alphaRes, 1 / exponent ) ), exponent );
    double socOffset = 3600 * ( cap - 1.0 ) * totalCap * soc / 100;

    TS_ASSERT_DELTA( aging.GetCapacityFactor(), cap, 1e-6 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), socOffset, 1e-6 );
    TS_ASSERT_DELTA( aging.GetResistanceFactor(), res, 1e-6 );
}

void TestCalendarianAging::testReset()
{
    double initialCap = 0.8;
    double initialRes = 1.1;
    double steptime = 10;
    double exponent = 0.8;
    double totalCap = 10.0;
    double soc = 50.0;

    auto voltageValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto temperatureValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto socValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    std::vector< typename object::ExpressionObject< double >::ParameterT > objParams{{"V", voltageValueState},
                                                                                     {"T", temperatureValueState},
                                                                                     {"SOC", socValueState}};
    const auto objCap = boost::make_shared< object::ExpressionObject< double > >( "0.0001 * (V + T - 300)", objParams );
    const auto objRes = boost::make_shared< object::ExpressionObject< double > >( "0.00015 * (V + T - 300)", objParams );

    aging::CalendarianAging aging( steptime, 0, 0, objCap, objRes, initialCap, initialRes, true, exponent );
    *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
    *temperatureValueState = state::ValueStateWrapper< double >( &aging.mActualTemperature );
    *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );

    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, 3.8, 0 );
    auto socState = boost::make_shared< state::Soc >( totalCap, totalCap, soc );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 300 - 273.15 );
    aging::TwoportState tpState( electricalData, socState, thermalState );

    aging.CollectData( tpState, tpState, 3 );

    aging.ResetToPointInTime( 0.0 );
    aging.CalculateAging( tpState, 3.0, steptime * 86400 / 3.0 );
    TS_ASSERT_EQUALS( aging.GetCapacityFactor(), initialCap );
    TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
    TS_ASSERT_EQUALS( aging.GetResistanceFactor(), initialRes );

    aging.CollectData( tpState, tpState, 3 );

    electricalData->mVoltageValue = 4.1;
    thermalState->ResetTemperature();
    thermalState->AddTemperature( 296 - 273.15, 1 );
    aging.CollectData( tpState, tpState, 4.2 );

    electricalData->mVoltageValue = 2.1;
    thermalState->ResetTemperature();
    thermalState->AddTemperature( 305 - 273.15, 1 );
    aging.CollectData( tpState, tpState, 2.5 );
    aging.ResetToPointInTime( 7.2 );

    aging.CalculateAging( tpState, 7.2, steptime * 86400 / 7.2 );
    double alphaCap = ( ( 0.0001 * ( 3.8 + 300 - 300 ) ) * 3 + ( 0.0001 * ( 4.1 + 296 - 300 ) ) * 4.2 ) / ( 3 + 4.2 );
    double alphaRes = ( ( 0.00015 * ( 3.8 + 300 - 300 ) ) * 3 + ( 0.00015 * ( 4.1 + 296 - 300 ) ) * 4.2 ) / ( 3 + 4.2 );
    double cap = 1 - pow( pow( 1 - initialCap, 1 / exponent ) + ( steptime * pow( alphaCap, 1 / exponent ) ), exponent );
    double res = 1 + pow( pow( initialRes - 1, 1 / exponent ) + ( steptime * pow( alphaRes, 1 / exponent ) ), exponent );
    double socOffset = 3600 * ( cap - 1.0 ) * totalCap * soc / 100;

    TS_ASSERT_DELTA( aging.GetCapacityFactor(), cap, 1e-6 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), socOffset, 1e-6 );
    TS_ASSERT_DELTA( aging.GetResistanceFactor(), res, 1e-6 );
}

void TestCalendarianAging::testFormulaVariables()
{
    double voltage = 3.5;
    double soc = 0.73;
    double temperature = 300.0;    // [K]
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, voltage, 0 );
    auto socState = boost::make_shared< state::Soc >( 1.0, 1.0, soc * 100 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( temperature - 273.15 );
    aging::TwoportState tpState( electricalData, socState, thermalState );
    auto voltageValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto temperatureValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto socValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    std::vector< typename object::ExpressionObject< double >::ParameterT > objParams{{"V", voltageValueState},
                                                                                     {"T", temperatureValueState},
                                                                                     {"SOC", socValueState}};
    {
        const auto obj = boost::make_shared< object::ExpressionObject< double > >( "V", objParams );
        aging::CalendarianAging aging( 1.0, 0, 0, obj, obj, 1.0, 1.0, true, 1.0 );
        *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
        *temperatureValueState = state::ValueStateWrapper< double >( &aging.mActualTemperature );
        *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
        aging.CollectData( tpState, tpState, 5.0 );
        aging.CalculateAging( tpState, 5.0, 1.0 );
        TS_ASSERT_DELTA( aging.GetStressFactorCapacity(), voltage, 1e-6 );
        TS_ASSERT_DELTA( aging.GetStressFactorResistance(), voltage, 1e-6 );
    }
    {
        const auto obj = boost::make_shared< object::ExpressionObject< double > >( "T", objParams );
        aging::CalendarianAging aging( 1.0, 0, 0, obj, obj, 1.0, 1.0, true, 1.0 );
        *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
        *temperatureValueState = state::ValueStateWrapper< double >( &aging.mActualTemperature );
        *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
        aging.CollectData( tpState, tpState, 5.0 );
        aging.CalculateAging( tpState, 5.0, 1.0 );
        TS_ASSERT_DELTA( aging.GetStressFactorCapacity(), temperature, 1e-6 );
        TS_ASSERT_DELTA( aging.GetStressFactorResistance(), temperature, 1e-6 );
    }
    {
        const auto obj = boost::make_shared< object::ExpressionObject< double > >( "SOC", objParams );
        aging::CalendarianAging aging( 1.0, 0, 0, obj, obj, 1.0, 1.0, true, 1.0 );
        *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
        *temperatureValueState = state::ValueStateWrapper< double >( &aging.mActualTemperature );
        *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
        aging.CollectData( tpState, tpState, 5.0 );
        aging.CalculateAging( tpState, 5.0, 1.0 );
        TS_ASSERT_DELTA( aging.GetStressFactorCapacity(), soc, 1e-6 );
        TS_ASSERT_DELTA( aging.GetStressFactorResistance(), soc, 1e-6 );
    }
}