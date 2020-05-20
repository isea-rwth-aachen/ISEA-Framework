/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestTwoPortsWithChildren.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 24 Mar 2014 16:43:56 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestTwoPortsWithChildren.h"

#include "../../misc/matrixInclude.h"

#include "../../electrical/capacity.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/paralleltwoport.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/twoport_with_state.h"

#include "../../object/const_obj.h"

#include <boost/make_shared.hpp>


void TestTwoPortsWithChildren::testParallelPort() { electrical::ParallelTwoPort< myMatrixType > pport; }

void TestTwoPortsWithChildren::testSerialPort() { electrical::SerialTwoPort< myMatrixType > sport; }

void TestTwoPortsWithChildren::testParallelPortOperation()
{
    electrical::ParallelTwoPort< myMatrixType > pport;
    TS_ASSERT_EQUALS( pport.GetParallelChildren(), 0 );
    TS_ASSERT_EQUALS( pport.HasChildren(), false );
    TS_ASSERT_THROWS( pport.at( 0 ), const std::out_of_range& );
    TS_ASSERT_EQUALS( pport.HasSystem(), false );
    TS_ASSERT_EQUALS( pport.size(), 0 );
}

void TestTwoPortsWithChildren::testSerialPortOperation()
{
    electrical::SerialTwoPort< myMatrixType > sport;
    TS_ASSERT_EQUALS( sport.HasChildren(), false );
    TS_ASSERT_THROWS( sport.at( 0 ), const std::out_of_range& );
    TS_ASSERT_EQUALS( sport.HasSystem(), false );
    TS_ASSERT_EQUALS( sport.size(), 0 );
}

void TestTwoPortsWithChildren::testParallelPortChildren()
{
    electrical::ParallelTwoPort< myMatrixType > pport;
    electrical::Capacity< myMatrixType >* res = new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( 10 ) );
    pport.AddChild( new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( 10 ) ) );
    pport.AddChild( new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( 10 ) ) );
    pport.AddChild( new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( 10 ) ) );
    pport.AddChild( res );

    TS_ASSERT_EQUALS( pport.size(), 4 );
    TS_ASSERT_EQUALS( pport.GetParallelChildren(), 4 );

#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS_NOTHING( pport.at( 0 ) );
    TS_ASSERT_THROWS_NOTHING( pport.at( 1 ) );
    TS_ASSERT_THROWS_NOTHING( pport.at( 2 ) );
    TS_ASSERT_THROWS_NOTHING( pport.at( 3 ) );
#endif

    TS_ASSERT_THROWS( pport.at( 5 ), const std::out_of_range& );
}

void TestTwoPortsWithChildren::testSerialPortChildren()
{
    electrical::SerialTwoPort< myMatrixType > pport;
    electrical::Capacity< myMatrixType >* res = new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( 10 ) );
    pport.AddChild( new electrical::OhmicResistance< myMatrixType >( new object::ConstObj< double >( 10 ) ) );
    pport.AddChild( new electrical::OhmicResistance< myMatrixType >( new object::ConstObj< double >( 10 ) ) );
    pport.AddChild( new electrical::OhmicResistance< myMatrixType >( new object::ConstObj< double >( 10 ) ) );
    pport.AddChild( res );

    TS_ASSERT_EQUALS( pport.size(), 4 );

#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS_NOTHING( pport.at( 0 ) );
    TS_ASSERT_THROWS_NOTHING( pport.at( 1 ) );
    TS_ASSERT_THROWS_NOTHING( pport.at( 2 ) );
    TS_ASSERT_THROWS_NOTHING( pport.at( 3 ) );
#endif

    TS_ASSERT_THROWS( pport.at( 5 ), std::out_of_range& );
}

void TestTwoPortsWithChildren::testTwoPortWithSoc()
{
#ifndef _SYMBOLIC_
    double capacity = 10.0;
    double initialSoc = 50.0;
    double current = 0.5;
    double timestep = 0.2;

    boost::shared_ptr< electrical::state::Soc > socState( new electrical::state::Soc( capacity, capacity, initialSoc ) );
    boost::scoped_ptr< systm::StateSystemGroup< myMatrixType > > stateSystemGroup( new systm::StateSystemGroup< myMatrixType >() );

    electrical::TwoPortWithState< myMatrixType > port( socState, nullptr );
    port.AddChild( new electrical::OhmicResistance< myMatrixType >( new object::ConstObj< double >( 10.0 ) ) );
    port.SetSystem( stateSystemGroup.get() );
    stateSystemGroup->Initialize();
    port.SetInitialCurrent( current );
    port.UpdateStateSystemGroup();

    TS_ASSERT_DELTA( port.GetSocValue(), initialSoc, 0.0001 );

    stateSystemGroup->mDt = timestep;
    port.CalculateStateDependentValues();

    TS_ASSERT_DELTA( port.GetSocValue(), initialSoc + ( 100 * current * timestep / ( capacity * 3600 ) ), 0.0001 );
#endif
}

void TestTwoPortsWithChildren::testResistanceFactor()
{
#ifndef _SYMBOLIC_
    auto parallel = boost::make_shared< electrical::ParallelTwoPort< myMatrixType > >();
    auto serial = boost::make_shared< electrical::SerialTwoPort< myMatrixType > >();
    auto resistance =
     boost::make_shared< electrical::OhmicResistance< myMatrixType > >( boost::make_shared< object::ConstObj< double > >( 10.0 ) );
    parallel->AddChild( serial );
    serial->AddChild( resistance );
    TS_ASSERT_EQUALS( resistance->GetValue(), 10.0 );
    parallel->SetResistanceFactor( 3 );
    TS_ASSERT_EQUALS( resistance->GetValue(), 30.0 );
#endif
}
