#include "TestCyclicalAging.h"
#include "../../aging/cyclical_aging.h"
#include <boost/make_shared.hpp>

void TestCyclicalAging::testNoAging()
{
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >();
    auto socState = boost::make_shared< electrical::state::Soc >( 10.0, 10.0, 50.0 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );
    {    // calculation time of zero seconds
        aging::CyclicalAging aging( 10, 0, 0, "0.0001", "0.0001", 1.0, 1.0, true, 1, 1 );
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
        aging::CyclicalAging aging( 10, 0, 0, "0.0001", "0.0001", 1.0, 1.0, false, 1, 1 );

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
    aging::CyclicalAging aging( steptime, 0, 0, "0.0001 * (meanV + deltaDOD)", "0.00015 * (meanV + deltaDOD)",
                                initialCap, initialRes, true, exponentCap, exponentRes );
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, voltage, 0 );
    auto socState = boost::make_shared< electrical::state::Soc >( totalCap, totalCap, soc );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );

    aging.CollectData( tpState, tpState, 3 );
    socState->SetStoredEnergy< electrical::state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage + 0.2;
    aging.CollectData( tpState, tpState, 4.2 );
    socState->SetStoredEnergy< electrical::state::SocSetFormat::DELTA >( -dod / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage - 0.1;
    aging.CollectData( tpState, tpState, 2.7 );
    socState->SetStoredEnergy< electrical::state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage;
    aging.CollectData( tpState, tpState, 6.3 );
    aging.CalculateAging( tpState, 16.2, steptime * 86400 / 16.2 );
    double cap = 0.6599;
    double res = 1.3458;
    double socOffset = 3600 * ( aging.GetCapacityFactor() - 1.0 ) * totalCap * soc / 100;

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
    aging::CyclicalAging aging( steptime, 0, 0, "0.0001 * (meanV + deltaDOD)", "0.00015 * (meanV + deltaDOD)",
                                initialCap, initialRes, true, exponentCap, exponentRes );
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 0, voltage, 0 );
    auto socState = boost::make_shared< electrical::state::Soc >( totalCap, totalCap, soc );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );

    aging.CollectData( tpState, tpState, 3.0 );
    aging.ResetToPointInTime( 0.0 );

    aging.CalculateAging( tpState, 3.0, steptime * 86400 / 3.0 );
    TS_ASSERT_EQUALS( aging.GetCapacityFactor(), initialCap );
    TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
    TS_ASSERT_EQUALS( aging.GetResistanceFactor(), initialRes );

    aging.CollectData( tpState, tpState, 3 );
    socState->SetStoredEnergy< electrical::state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage + 0.2;
    aging.CollectData( tpState, tpState, 4.2 );
    socState->SetStoredEnergy< electrical::state::SocSetFormat::DELTA >( -dod / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage - 0.1;
    aging.CollectData( tpState, tpState, 2.7 );
    socState->SetStoredEnergy< electrical::state::SocSetFormat::DELTA >( dod / 2 / 100 * totalCap * 3600 );
    electricalData->mVoltageValue = voltage;
    aging.CollectData( tpState, tpState, 6.3 );
    aging.CalculateAging( tpState, 16.2, steptime * 86400 / 16.2 );
    double cap = 0.6599;
    double res = 1.3458;
    double socOffset = 3600 * ( aging.GetCapacityFactor() - 1.0 ) * totalCap * soc / 100;

    TS_ASSERT_DELTA( aging.GetCapacityFactor(), cap, 1e-3 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), socOffset, 1e-3 );
    TS_ASSERT_DELTA( aging.GetResistanceFactor(), res, 1e-3 );
}