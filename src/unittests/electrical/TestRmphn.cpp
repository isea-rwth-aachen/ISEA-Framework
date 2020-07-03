/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestRmphn.cpp
* Creation Date : 29-08-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestRmphn.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/rmphn.h"

#include "../../factory/electricalEquationSystem/chooseElectricSystemType.h"
#include "../../factory/factorybuilder.h"

#include "../../object/const_obj.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

extern template class electrical::Rmphn< myMatrixType >;
extern template class electrical::ParallelRC< myMatrixType >;
extern template class systm::StateSystemGroup< myMatrixType >;
extern template class object::Object< double >;
extern template class object::ConstObj< double >;
extern template class factory::Factory< state::State, ::factory::ArgumentTypeState >;
extern template class factory::Factory< ::object::Object< double >, ::factory::ArgumentTypeObject< double > >;
extern template class factory::Factory< ::electrical::TwoPort< myMatrixType >, ::factory::ArgumentTypeElectrical >;


void TestRmphn::testRmphnCreation()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );

    Rmphn< myMatrixType > rm( r_ct, r_mp );
    auto rc = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, rm.mDataStruct );
    rm.AddChild( rc );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    rm.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.resize( 1, maxSize );
    current.setZero();
    current.coeffRef( 0, maxSize - 1 ) = cur;
    rm.SetCurrent( current );
    auto voltage = *rm.GetVoltage();
#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA( voltage.coeff( 0, 0 ), 4.31132, 0.001 );
#endif
    TS_ASSERT_EQUALS( voltage.rows(), 1 );
    TS_ASSERT_EQUALS( voltage.cols(), 1 );
#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix().coeff( 0, 0 ), 99, 0.0001 );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemCVector().coeff( 0, 0 ), 99, 0.0001 );
#endif
    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector().size(), 0 );
}

// Test the implementation R-RC vs RMPHN
// Both circuits are created and compared so that both circuits show equal values for the system matrix and all other
// matrices
void TestRmphn::testRmphnRCDifference()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );
    const double ohmValue( 0.331132 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );
    auto ohm = boost::make_shared< object::ConstObj< double > >( ohmValue );

    Rmphn< myMatrixType > rm( r_ct, r_mp );
    auto rc1 = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, rm.mDataStruct );
    rm.AddChild( rc1 );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    rm.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.resize( 1, maxSize );
    current.setZero();
    current.coeffRef( 0, maxSize - 1 ) = cur;
    rm.SetCurrent( current );
    auto voltage = *rm.GetVoltage();

    SerialTwoPort< myMatrixType > ser;
    auto rc = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, ser.mDataStruct );
    auto ohmEle = boost::make_shared< OhmicResistance< myMatrixType > >( ohm );
    ser.AddChild( rc );
    ser.AddChild( ohmEle );

    systm::StateSystemGroup< myMatrixType > differenceGroup;
    differenceGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    ser.SetSystem( &differenceGroup );
    differenceGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType currentdiff;
    currentdiff.resize( 1, maxSize );
    currentdiff.setZero();
    currentdiff.coeffRef( 0, maxSize - 1 ) = cur;
    ser.SetCurrent( currentdiff );
    auto voltagediff = *ser.GetVoltage();
#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA( voltagediff.coeff( 0, 0 ), voltage.coeff( 0, 0 ), 0.0001 );
#endif

    auto y = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();
    auto x = differenceGroup.mDglStateSystem.GetEquationSystemAMatrix();
#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA( x.coeff( 0, 0 ), y.coeff( 0, 0 ), 0.0001 );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemCVector().coeff( 0, 0 ),
                     differenceGroup.mDglStateSystem.GetEquationSystemCVector().coeff( 0, 0 ), 0.0001 );
#endif

    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector().size(), 0 );
    TS_ASSERT_EQUALS( differenceGroup.mAlgStateSystem.GetEquationSystemAMatrix().size(), 0 );
}

// Same as above, but this time created through the factory
// Test the implementation R-RC vs RMPHN
// Both circuits are created and compared so that both circuits show equal values for the system matrix and all other
// matrices
void TestRmphn::testRmphnFactoryCreation()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );
    const double ohmValue( 0.331132 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );
    auto ohm = boost::make_shared< object::ConstObj< double > >( ohmValue );

    Rmphn< myMatrixType > rm( r_ct, r_mp );
    auto rc1 = boost::make_shared< ParallelRC< myMatrixType > >( r_ct, tau_dl, true, rm.mDataStruct );
    rm.AddChild( rc1 );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    rm.SetSystem( &stateSystemGroup );
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.resize( 1, maxSize );
    current.setZero();
    current.coeffRef( 0, maxSize - 1 ) = cur;
    rm.SetCurrent( current );
    auto voltage = *rm.GetVoltage();


    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();


    const char* xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Options/>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement class='Rmphn'>\
                <LookupOhmicResistance class='ConstObj'>\
                    <Value>10</Value>\
                </LookupOhmicResistance>\
                <LookupTau class='ConstObj'>\
                    <Value>1</Value>\
                </LookupTau>\
                <OhmicResistance_RMP class='ConstObj'>\
                    <Value>1</Value>\
                </OhmicResistance_RMP>\
                </RootElement>\
             </Configuration>";


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto sd = electricalFactory->CreateInstance( rootparam );

    systm::StateSystemGroup< myMatrixType > differenceGroup;
    differenceGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    sd->SetSystem( &differenceGroup );
    differenceGroup.mDglStateSystem.Initialize( maxSize );

    current.resize( 1, maxSize );
    current.setZero();
    current.coeffRef( 0, maxSize - 1 ) = cur;
    sd->SetCurrent( current );
    auto voltagediff = *sd->GetVoltage();


#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA( voltagediff.coeff( 0, 0 ), voltage.coeff( 0, 0 ), 0.0001 );
#endif

    auto y = stateSystemGroup.mDglStateSystem.GetEquationSystemAMatrix();
    auto x = differenceGroup.mDglStateSystem.GetEquationSystemAMatrix();
#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA( x.coeff( 0, 0 ), y.coeff( 0, 0 ), 0.0001 );
    TS_ASSERT_DELTA( stateSystemGroup.mDglStateSystem.GetEquationSystemCVector().coeff( 0, 0 ),
                     differenceGroup.mDglStateSystem.GetEquationSystemCVector().coeff( 0, 0 ), 0.0001 );
#endif

    TS_ASSERT_EQUALS( stateSystemGroup.mAlgStateSystem.GetEquationSystemCVector().size(), 0 );
    TS_ASSERT_EQUALS( differenceGroup.mAlgStateSystem.GetEquationSystemAMatrix().size(), 0 );
    auto rmphn = boost::static_pointer_cast< electrical::Rmphn< myMatrixType > >( sd );
    TS_ASSERT_EQUALS( rmphn->size(), 1 );
    TS_ASSERT_EQUALS( strcmp( rmphn->at( 0 )->GetName(), "ParallelRC" ), 0 );
}

void TestRmphn::testRmphnFactoryCreationSimplification()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );
    const double ohmValue( 0.331132 );

    const double cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );
    auto ohm = boost::make_shared< object::ConstObj< double > >( ohmValue );

    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();


    const char* xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Options>\
                <SampleRate>\
                    0.00001\
                </SampleRate>\
            </Options>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement class='Rmphn'>\
                <LookupOhmicResistance class='ConstObj'>\
                    <Value>10</Value>\
                </LookupOhmicResistance>\
                <LookupTau class='ConstObj'>\
                    <Value>1</Value>\
                </LookupTau>\
                <OhmicResistance_RMP class='ConstObj'>\
                    <Value>1</Value>\
                </OhmicResistance_RMP>\
                </RootElement>\
             </Configuration>";


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto sd = electricalFactory->CreateInstance( rootparam );

    systm::StateSystemGroup< myMatrixType > differenceGroup;
    differenceGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    sd->SetSystem( &differenceGroup );
    differenceGroup.mDglStateSystem.Initialize( maxSize );

    myMatrixType current;
    current.resize( 1, maxSize );
    current.setZero();
    current.coeffRef( 0, maxSize - 1 ) = cur;
    sd->SetCurrent( current );
    auto voltagediff = *sd->GetVoltage();


#ifndef _SYMBOLIC_
    TS_ASSERT_DELTA( voltagediff.coeff( 0, 0 ), ( ohmValue + resValue ) * cur, 0.0001 );
#endif

    auto x = differenceGroup.mDglStateSystem.GetEquationSystemAMatrix();
    TS_ASSERT_EQUALS( x.size(), 0 );
    TS_ASSERT_EQUALS( differenceGroup.mDglStateSystem.GetEquationSystemCVector().size(), 0 );

    auto rmphn = boost::static_pointer_cast< electrical::Rmphn< myMatrixType > >( sd );
    TS_ASSERT_EQUALS( rmphn->size(), 1 );
    TS_ASSERT_EQUALS( strcmp( rmphn->at( 0 )->GetName(), "OhmicResistance" ), 0 );
}

void TestRmphn::testRmphnFactoryPower()
{
    const size_t maxSize = 1;
    const double resValue( 10.0 );
    const double tauValue( 1 );
    const double rmpValue( 1 );
    const double ohmValue( 0.331132 );

    const ScalarUnit cur = 10;
    auto r_ct = boost::make_shared< object::ConstObj< double > >( resValue );
    auto tau_dl = boost::make_shared< object::ConstObj< double > >( tauValue );
    auto r_mp = boost::make_shared< object::ConstObj< double > >( rmpValue );
    auto ohm = boost::make_shared< object::ConstObj< double > >( ohmValue );

    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();


    const char* xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Options>\
                <SampleRate>\
                    100000\
                </SampleRate>\
            </Options>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement class='Rmphn' observable='true'>\
                <LookupOhmicResistance class='ConstObj'>\
                    <Value>10</Value>\
                </LookupOhmicResistance>\
                <LookupTau class='ConstObj'>\
                    <Value>1</Value>\
                </LookupTau>\
                <OhmicResistance_RMP class='ConstObj'>\
                    <Value>1</Value>\
                </OhmicResistance_RMP>\
                </RootElement>\
             </Configuration>";


    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto sd = electricalFactory->CreateInstance( rootparam );

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    stateSystemGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    /*
    sd->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();*/
    stateSystemGroup.mDglStateSystem.Initialize( maxSize );


    systm::StateSystemGroup< myMatrixType > differenceGroup;
    differenceGroup.mDglStateSystem = systm::StateSystem< myMatrixType >();
    sd->SetSystem( &differenceGroup );
    // differenceGroup.mDglStateSystem.Initialize( maxSize );
    differenceGroup.Initialize();
    auto mEqSystem = factory::ChooseElectricEquationSystemType( &differenceGroup );

    myMatrixType current;
    current.resize( 1, maxSize );
    current.setZero();
    current.coeffRef( 0, maxSize - 1 ) = cur;
    sd->SetCurrent( current );
    auto voltagediff = *sd->GetVoltage();

    differenceGroup.mStateVector.resize( maxSize, 1 );
    differenceGroup.mStateVector.setZero();
    for ( size_t i = 0; i < maxSize; ++i )
    {
        differenceGroup.mStateVector.coeffRef( i, 0 ) = 1;
    }


    sd->UpdateStateSystemGroup();
    mEqSystem->PrepareEquationSystem();
    auto voltage = *sd->GetVoltage();
    sd->CalculateStateDependentValues();

#ifdef _SYMBOLIC_
    TS_ASSERT_EQUALS( sd->GetCurrentValue(), cur );
#else
    TS_ASSERT_DELTA( sd->GetCurrentValue(), cur, 0.0001 );
    TS_ASSERT_DELTA( sd->GetVoltageValue(), 1 + cur * ohmValue, 0.0001 );                     // u + i* r
    TS_ASSERT_DELTA( sd->GetPowerValue(), 1.0 / resValue + cur * cur * ohmValue, 0.0001 );    // u^2 / r + i^2 * r;
#endif
}
