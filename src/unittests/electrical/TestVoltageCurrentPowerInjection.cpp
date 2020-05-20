/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestVoltageCurrentPowerInjection.cpp
* Creation Date : 07-03-2016
* Last Modified : Di 08 Mär 2016 16:03:59 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestVoltageCurrentPowerInjection.h"

// BOOST
#include <boost/make_shared.hpp>

// ETC
#include "../../object/const_obj.h"

#include "../../electrical/capacity.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/constantphaseelement.h"
#include "../../electrical/inductance.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/parallelRCAlg.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/paralleltwoport.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/twoport.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/warburgCotanh.h"
#include "../../electrical/warburgTanh.h"
#include "../../electrical/zarc.h"
#include "../../electrical/zarcalg.h"

void Asserter( ElectricalDataStruct< double > *data )
{
    TS_ASSERT_EQUALS( data->mCurrentValue, 1.0 )
    TS_ASSERT_EQUALS( data->mVoltageValue, 2.0 )
    TS_ASSERT_EQUALS( data->mPowerValue, 3.0 )
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::TwoArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    TConcrete< MatrixT > tester( false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::ThreeArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    boost::shared_ptr< object::Object< double > > obj( 0 );
    TConcrete< MatrixT > tester( obj, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::FourArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    boost::shared_ptr< object::Object< double > > obj( 0 );
    TConcrete< MatrixT > tester( obj, obj, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

template < typename MatrixT, template < typename > class TConcrete >
void TestVoltageCurrentPowerInjection::FiveArgs()
{
    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );
    boost::shared_ptr< object::Object< double > > obj( new object::ConstObj< double >( 10.0 ) );
    TConcrete< MatrixT > tester( obj, obj, obj, 0.001, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
}

void TestVoltageCurrentPowerInjection::TestTwoArgumentsTwoPorts()
{    // Two arguments
#ifndef _SYMBOLIC_

    this->TwoArgs< myMatrixType, electrical::TwoPort >();
    this->TwoArgs< myMatrixType, electrical::SerialTwoPort >();
    this->TwoArgs< myMatrixType, electrical::ParallelTwoPort >();
    this->TwoArgs< myMatrixType, electrical::WarburgTanh >();
    this->TwoArgs< myMatrixType, electrical::WarburgCotanh >();
#endif /* SYMBOLIC */
}

void TestVoltageCurrentPowerInjection::TestThreeArgumentsTwoPorts()
{
// Three arguments
#ifndef _SYMBOLIC_
    this->ThreeArgs< myMatrixType, electrical::OhmicResistance >();
    this->ThreeArgs< myMatrixType, electrical::Capacity >();
    this->ThreeArgs< myMatrixType, electrical::VoltageSource >();
    this->ThreeArgs< myMatrixType, electrical::ConstantPhaseElement >();
    this->ThreeArgs< myMatrixType, electrical::Inductance >();
#endif /* SYMBOLIC */
}

void TestVoltageCurrentPowerInjection::TestFourArgumentsTwoPorts()
{
// Four arguments
#ifndef _SYMBOLIC_
    this->FourArgs< myMatrixType, electrical::ParallelRC >();
    this->FourArgs< myMatrixType, electrical::ParallelRCAlg >();
#endif /* SYMBOLIC */
}

void TestVoltageCurrentPowerInjection::TestFiveArgumentsTwoPorts()
{
#ifndef _SYMBOLIC_
    this->FiveArgs< myMatrixType, electrical::Zarc >();
    this->FiveArgs< myMatrixType, electrical::ZarcAlg >();
#endif /* SYMBOLIC */
}

void TestVoltageCurrentPowerInjection::TestEveryhtingElse()
{
#ifndef _SYMBOLIC_
    boost::shared_ptr< ::state::ThermalState< double > > temp = boost::make_shared< ::state::ThermalState< double > >( 25 );
    boost::shared_ptr< electrical::state::Soc > socState = boost::make_shared< ::electrical::state::Soc >( 2.05, 2.05, 50 );

    double a = 1.0, b = 2.0, c = 3.0;
    auto dataStruct = boost::make_shared< ElectricalDataStruct< double > >( a, b, c );

    electrical::Cellelement< myMatrixType > tester( temp, socState, false, dataStruct );
    Asserter( tester.mDataStruct.get() );
#endif /* SYMBOLIC */
}
