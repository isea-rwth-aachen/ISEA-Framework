/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestElectricalElement.cpp
* Creation Date : 21-02-2014
* Last Modified : Di 20 Jun 2017 17:52:21 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestElectricalElement.h"


#include "../../electrical/capacity.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/voltagesource.h"

#include "../../object/const_obj.h"
#include "../../object/lookup_obj1d.h"
#include "../../object/lookup_obj2d.h"

static const double sDelta = 0.000001;


void TestElectricalElement::testElectricalElementOhmicConst()
{
    double Resistance( 10.0 );

    boost::shared_ptr< object::Object< double > > obj( new object::ConstObj< double >( Resistance ) );
    electrical::OhmicResistance< myMatrixType > test( obj );
    myMatrixType current;
    current.resize( 1, 2 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 1 ) = 10;
    test.SetCurrent( current );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * Resistance );
    TS_ASSERT( obj.get() == test() );
    TS_ASSERT( obj.get() == test.GetElementObject() );
#else
    current.coeffRef( 0, 1 ) = ScalarUnit( "InputCurrent" );
    test.SetCurrent( current );
    TS_ASSERT( ( *test.GetVoltage() ).coeff( 0, 0 ).IsEmpty() );

    ScalarUnit left = ( *test.GetVoltage() ).coeff( 0, 1 );
    ScalarUnit right = current.coeff( 0, 1 ) * ScalarUnit( "ID0_ObjR" );
    TS_ASSERT( left == right );
    TS_ASSERT( obj.get() == test() );
    TS_ASSERT( obj.get() == test.GetElementObject() );
#endif
}

void TestElectricalElement::testElectricalElementOhmic1DLookup()
{
    const double mPointer[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector< double > measurementpoint;
    measurementpoint.insert( measurementpoint.begin(), mPointer, mPointer + 9 );

    const double dPointer[] = {0.1, 1.2, 9.3, 1.4, 5, 6, 7, 8, 9};
    std::vector< double > data;
    data.insert( data.begin(), dPointer, dPointer + 9 );

    electrical::OhmicResistance< myMatrixType > test( new object::LookupObj1D< double >( data, measurementpoint ) );
    myMatrixType current;
    current.resize( 1, 2 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.00001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * 0.1, 0.00001 );

    test()->GetValue( 2.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.00001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * 1.2, 0.000001 );

    test()->GetValue( 8.5 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.00001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * 8.5, 0.00001 );
#else
    test.SetID( 23 );
    current.coeffRef( 0, 1 ) = ScalarUnit( "InputCurrent" );
    test.SetCurrent( current );
    TS_ASSERT( ( *test.GetVoltage() ).coeff( 0, 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * ScalarUnit( "ID23_ObjR" ) );
#endif
}

void TestElectricalElement::testElectricalElementVoltageSourceConst()
{
    double voltage = 10;
    electrical::VoltageSource< myMatrixType > test( new object::ConstObj< double >( voltage ) );
    myMatrixType current;
    current.resize( 1, 10 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 1 ) = 10;
#else
    current.coeffRef( 0, 1 ) = ScalarUnit( "InputCurrent" );
#endif
    test.SetCurrent( current );
}

void TestElectricalElement::testElectricalElementOhmic2DLookup()
{
    double const mpRow[] = {1, 2, 3};
    std::vector< double > measurementpointRow( mpRow, mpRow + 3 );

    double const mpCol[] = {1, 2, 3};
    std::vector< double > measurementpointColumn( mpCol, mpCol + 3 );

    double const mpData[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    std::vector< std::vector< double > > data( 3, std::vector< double >( 3, 0 ) );
    for ( size_t i = 0; i < 3; ++i )
        data.at( i ).assign( mpData[i], mpData[i] + 3 );

    electrical::OhmicResistance< myMatrixType > test(
     new object::LookupObj2D< double >( data, measurementpointRow, measurementpointColumn ) );
    myMatrixType current;
    current.resize( 1, 2 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0, 1.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * 1 );

    test()->GetValue( 2.0, 2.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * 5.0 );

    test()->GetValue( 3.0, 3.0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 0 ), 0 );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 1 ), current.coeff( 0, 1 ) * 9.0 );
#endif
}

void TestElectricalElement::testElectricalElementVoltageSource1DLookup()
{
    const double mp[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector< double > measurementpoint( mp, mp + 9 );
    const double mpData[] = {0.1, 1.2, 9.3, 1.4, 5, 6, 7, 8, 9};
    std::vector< double > data( mpData, mpData + 9 );

    electrical::VoltageSource< myMatrixType > test( new object::LookupObj1D< double >( data, measurementpoint ) );
    myMatrixType current;
    current.resize( 1, 10 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 9 ), 0.1, 0.001 );

    test()->GetValue( 2.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 9 ), 1.2, 0.001 );

    test()->GetValue( 8.5 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 9 ), 8.5, 0.001 );
#else
    test.SetID( 12 );
    current.coeffRef( 0, 1 ) = ScalarUnit( "InputCurrent" );
    test.SetCurrent( current );
    TS_ASSERT( ( *test.GetVoltage() ).coeff( 0, 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 9 ), ScalarUnit( "ID12_ObjV" ) );
#endif
}

void TestElectricalElement::testElectricalElementVoltageSource2DLookup()
{
    double const mpRow[] = {1, 2, 3};
    std::vector< double > measurementpointRow( mpRow, mpRow + 3 );

    double const mpCol[] = {1, 2, 3};
    std::vector< double > measurementpointColumn( mpCol, mpCol + 3 );

    double const mpData[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    std::vector< std::vector< double > > data( 3, std::vector< double >( 3, 0 ) );
    for ( size_t i = 0; i < 3; ++i )
        data.at( i ).assign( mpData[i], mpData[i] + 3 );

    electrical::VoltageSource< myMatrixType > test(
     new object::LookupObj2D< double >( data, measurementpointRow, measurementpointColumn ) );
    myMatrixType current;
    current.resize( 1, 10 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 1 ) = 10;
    test.SetCurrent( current );
    test()->GetValue( 1.0, 1.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 9 ), 1, 0.001 );

    test()->GetValue( 2.0, 2.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 9 ), 5.0, 0.001 );

    test()->GetValue( 3.0, 3.0 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 0, 0.001 );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 9 ), 9.0, 0.001 );
#endif
}

void TestElectricalElement::testElectricalParllelRCCreation()
{
    double Resistance( 10.0 );
    double Capacity( 0.1 );

    boost::shared_ptr< object::Object< double > > rObj( new object::ConstObj< double >( Resistance ) );
    boost::shared_ptr< object::Object< double > > tauObj( new object::ConstObj< double >( Capacity ) );
    electrical::ParallelRC< myMatrixType > test( rObj, tauObj );

    TS_ASSERT( rObj.get() == test() );
    TS_ASSERT( tauObj.get() == test.GetTauObject() );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    test.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( 9 );


#ifndef _SYMBOLIC_
    myMatrixType current;
    current.resize( 1, 10 );
    current.setZero();
    current.coeffRef( 0, 1 ) = 5.4;

    test.SetID( 12 );
    test.SetCurrent( current );
    TS_ASSERT_DELTA( ( *test.GetVoltage() ).coeff( 0, 0 ), 1.0, sDelta );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().coeff( 0, 1 ), 5.4 * Resistance / Capacity, sDelta );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().coeff( 0, 0 ), -1.0 / Capacity, sDelta );

    TS_ASSERT_EQUALS( test.GetTauValue(), Capacity );
#else
    myMatrixType current;
    current.resize( 1, 10 );
    current.setZero();
    current.coeffRef( 0, 1 ) = ScalarUnit( "InputCurrent" );

    test.SetID( 12 );
    test.SetCurrent( current );
    TS_ASSERT_EQUALS( ( *test.GetVoltage() ).coeff( 0, 0 ), ScalarUnit( 1.0 ) );
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().coeff( 0, 1 ),
                      ScalarUnit( "DIV(MUL(InputCurrent,ID12_ObjR),ID12_ObjTau)" ) );
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().coeff( 0, 0 ),
                      ScalarUnit( "NEG(DIV(1.000000,ID12_ObjTau))" ) );

    TS_ASSERT_EQUALS( test.GetTauValue(), ScalarUnit( "ID12_ObjTau" ) );
#endif
}

void TestElectricalElement::testElectricalElementOhmPower()
{
    double Resistance( 10.0 );
    myMatrixType current;
    current.resize( 1, 2 );
    current.setZero();
    current.coeffRef( 0, 0 ) = 1;
    current.coeffRef( 0, 1 ) = 1;

    //        myMatrixType state(2,1);
    //        state(0,0) = 2;
    //        state(1,0) = -0.5;

    electrical::OhmicResistance< myMatrixType > test( new object::ConstObj< double >( Resistance ) );
    test.SetCurrent( current );
    test.GetVoltage();
    //        TS_ASSERT_DELTA(test.GetPower(), 22.5, 0.0001)
}

void TestElectricalElement::testElectricalElementCapPower()
{
    double Capacity( 10.0 );
    myMatrixType current;
    current.resize( 1, 2 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 0 ) = 1;
    current.coeffRef( 0, 1 ) = 1;
#else
    current.coeffRef( 0, 0 ) = ScalarUnit( "Var0" );
    current.coeffRef( 0, 1 ) = ScalarUnit( "Var1" );
#endif

    //        myMatrixType state(2,1);
    //        state(0,0) = 2;
    //        state(1,0) = -0.5;

    electrical::Capacity< myMatrixType > test( new object::ConstObj< double >( Capacity ) );
    test.SetCurrent( current );
    test.GetVoltage();
    //        TS_ASSERT_DELTA(test.GetPower(), 0, 0.0001)
}

void TestElectricalElement::testElectricalElementParallelRCPower()
{
    double Resistance( 10.0 );
    myMatrixType current;
    current.resize( 1, 2 );
    current.setZero();

#ifndef _SYMBOLIC_
    current.coeffRef( 0, 0 ) = 1;
    current.coeffRef( 0, 1 ) = 1;
#else
    current.coeffRef( 0, 0 ) = ScalarUnit( "Var0" );
    current.coeffRef( 0, 1 ) = ScalarUnit( "Var1" );
#endif

    //        myMatrixType state(2,1);
    //        state(0,0) = 2;
    //        state(1,0) = -0.5;

    electrical::ParallelRC< myMatrixType > test( new object::ConstObj< double >( Resistance ),
                                                 new object::ConstObj< double >( 1.0 / Resistance ) );
    test.SetCurrent( current );
    test.GetVoltage();
    //        TS_ASSERT_DELTA(test.GetPower(), 0.4, 0.0001)
}

void TestElectricalElement::testCellelementWithReversibleHeat()
{
    const double Temp( 47 );
    const ScalarUnit current = 10;
    boost::shared_ptr< state::ThermalState< double > > thermalState( new state::ThermalState< double >( Temp ) );
    boost::shared_ptr< state::Soc > socState( new state::Soc( 1.0, 1.0, 50 ) );

    std::vector< double > lookupData = {-0.000996075, -0.000594878, -0.000098689, -0.000067029, -0.000132195,
                                        -0.000198293, -0.000166013, 0.000066405,  0.000166788,  0.000199215,
                                        0.000230275,  0.000232418,  0.000198293,  0.000099608,  0.000033049,
                                        0.000032896,  0.000033049,  0.000099146,  0.000100073,  0.000099608,
                                        0.000099146,  0.000132810,  0.00012874};

    std::vector< double > measurementPoints = {-10, -5, 0,  5,  10, 15, 20, 25, 30, 35, 40, 45,
                                               50,  55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
    auto reversibleHeat = boost::make_shared< object::LookupObj1dWithState< double > >( lookupData, measurementPoints,
                                                                                        socState, lookup::LookupType( 0 ) );

    electrical::Cellelement< myMatrixType > test( 0, thermalState, socState, true,
                                                  electrical::TwoPort< myMatrixType >::DataType( new ElectricalDataStruct< ScalarUnit > ),
                                                  reversibleHeat );

    TS_ASSERT_EQUALS( test.GetThermalState().get(), thermalState.get() );
    TS_ASSERT_EQUALS( test.GetSoc().get(), socState.get() );

    TS_ASSERT_EQUALS( test.HasReversibleHeat(), true );
    test.mCurrentValue = current;
#ifdef _SYMBOLIC_
    TS_ASSERT_EQUALS( test.CalculateReversibleHeat(), current * ( Temp + 273.15 ) * 0.000198293 );
#else
    TS_ASSERT_DELTA( test.CalculateReversibleHeat(), current * ( Temp + 273.15 ) * 0.000198293, 0.0000001 );
#endif
}

void TestElectricalElement::testCellelementWithoutReverisbleHeat()
{
    const double Temp( 47 );
    const ScalarUnit current = 10;
    boost::shared_ptr< state::ThermalState< double > > thermalState( new state::ThermalState< double >( Temp ) );
    boost::shared_ptr< state::Soc > socState( new state::Soc( 1.0, 1.0, 50 ) );

    electrical::Cellelement< myMatrixType > test( thermalState, socState, true,
                                                  electrical::TwoPort< myMatrixType >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    TS_ASSERT_EQUALS( test.GetThermalState().get(), thermalState.get() );
    TS_ASSERT_EQUALS( test.GetSoc().get(), socState.get() );

    TS_ASSERT_EQUALS( test.HasReversibleHeat(), false );
    test.mCurrentValue = current;
#ifdef _SYMBOLIC_
    TS_ASSERT_EQUALS( test.CalculateReversibleHeat(), 0 );
#else
    TS_ASSERT_DELTA( test.CalculateReversibleHeat(), 0, 0.0000001 );
#endif
}

void TestElectricalElement::testTotalCapacitySingleCell()
{
    const double Temp( 47 );
    const double capacityInAh = 1.0;
    boost::shared_ptr< state::ThermalState< double > > thermalState( new state::ThermalState< double >( Temp ) );
    boost::shared_ptr< state::Soc > socState( new state::Soc( 1.0, capacityInAh, 50 ) );

    electrical::Cellelement< myMatrixType > test( thermalState, socState, true,
                                                  electrical::TwoPort< myMatrixType >::DataType( new ElectricalDataStruct< ScalarUnit > ) );

    TS_ASSERT_DELTA( test.GetTotalCapacity(), capacityInAh, 0.00001 );
}

void TestElectricalElement::testTotalCapacitySerial()
{
    const double Temp( 47 );
    const double capacityInAh = 1.0;
    boost::shared_ptr< state::ThermalState< double > > thermalState( new state::ThermalState< double >( Temp ) );
    boost::shared_ptr< state::Soc > socState( new state::Soc( 1.0, capacityInAh, 50 ) );

    // Serial twoport
    electrical::SerialTwoPort< myMatrixType > sport;

    // Create two cellelements
    auto test = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState, true,
                                                                               electrical::TwoPort< myMatrixType >::DataType(
                                                                                new ElectricalDataStruct< ScalarUnit > ) );

    auto test2 = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState, true,
                                                                                electrical::TwoPort< myMatrixType >::DataType(
                                                                                 new ElectricalDataStruct< ScalarUnit > ) );


    /// connect cells in series
    sport.AddChild( test );
    sport.AddChild( test2 );

    TS_ASSERT_DELTA( sport.GetTotalCapacity(), capacityInAh, 0.00001 );    // Expected behavior
}

void TestElectricalElement::testTotalCapacitySerialUnequalCapacity()
{
    const double Temp( 47 );
    const double capacityInAh = 1.0;
    const double capacityInAh2 = 0.9;
    boost::shared_ptr< state::ThermalState< double > > thermalState( new state::ThermalState< double >( Temp ) );
    boost::shared_ptr< state::Soc > socState( new state::Soc( 1.0, capacityInAh, 50 ) );
    boost::shared_ptr< state::Soc > socState2( new state::Soc( 0.9, capacityInAh2, 50 ) );

    // Serial twoport
    electrical::SerialTwoPort< myMatrixType > sport;

    // Create two cellelements
    auto test = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState, true,
                                                                               electrical::TwoPort< myMatrixType >::DataType(
                                                                                new ElectricalDataStruct< ScalarUnit > ) );

    auto test2 = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState2, true,
                                                                                electrical::TwoPort< myMatrixType >::DataType(
                                                                                 new ElectricalDataStruct< ScalarUnit > ) );


    /// connect cells in series
    sport.AddChild( test );
    sport.AddChild( test2 );

    TS_ASSERT_DELTA( sport.GetTotalCapacity(), capacityInAh2, 0.00001 );    // Expected behavior
}

void TestElectricalElement::testTotalCapacityParallelCapacity()
{
    const double Temp( 47 );
    const double capacityInAh = 1.0;
    const double capacityInAh2 = 0.9;
    const double totalCapacityinAh = capacityInAh + capacityInAh2;
    boost::shared_ptr< state::ThermalState< double > > thermalState( new state::ThermalState< double >( Temp ) );
    boost::shared_ptr< state::Soc > socState( new state::Soc( 1.0, capacityInAh, 50 ) );
    boost::shared_ptr< state::Soc > socState2( new state::Soc( 0.9, capacityInAh2, 50 ) );

    // Serial twoport
    electrical::ParallelTwoPort< myMatrixType > pport;

    // Create two cellelements
    auto test = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState, true,
                                                                               electrical::TwoPort< myMatrixType >::DataType(
                                                                                new ElectricalDataStruct< ScalarUnit > ) );

    auto test2 = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState2, true,
                                                                                electrical::TwoPort< myMatrixType >::DataType(
                                                                                 new ElectricalDataStruct< ScalarUnit > ) );


    /// connect cells in series
    pport.AddChild( test );
    pport.AddChild( test2 );

    TS_ASSERT_DELTA( pport.GetTotalCapacity(), totalCapacityinAh, 0.00001 );    // Expected behavior
}

void TestElectricalElement::testTotalCapacitySerialChildWithoutCapacity()
{
    const double Temp( 47 );
    const double capacityInAh = 1.0;
    const double capacityInAh2 = 0.9;
    boost::shared_ptr< state::ThermalState< double > > thermalState( new state::ThermalState< double >( Temp ) );
    boost::shared_ptr< state::Soc > socState( new state::Soc( 1.0, capacityInAh, 50 ) );
    boost::shared_ptr< state::Soc > socState2( new state::Soc( 0.9, capacityInAh2, 50 ) );
    boost::shared_ptr< object::ConstObj< double > > obj( new object::ConstObj< double >( 10.0 ) );

    // Serial twoport
    electrical::SerialTwoPort< myMatrixType > sport;

    // Create two cellelements
    auto test = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState, true,
                                                                               electrical::TwoPort< myMatrixType >::DataType(
                                                                                new ElectricalDataStruct< ScalarUnit > ) );

    auto test2 = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState2, true,
                                                                                electrical::TwoPort< myMatrixType >::DataType(
                                                                                 new ElectricalDataStruct< ScalarUnit > ) );

    // Create ohmic resistance
    auto test3 = boost::make_shared< electrical::OhmicResistance< myMatrixType > >( obj, true );

    // connect cells in series
    sport.AddChild( test );
    sport.AddChild( test3 );
    sport.AddChild( test2 );

    TS_ASSERT_DELTA( sport.GetTotalCapacity(), capacityInAh2, 0.00001 );    // Expected behavior
}
