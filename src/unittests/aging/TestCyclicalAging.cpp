#include "TestCyclicalAging.h"
#include "../../aging/cyclical_aging.h"
#include "../../object/expression_obj.h"
#include "../../state/valueStateWrapper.h"
#include <boost/make_shared.hpp>

void TestCyclicalAging::testNoAging()
{
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >();
    auto socState = boost::make_shared< state::Soc >( 10.0, 10.0, 50.0 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );
    std::vector< typename object::ExpressionObject< double >::Parameter > objParams;
    const auto obj = boost::make_shared< object::ExpressionObject< double > >( "0.0001", objParams );
    {    // calculation time of zero seconds
        aging::CyclicalAging aging( 10, 0, 0, 1e3, 1e3, obj, obj, 1.0, 1.0, true, 1, 1, "Ah" );
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
        aging::CyclicalAging aging( 10, 0, 0, 1e3, 1e3, obj, obj, 1.0, 1.0, false, 1, 1, "Ah" );

        aging.CollectData( tpState, tpState, 10 );
        aging.CalculateAging( tpState, 3.0, 1.0 );

        TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
        TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
        TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
    }
}

void TestCyclicalAging::testAgingCalculation()
{
    double initialCap = 0.8;
    double initialRes = 1.1;
    double steptime = 0.1;
    double exponentCap = 0.8;
    double exponentRes = 0.8;
    double totalCap = 10.0;
    double soc = 50.0;
    double dod = 40.0;
    double voltage = 3.8;

    auto dodValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto voltageValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto socValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto currentValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    std::vector< typename object::ExpressionObject< double >::Parameter > objParams{ { "deltaDOD", dodValueState },
                                                                                     { "meanV", voltageValueState },
                                                                                     { "meanSOC", socValueState },
                                                                                     { "meanI", currentValueState } };
    const auto objCap = boost::make_shared< object::ExpressionObject< double > >( "0.0001 * meanV + 0.000001 * deltaDOD", objParams );
    const auto objRes = boost::make_shared< object::ExpressionObject< double > >( "0.00015 * meanV + 0.0000015 * deltaDOD", objParams );

    aging::CyclicalAging aging( steptime, 0, 0, 1e3, 1e3, objCap, objRes, initialCap, initialRes, true, exponentCap, exponentRes, "Ah" );
    *dodValueState = state::ValueStateWrapper< double >( &aging.mActualDod );
    *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
    *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
    *currentValueState = state::ValueStateWrapper< double >( &aging.mActualCurrent );
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, voltage, 0 );
    auto socState = boost::make_shared< state::Soc >( totalCap, totalCap, soc );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );

    aging.CollectData( tpState, tpState, 3 );
    socState->SetStoredEnergy< state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage + 0.2;
    aging.CollectData( tpState, tpState, 4.2 );
    socState->SetStoredEnergy< state::SocSetFormat::DELTA >( -dod / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage - 0.1;
    aging.CollectData( tpState, tpState, 2.7 );
    socState->SetStoredEnergy< state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage;
    aging.CollectData( tpState, tpState, 6.3 );
    aging.CalculateAging( tpState, 16.2, steptime * 86400 / 16.2 );
    double cap = 0.6599;
    double res = 1.3458;
    double socOffset = 3600 * ( aging.GetCapacityFactor() - initialCap ) * totalCap * soc / 100;

    TS_ASSERT_DELTA( aging.GetCapacityFactor(), cap, 1e-3 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), socOffset, 1e-3 );
    TS_ASSERT_DELTA( aging.GetResistanceFactor(), res, 1e-3 );
}

void TestCyclicalAging::testReset()
{
    double initialCap = 0.8;
    double initialRes = 1.1;
    double steptime = 0.1;
    double exponentCap = 0.8;
    double exponentRes = 0.8;
    double totalCap = 10.0;
    double soc = 50.0;
    double dod = 40.0;
    double voltage = 3.8;

    auto dodValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto voltageValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto socValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto currentValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    std::vector< typename object::ExpressionObject< double >::Parameter > objParams{ { "deltaDOD", dodValueState },
                                                                                     { "meanV", voltageValueState },
                                                                                     { "meanSOC", socValueState },
                                                                                     { "meanI", currentValueState } };
    const auto objCap = boost::make_shared< object::ExpressionObject< double > >( "0.0001 * meanV + 0.000001 * deltaDOD", objParams );
    const auto objRes = boost::make_shared< object::ExpressionObject< double > >( "0.00015 * meanV + 0.0000015 * deltaDOD", objParams );

    aging::CyclicalAging aging( steptime, 0, 0, 1e3, 1e3, objCap, objRes, initialCap, initialRes, true, exponentCap, exponentRes, "Ah" );
    *dodValueState = state::ValueStateWrapper< double >( &aging.mActualDod );
    *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
    *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
    *currentValueState = state::ValueStateWrapper< double >( &aging.mActualCurrent );
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, voltage, 0 );
    auto socState = boost::make_shared< state::Soc >( totalCap, totalCap, soc );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );

    aging.CollectData( tpState, tpState, 3.0 );
    aging.ResetToPointInTime( 0.0 );

    aging.CalculateAging( tpState, 3.0, steptime * 86400 / 3.0 );
    TS_ASSERT_EQUALS( aging.GetCapacityFactor(), initialCap );
    TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
    TS_ASSERT_EQUALS( aging.GetResistanceFactor(), initialRes );

    aging.CollectData( tpState, tpState, 3 );
    socState->SetStoredEnergy< state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage + 0.2;
    aging.CollectData( tpState, tpState, 4.2 );
    socState->SetStoredEnergy< state::SocSetFormat::DELTA >( -dod / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage - 0.1;
    aging.CollectData( tpState, tpState, 2.7 );
    socState->SetStoredEnergy< state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage;
    aging.CollectData( tpState, tpState, 6.3 );
    aging.CalculateAging( tpState, 16.2, steptime * 86400 / 16.2 );
    double cap = 0.6599;
    double res = 1.3458;
    double socOffset = 3600 * ( aging.GetCapacityFactor() - initialCap ) * totalCap * soc / 100;

    TS_ASSERT_DELTA( aging.GetCapacityFactor(), cap, 1e-3 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), socOffset, 1e-3 );
    TS_ASSERT_DELTA( aging.GetResistanceFactor(), res, 1e-3 );
}

void TestCyclicalAging::testFormulaVariables()
{
    double voltage = 3.5;
    double soc = 73; // [%]
    double dod = 43; // [%]
    double capacity = 3600;    // [As]
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, voltage, 0 );
    auto socState = boost::make_shared< state::Soc >( capacity / 3600, capacity / 3600, soc );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );
    auto dodValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto voltageValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto socValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    auto currentValueState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
    std::vector< typename object::ExpressionObject< double >::Parameter > objParams{ { "deltaDOD", dodValueState },
                                                                                     { "meanV", voltageValueState },
                                                                                     { "meanSOC", socValueState },
                                                                                     { "meanI", currentValueState } };
    {
        const auto obj = boost::make_shared< object::ExpressionObject< double > >( "meanV", objParams );
        aging::CyclicalAging aging( 1.0, 0, 0, 1e3, 1e3, obj, obj, 1.0, 1.0, true, 1.0, 1.0, "Ah" );
        *dodValueState = state::ValueStateWrapper< double >( &aging.mActualDod );
        *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
        *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
        *currentValueState = state::ValueStateWrapper< double >( &aging.mActualCurrent );
        aging.CollectData( tpState, tpState, 0.0 );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc - dod );
        aging.CollectData( tpState, tpState, 5.0 );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc );
        aging.CollectData( tpState, tpState, 5.0 );
        aging.CalculateAging( tpState, 10.0, 1.0 );
        TS_ASSERT_DELTA( aging.GetStressFactorCapacity(), voltage, 1e-6 );
        TS_ASSERT_DELTA( aging.GetStressFactorResistance(), voltage, 1e-6 );
    }
    {
        const auto obj = boost::make_shared< object::ExpressionObject< double > >( "meanSOC", objParams );
        aging::CyclicalAging aging( 1.0, 0, 0, 1e3, 1e3, obj, obj, 1.0, 1.0, true, 1.0, 1.0, "Ah" );
        *dodValueState = state::ValueStateWrapper< double >( &aging.mActualDod );
        *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
        *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
        *currentValueState = state::ValueStateWrapper< double >( &aging.mActualCurrent );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc + dod / 2 );
        aging.CollectData( tpState, tpState, 0.0 );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc - dod / 2 );
        aging.CollectData( tpState, tpState, 5.0 );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc );
        aging.CalculateAging( tpState, 5.0, 1.0 );
        TS_ASSERT_DELTA( aging.GetStressFactorCapacity(), soc, 1e-6 );
        TS_ASSERT_DELTA( aging.GetStressFactorResistance(), soc, 1e-6 );
    }
    {
        const auto obj = boost::make_shared< object::ExpressionObject< double > >( "deltaDOD", objParams );
        aging::CyclicalAging aging( 1.0, 0, 0, 1e3, 1e3, obj, obj, 1.0, 1.0, true, 1.0, 1.0, "Ah" );
        *dodValueState = state::ValueStateWrapper< double >( &aging.mActualDod );
        *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
        *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
        *currentValueState = state::ValueStateWrapper< double >( &aging.mActualCurrent );
        aging.CollectData( tpState, tpState, 0.0 );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc - dod );
        aging.CollectData( tpState, tpState, 5.0 );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc );
        aging.CollectData( tpState, tpState, 5.0 );
        aging.CalculateAging( tpState, 10.0, 1.0 );
        TS_ASSERT_DELTA( aging.GetStressFactorCapacity(), dod, 1e-6 );
        TS_ASSERT_DELTA( aging.GetStressFactorResistance(), dod, 1e-6 );
    }
    {
        const auto obj = boost::make_shared< object::ExpressionObject< double > >( "meanI", objParams );
        aging::CyclicalAging aging( 1.0, 0, 0, 1e3, 1e3, obj, obj, 1.0, 1.0, true, 1.0, 1.0, "Ah" );
        *dodValueState = state::ValueStateWrapper< double >( &aging.mActualDod );
        *voltageValueState = state::ValueStateWrapper< double >( &aging.mActualVoltage );
        *socValueState = state::ValueStateWrapper< double >( &aging.mActualSoc );
        *currentValueState = state::ValueStateWrapper< double >( &aging.mActualCurrent );
        double dt = 5.0;
        aging.CollectData( tpState, tpState, 0.0 );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc - dod );
        aging.CollectData( tpState, tpState, dt );
        socState->SetStoredEnergy< state::SocSetFormat::PERCENT >( soc );
        aging.CollectData( tpState, tpState, dt );
        aging.CalculateAging( tpState, 10.0, 1.0 );
        double meanI = dod / 100 * capacity / dt;
        TS_ASSERT_DELTA( aging.GetStressFactorCapacity(), meanI, 1e-6 );
        TS_ASSERT_DELTA( aging.GetStressFactorResistance(), meanI, 1e-6 );
    }
}