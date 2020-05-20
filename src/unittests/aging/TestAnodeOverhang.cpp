#include "TestAnodeOverhang.h"
#include "../../aging/anode_overhang.h"
#include "../../object/const_obj.h"

void TestAnodeOverhang::testNoAging()
{
    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 1.0, -3.0, 0.5 );
    auto socState = boost::make_shared< electrical::state::Soc >( 10.0, 10.0, 50.0 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );
    auto voltageObj = boost::make_shared< object::ConstObj< double > >( 3.0 );
    auto overhangSoc = boost::make_shared< electrical::state::Soc >( 3.0, 3.0, 50.0 );
    double tauCoef = 5.0;
    double activCoef = 2.3;
    {    // aging is disabled
        aging::AnodeOverhang aging( 10.0, voltageObj, activCoef, tauCoef, overhangSoc, 60.0, false );
        aging.CalculateAging( tpState, 10.0, 1.0 );
        TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
        TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
        TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
    }
    {    // no aging data collected
        aging::AnodeOverhang aging( 10.0, voltageObj, activCoef, tauCoef, overhangSoc, 50.0, true );
        aging.CalculateAging( tpState, 10.0, 1.0 );
        TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
        TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
        TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
    }
    {    // anode and overhang have the same voltage
        aging::AnodeOverhang aging( 10.0, voltageObj, activCoef, tauCoef, overhangSoc, 50.0, true );
        aging.CollectData( tpState, tpState, 10.0 );
        aging.CalculateAging( tpState, 10.0, 1.0 );
        TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
        TS_ASSERT_EQUALS( aging.GetSocOffset(), 0.0 );
        TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
    }
}

void TestAnodeOverhang::testInitialOffset()
{
    double socWithoutOffset = 40.0;
    double initialSoc = 60.0;
    double overhangCapacity = 3.0;
    auto voltageObj = boost::make_shared< object::ConstObj< double > >( 3.0 );
    auto overhangSoc = boost::make_shared< electrical::state::Soc >( overhangCapacity, overhangCapacity, initialSoc );
    double tauCoef = 5.0;
    double activCoef = 2.3;
    aging::AnodeOverhang aging( 10.0, voltageObj, activCoef, tauCoef, overhangSoc, socWithoutOffset, true );
    TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), ( socWithoutOffset - initialSoc ) / 100 * overhangCapacity * 3600, 1.0e-5 );
    TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
}

void TestAnodeOverhang::testAgingCalculation()
{
    double socWithoutOffset = 40.0;
    double initialSoc = 60.0;
    double overhangCapacity = 3.0;
    auto voltageObj = boost::make_shared< object::ConstObj< double > >( 3.0 );
    auto overhangSoc = boost::make_shared< electrical::state::Soc >( overhangCapacity, overhangCapacity, initialSoc );
    double tauCoef = 5.0;
    double activCoef = 2.3;
    double offset = ( socWithoutOffset - initialSoc ) / 100 * overhangCapacity * 3600;
    aging::AnodeOverhang aging( 10.0, voltageObj, activCoef, tauCoef, overhangSoc, socWithoutOffset, true );

    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 1.0, -2.7, 0.5 );
    auto socState = boost::make_shared< electrical::state::Soc >( 10.0, 10.0, 50.0 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );

    aging.CollectData( tpState, tpState, 3.0 );
    tpState.mElectricalData->mVoltageValue = -2.1;
    tpState.mThermalState = boost::make_shared< state::ThermalState< double > >( 23.0 );
    aging.CollectData( tpState, tpState, 6.2 );
    aging.CalculateAging( tpState, 3.0, 10.0 * 86400 / 3.0 );

    double avgTemp = ( 20.0 * 3.0 + 23.0 * 6.2 ) / ( 3.0 + 6.2 ) + 273.15;
    double avgVoltage = ( 2.7 * 3.0 + 2.1 * 6.2 ) / ( 3.0 + 6.2 );
    double tau = tauCoef * std::exp( activCoef / avgTemp ) * 3600 * 24;
    double overhangCap = ( initialSoc / 100 ) * ( overhangCapacity * 3600 ) / voltageObj->GetValue();
    offset += overhangCap * ( avgVoltage - voltageObj->GetValue() ) * ( 1 - exp( -10.0 * 3600 * 24 / tau ) );

    TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), offset, 1.0e-5 );
    TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
}

void TestAnodeOverhang::testReset()
{
    double socWithoutOffset = 40.0;
    double initialSoc = 60.0;
    double overhangCapacity = 3.0;
    auto voltageObj = boost::make_shared< object::ConstObj< double > >( 3.0 );
    auto overhangSoc = boost::make_shared< electrical::state::Soc >( overhangCapacity, overhangCapacity, initialSoc );
    double tauCoef = 5.0;
    double activCoef = 2.3;
    double offset = ( socWithoutOffset - initialSoc ) / 100 * overhangCapacity * 3600;
    aging::AnodeOverhang aging( 10.0, voltageObj, activCoef, tauCoef, overhangSoc, socWithoutOffset, true );

    auto electricalData = boost::make_shared< ElectricalDataStruct< double > >( 1.0, -2.7, 0.5 );
    auto socState = boost::make_shared< electrical::state::Soc >( 10.0, 10.0, 50.0 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    aging::TwoportState tpState( electricalData, socState, thermalState );

    aging.CollectData( tpState, tpState, 3.0 );
    aging.ResetToPointInTime( 0.0 );

    aging.CalculateAging( tpState, 3.0, 10.0 * 86400 / 3.0 );
    TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), offset, 10e-5 );
    TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );

    aging.CollectData( tpState, tpState, 3.0 );
    tpState.mElectricalData->mVoltageValue = -2.1;
    tpState.mThermalState = boost::make_shared< state::ThermalState< double > >( 23.0 );
    aging.CollectData( tpState, tpState, 6.2 );
    tpState.mElectricalData->mVoltageValue = -2.5;
    tpState.mThermalState = boost::make_shared< state::ThermalState< double > >( 19.0 );
    aging.CollectData( tpState, tpState, 4.7 );
    aging.ResetToPointInTime( 9.2 );
    aging.CalculateAging( tpState, 9.2, 10.0 * 86400 / 9.2 );

    double avgTemp = ( 20.0 * 3.0 + 23.0 * 6.2 ) / ( 3.0 + 6.2 ) + 273.15;
    double avgVoltage = ( 2.7 * 3.0 + 2.1 * 6.2 ) / ( 3.0 + 6.2 );
    double tau = tauCoef * std::exp( activCoef / avgTemp ) * 3600 * 24;
    double overhangCap = ( initialSoc / 100 ) * ( overhangCapacity * 3600 ) / voltageObj->GetValue();
    offset += overhangCap * ( avgVoltage - voltageObj->GetValue() ) * ( 1 - exp( -10.0 * 3600 * 24 / tau ) );

    TS_ASSERT_EQUALS( aging.GetCapacityFactor(), 1.0 );
    TS_ASSERT_DELTA( aging.GetSocOffset(), offset, 1.0e-5 );
    TS_ASSERT_EQUALS( aging.GetResistanceFactor(), 1.0 );
}