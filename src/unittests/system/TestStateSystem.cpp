/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestStateSystem.cpp
* Creation Date : 21-02-2014
* Last Modified : Di 20 Jun 2017 17:58:41 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestStateSystem.h"
// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../../system/stateSystemGroup.h"

#include "../../electrical/capacity.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/twoport.h"
#include "../../electrical/voltagesource.h"

using namespace electrical;

void TestStateSystem::testSystemWithCapacity()
{

    double Resistance( 10.0 );
    boost::scoped_ptr< electrical::Capacity< myMatrixType > > res(
     new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( Resistance ) ) );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    res->SetSystem( &stateSystemGroup );
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationCount(), 1 )
}

void TestStateSystem::testSystemWithParallelPort()
{
    std::vector< electrical::TwoPort< myMatrixType >* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(
     CreateSampleNetwork< electrical::Capacity< myMatrixType > >( twoPortVector ) );
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    rootPort->SetInitialCurrent();
    rootPort->UpdateStateSystemGroup();

    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationCount(), 6 );
    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationCount(), 5 );

    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().cols(),
                      stateSystemGroup.mDglStateSystem.GetEquationCount() + stateSystemGroup.mAlgStateSystem.GetEquationCount() );
    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().rows(),
                      stateSystemGroup.mDglStateSystem.GetEquationCount() );
    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix().rows(),
                      stateSystemGroup.mAlgStateSystem.GetEquationCount() );
}

void TestStateSystem::testSystemWithSerialPort()
{

    double Resistance( 10.0 );

    boost::scoped_ptr< electrical::SerialTwoPort< myMatrixType > > serial( new electrical::SerialTwoPort< myMatrixType > );

    electrical::Capacity< myMatrixType >* res( new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( Resistance ) ) );

    electrical::Capacity< myMatrixType >* res1(
     new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( Resistance ) ) );

    electrical::Capacity< myMatrixType >* res2(
     new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( Resistance ) ) );

    serial->AddChild( res );
    serial->AddChild( res1 );
    serial->AddChild( res2 );

    electrical::SerialTwoPort< myMatrixType >* serial2( new electrical::SerialTwoPort< myMatrixType >() );

    res = new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( Resistance ) );
    res1 = new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( Resistance ) );
    res2 = new electrical::Capacity< myMatrixType >( new object::ConstObj< double >( Resistance ) );

    serial2->AddChild( res );
    serial2->AddChild( res1 );
    serial2->AddChild( res2 );

    serial->AddChild( serial2 );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    serial->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();

    TS_ASSERT_EQUALS( stateSystemGroup.mDglStateSystem.GetEquationCount(), 6 );
    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationCount(), 0 );
}

void TestStateSystem::testDaeSystemWithParallelPortCurrentOhmic()
{
    std::vector< electrical::TwoPort< myMatrixType >* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(
     CreateSampleNetwork< electrical::OhmicResistance< myMatrixType > >( twoPortVector ) );
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();

#ifndef _SYMBOLIC_
    double Current( 10.0 );
    rootPort->SetInitialCurrent( Current );
#else
    rootPort->SetInitialCurrent( ScalarUnit( "InputCurrent" ) );
#endif
    rootPort->UpdateStateSystemGroup();

    for ( size_t i = 0; i < twoPortVector.size(); ++i )
    {
        TS_ASSERT_EQUALS( twoPortVector.at( i )->mCurrent.cols(), ( stateSystemGroup.GetStateCount() + 1 ) );
        TS_ASSERT_EQUALS( twoPortVector.at( i )->mCurrent.rows(), 1 );
    }
    /*
       -1 -1 -1 0 0 10
       1 0 0 0 0 0
       0 1 0 0 0 0
       0 0 1 -1 -1 0
       0 0 0 1 0 0
       0 0 0 0 1 0
       0 0 1 0 0 0
       0 0 0 0 0 10
       */


#ifndef _SYMBOLIC_
    for ( size_t i = 0; i < 3; ++i )
        TS_ASSERT_EQUALS( twoPortVector.at( 0 )->mCurrent.coeff( 0, i ), -1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 0 )->mCurrent.coeff( 0, 5 ), 10 );


    TS_ASSERT_EQUALS( twoPortVector.at( 1 )->mCurrent.coeff( 0, 0 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 2 )->mCurrent.coeff( 0, 1 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, 2 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, 3 ), -1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, 4 ), -1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 4 )->mCurrent.coeff( 0, 3 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 5 )->mCurrent.coeff( 0, 4 ), 1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 6 )->mCurrent.coeff( 0, 2 ), 1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 7 )->mCurrent.coeff( 0, 5 ), Current );
#else
    for ( size_t i = 0; i < 3; ++i )
        TS_ASSERT_EQUALS( twoPortVector.at( 0 )->mCurrent.coeff( 0, i ), ScalarUnit( "-1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 0 )->mCurrent.coeff( 0, 5 ), ScalarUnit( "InputCurrent" ) );


    TS_ASSERT_EQUALS( twoPortVector.at( 1 )->mCurrent.coeff( 0, 0 ), ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 2 )->mCurrent.coeff( 0, 1 ), ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, 2 ), ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, 3 ), ScalarUnit( "-1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, 4 ), ScalarUnit( "-1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 4 )->mCurrent.coeff( 0, 3 ), ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 5 )->mCurrent.coeff( 0, 4 ), ScalarUnit( "1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 6 )->mCurrent.coeff( 0, 2 ), ScalarUnit( "1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 7 )->mCurrent.coeff( 0, 5 ), ScalarUnit( "InputCurrent" ) );
#endif
}

void TestStateSystem::testDaeSystemWithParallelPortCurrentCapacity()
{
    std::vector< electrical::TwoPort< myMatrixType >* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(
     CreateSampleNetwork< electrical::Capacity< myMatrixType > >( twoPortVector ) );
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();

#ifndef _SYMBOLIC_
    double Current( 10.0 );
    rootPort->SetInitialCurrent( Current );
#else
    rootPort->SetInitialCurrent( ScalarUnit( "InputCurrent" ) );
#endif
    rootPort->UpdateStateSystemGroup();

    for ( size_t i = 0; i < twoPortVector.size(); ++i )
    {
        TS_ASSERT_EQUALS( twoPortVector.at( i )->mCurrent.cols(), stateSystemGroup.GetStateCount() + 1 );
        TS_ASSERT_EQUALS( twoPortVector.at( i )->mCurrent.rows(), 1 );
    }

#ifndef _SYMBOLIC_
    TS_ASSERT_EQUALS( twoPortVector.at( 1 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 0 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 2 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 1 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ), -1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ), -1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 4 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 5 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ), 1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 6 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ), 1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 7 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 5 ), Current );
#else
    TS_ASSERT_EQUALS( twoPortVector.at( 1 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 0 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 2 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 1 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ),
                      ScalarUnit( "-1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ),
                      ScalarUnit( "-1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 4 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 5 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ),
                      ScalarUnit( "1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 6 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ),
                      ScalarUnit( "1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 7 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 5 ),
                      ScalarUnit( "InputCurrent" ) );
#endif
}

void TestStateSystem::testDaeSystemWithParallelPortCurrentVoltage()
{
    std::vector< electrical::TwoPort< myMatrixType >* > twoPortVector;
    boost::scoped_ptr< electrical::TwoPortWithChild< myMatrixType > > rootPort(
     CreateSampleNetwork< electrical::VoltageSource< myMatrixType > >( twoPortVector ) );
    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();

#ifndef _SYMBOLIC_
    double Current( 10.0 );
    rootPort->SetInitialCurrent( Current );
#else
    rootPort->SetInitialCurrent( ScalarUnit( "InputCurrent" ) );
#endif
    rootPort->UpdateStateSystemGroup();

    for ( size_t i = 0; i < twoPortVector.size(); ++i )
    {
        TS_ASSERT_EQUALS( twoPortVector.at( i )->mCurrent.cols(), stateSystemGroup.GetStateCount() + 1 );
        TS_ASSERT_EQUALS( twoPortVector.at( i )->mCurrent.rows(), 1 );
    }

#ifndef _SYMBOLIC_
    TS_ASSERT_EQUALS( twoPortVector.at( 1 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 0 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 2 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 1 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ), -1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ), -1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 4 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ), 1 );
    TS_ASSERT_EQUALS( twoPortVector.at( 5 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ), 1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 6 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ), 1 );

    TS_ASSERT_EQUALS( twoPortVector.at( 7 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 5 ), Current );
#else
    TS_ASSERT_EQUALS( twoPortVector.at( 1 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 0 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 2 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 1 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ),
                      ScalarUnit( "-1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 3 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ),
                      ScalarUnit( "-1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 4 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 3 ),
                      ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( twoPortVector.at( 5 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 4 ),
                      ScalarUnit( "1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 6 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 2 ),
                      ScalarUnit( "1.000000" ) );

    TS_ASSERT_EQUALS( twoPortVector.at( 7 )->mCurrent.coeff( 0, stateSystemGroup.mDglStateSystem.GetEquationCount() + 5 ),
                      ScalarUnit( "InputCurrent" ) );
#endif
}

void TestStateSystem::TestMixedSystem()
{
#ifndef _SYMBOLIC_
#else
    // Attention: This must look like TestDaeSystem::testDaeSystemMixedSystemCurrentSolveable()
    boost::scoped_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );

    boost::shared_ptr< electrical::ParallelTwoPort<> > p1( new electrical::ParallelTwoPort<>() );
    parallel->AddChild( p1 );

    boost::shared_ptr< electrical::SerialTwoPort<> > s11( new electrical::SerialTwoPort<>() );
    boost::shared_ptr< electrical::SerialTwoPort<> > s12( new electrical::SerialTwoPort<>() );
    p1->AddChild( s11 );
    p1->AddChild( s12 );

    s11->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 10 ) ) );
    s11->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 2 ) ) );
    s12->AddChild( new electrical::Capacity<>( new object::ConstObj< double >( 5 ) ) );
    s12->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 2 ) ) );

    boost::shared_ptr< electrical::SerialTwoPort<> > s2( new electrical::SerialTwoPort<>() );
    parallel->AddChild( s2 );

    s2->AddChild( new electrical::OhmicResistance<>( new object::ConstObj< double >( 3 ) ) );
    s2->AddChild( new electrical::VoltageSource<>( new object::ConstObj< double >( 2.5 ) ) );

    parallel->SetID( 0 );
    p1->SetID( 1 );
    s11->SetID( 2 );
    s11->at( 0 )->SetID( 3 );
    s11->at( 1 )->SetID( 4 );
    s12->SetID( 5 );
    s12->at( 0 )->SetID( 6 );
    s12->at( 1 )->SetID( 7 );
    s2->SetID( 8 );
    s2->at( 0 )->SetID( 9 );
    s2->at( 1 )->SetID( 10 );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent( ScalarUnit( "InputCurrent" ) );
    parallel->UpdateStateSystemGroup();

    // systm::DifferentialAlgebraicSystem< myMatrixType > test(&stateSystemGroup);
    const myMatrixType& dglMat = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();
    const myMatrixType& dglVec = stateSystemGroup.mDglStateSystem.GetEquationSystemCVector();
    const myMatrixType& algMat = stateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix();
    const myMatrixType& algVec = stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector();

    TS_ASSERT( dglMat.coeff( 0, 0 ).IsEmpty() );
    TS_ASSERT( dglMat.coeff( 1, 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( algMat.coeff( 0, 0 ), ScalarUnit( "NEG(1.000000)" ) );
    TS_ASSERT( algMat.coeff( 1, 0 ).IsEmpty() );

    TS_ASSERT( dglMat.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( dglMat.coeff( 1, 1 ).IsEmpty() );
    TS_ASSERT_EQUALS( algMat.coeff( 0, 1 ), ScalarUnit( "1.000000" ) );
    TS_ASSERT_EQUALS( algMat.coeff( 1, 1 ), ScalarUnit( "NEG(1.000000)" ) );

    TS_ASSERT_EQUALS( dglVec.coeff( 0, 0 ), ScalarUnit( "DIV(InputCurrent,ID3_ObjC)" ) );
    TS_ASSERT( dglVec.coeff( 1, 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( algVec.coeff( 0, 0 ), ScalarUnit( "NEG(MUL(InputCurrent,ID4_ObjR))" ) );
    TS_ASSERT_EQUALS( algVec.coeff( 1, 0 ), ScalarUnit( "ID10_ObjV" ) );

    TS_ASSERT_EQUALS( dglMat.coeff( 0, 2 ), ScalarUnit( "DIV(-1.000000,ID3_ObjC)" ) );
    TS_ASSERT_EQUALS( dglMat.coeff( 1, 2 ), ScalarUnit( "DIV(1.000000,ID6_ObjC)" ) );
    TS_ASSERT_EQUALS( algMat.coeff( 0, 2 ), ScalarUnit( "SUB(ID7_ObjR,MUL(-1.000000,ID4_ObjR))" ) );
    TS_ASSERT_EQUALS( algMat.coeff( 1, 2 ), ScalarUnit( "NEG(ID7_ObjR)" ) );

    TS_ASSERT_EQUALS( dglMat.coeff( 0, 3 ), ScalarUnit( "DIV(-1.000000,ID3_ObjC)" ) );
    TS_ASSERT( dglMat.coeff( 1, 3 ).IsEmpty() );
    TS_ASSERT_EQUALS( algMat.coeff( 0, 3 ), ScalarUnit( "NEG(MUL(-1.000000,ID4_ObjR))" ) );
    TS_ASSERT_EQUALS( algMat.coeff( 1, 3 ), ScalarUnit( "ID9_ObjR" ) );
#endif
}
