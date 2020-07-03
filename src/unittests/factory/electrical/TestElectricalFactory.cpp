/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestElectricalFactory.cpp
* Creation Date : 09-03-2016
* Last Modified : Fr 11 Mär 2016 10:55:35 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestElectricalFactory.h"

std::string TestElectricalFactory::CreateConfigurationForTwoPort() const
{
    return "<?xml version='1.0'?>\
        <LookupObj1D class='LookupObj1dWithState'>\
            <State class='ElectricalState' StateType='Current' />\
            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
        </LookupObj1D>";
}

std::string TestElectricalFactory::CreateConfigurationForTwoPort( const std::string& twoPortName, const std::string& /*objName*/ ) const
{
    return "<?xml version='1.0'?>\
            <Configuration>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='" +
           twoPortName +
           "' >\
                        <Object class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </Object>\
                </RootElement>\
             </Configuration>";
}

void TestElectricalFactory::TestStateInjection1Lookup() const
{
    this->StatePropagationTest1< myMatrixType, electrical::OhmicResistance >();
    this->StatePropagationTest1< myMatrixType, electrical::Capacity >();
    this->StatePropagationTest1< myMatrixType, electrical::VoltageSource >();
    this->StatePropagationTest1< myMatrixType, electrical::Inductance >();
    this->StatePropagationTest1< myMatrixType, electrical::ConstantPhaseElement >();
}

void TestElectricalFactory::TestStateInjectionForParallelRCLookup() const
{
#ifndef _SYMBOLIC_
    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< state::State, factory::ArgumentTypeState >* stateFactory = factoryBuilder.BuildStateFactory();
    factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > >* objectFactory =
     factoryBuilder.BuildObjectFactory();
    std::string name = this->CreateConfigurationForTwoPort();
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( name.c_str() );
    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );


    boost::scoped_ptr< factory::ArgumentTypeObject< double > > argObject( new factory::ArgumentTypeObject< double > );
    auto elecData = boost::make_shared< ElectricalDataStruct< double > >();
    argObject->mElectricalDataStruct = elecData;

    boost::shared_ptr< object::Object< double > > obj1( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );
    boost::shared_ptr< object::Object< double > > obj2( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );

    electrical::ParallelRC< myMatrixType > test( obj1, obj2, true, elecData );

    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        elecData->mCurrentValue = current;
        TS_ASSERT_DELTA( test.GetCurrentValue(), current, 0.000001 );
        auto valueR = test.mObject->GetValue();
        auto valueTau = test.mObjectTau->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
        TS_ASSERT_DELTA( valueTau, current, 0.000001 );
    }

#endif /* _SYMBOLIC_*/
}

void TestElectricalFactory::TestStateInjectionForZarcLookup() const
{
#ifndef _SYMBOLIC_
    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< state::State, factory::ArgumentTypeState >* stateFactory = factoryBuilder.BuildStateFactory();
    factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > >* objectFactory =
     factoryBuilder.BuildObjectFactory();
    std::string name = this->CreateConfigurationForTwoPort();
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( name.c_str() );
    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );


    boost::scoped_ptr< factory::ArgumentTypeObject< double > > argObject( new factory::ArgumentTypeObject< double > );
    auto elecData = boost::make_shared< ElectricalDataStruct< double > >();
    argObject->mElectricalDataStruct = elecData;

    boost::shared_ptr< object::Object< double > > obj1( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );
    boost::shared_ptr< object::Object< double > > obj2( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );
    boost::shared_ptr< object::Object< double > > obj3( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );

    electrical::Zarc< myMatrixType > test( obj1, obj2, obj3, true, 0.0001, elecData );

    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        elecData->mCurrentValue = current;
        TS_ASSERT_DELTA( test.GetCurrentValue(), current, 0.000001 );
        auto valueR = test.mLookupOhmicResistance->GetValue();
        auto valueTau = test.mLookupRelaxationTime->GetValue();
        auto valuePhi = test.mLookupPhi->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
        TS_ASSERT_DELTA( valueTau, current, 0.000001 );
        TS_ASSERT_DELTA( valuePhi, current, 0.000001 );
    }
#endif /* _SYMBOLIC_*/
}

void TestElectricalFactory::TestFactoryInjection1Lookup() const
{
#ifndef _SYMBOLIC_
    std::vector< std::string > twoPortVectorName{"OhmicResistance", "Capacity", "VoltageSource", "Inductance",
                                                 "ConstantPhaseElement"};

    for ( size_t j = 0; j < twoPortVectorName.size(); ++j )
    {
        factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
        factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
         factoryBuilder.BuildElectricalFactory();

        std::string xmlConfig = this->CreateConfigurationForTwoPort( twoPortVectorName[j], "" );
        boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromMem( xmlConfig.c_str() );

        boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

        TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

        auto twoPort( boost::static_pointer_cast< electrical::ElectricalElement< myMatrixType > >(
         electricalFactory->CreateInstance( rootparam ) ) );
        for ( size_t i = 1; i < 10; ++i )
        {
            auto current = static_cast< double >( i );
            twoPort->mDataStruct->mCurrentValue = current;

            TS_ASSERT_DELTA( twoPort->GetCurrentValue(), current, 0.000001 );
            auto value = twoPort->mObject->GetValue();
            TS_ASSERT_DELTA( value, current, 0.000001 );
        }
    }
#endif
}

void TestElectricalFactory::TestFactoryInjection2Lookup() const
{
#ifndef _SYMBOLIC_
    std::string twoPortVectorName( "ParallelRC" );
    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    std::string xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='ParallelRC' >\
                        <LookupTau class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </LookupTau >\
                        <LookupOhmicResistance class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                    </LookupOhmicResistance>\n\
                </RootElement>\
             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto twoPort( boost::static_pointer_cast< electrical::ParallelRC< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );
    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        twoPort->mDataStruct->mCurrentValue = current;

        TS_ASSERT_DELTA( twoPort->GetCurrentValue(), current, 0.000001 );
        auto valueR = twoPort->mObject->GetValue();
        auto valueTau = twoPort->mObjectTau->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
        TS_ASSERT_DELTA( valueTau, current, 0.000001 );
    }
#endif
}


void TestElectricalFactory::TestFactroyInjectionForWarburgCotanh() const
{
#ifndef _SYMBOLIC_
    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    std::string xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Option>\
            <Samplerate>100000000</Samplerate>\
            </Option>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='WarburgCotanh' >\
                        <Sigma class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </Sigma >\
                        <Clim class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                    </Clim>\n\
                </RootElement>\
             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto twoPort( boost::static_pointer_cast< electrical::WarburgCotanh< myMatrixType > >(
     electricalFactory->CreateInstance( rootparam ) ) );

    std::vector< double > lastRValues( twoPort->mChildren.size() - 1 );
    std::vector< double > lastTauValues( twoPort->mChildren.size() - 1 );
    double lastCValue = 0;
    for ( size_t i = 1; i < 10; ++i )
    {

        auto current = static_cast< double >( i );
        twoPort->mDataStruct->mCurrentValue = current;
        for ( size_t j = 0; j < twoPort->mChildren.size() - 1; ++j )
        {
            TS_ASSERT_DELTA( twoPort->mChildren.at( j )->GetCurrentValue(), 0.0, 0.000001 );
            auto child = boost::static_pointer_cast< electrical::ParallelRC< myMatrixType > >( twoPort->mChildren.at( j ) );
            auto valueR = child->mObject->GetValue();
            auto valueTau = child->mObjectTau->GetValue();
            TS_ASSERT( valueR > lastRValues[j] );
            TS_ASSERT( valueTau > lastTauValues[j] );
            lastRValues[j] = valueR;
            lastTauValues[j] = valueTau;
        }
        auto child = boost::static_pointer_cast< electrical::Capacity< myMatrixType > >(
         twoPort->mChildren.at( twoPort->mChildren.size() - 1 ) );

        auto valueC = child->mObject->GetValue();
        TS_ASSERT( valueC > lastCValue );
        lastCValue = valueC;
    }
#endif
}

void TestElectricalFactory::TestFactroyInjectionForWarburgTanh() const
{
#ifndef _SYMBOLIC_
    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    std::string xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
            <Option>\
            <Samplerate>100000000</Samplerate>\
            </Option>\
                <CustomDefinitions>\
                </CustomDefinitions>\
                <RootElement  class='WarburgTanh' >\
                        <OhmicResistance class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                        </OhmicResistance >\
                        <Tau class='LookupObj1dWithState'>\
                            <State class='ElectricalState' StateType='Current' />\
                            <LookupData>1,2,3,4,5,6,7,8,9</LookupData>\
                            <MeasurementPoints>1,2,3,4,5,6,7,8,9</MeasurementPoints>\
                    </Tau>\n\
                </RootElement>\
             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );

    TS_ASSERT_THROWS_NOTHING( electricalFactory->CreateInstance( rootparam ) );

    auto twoPort( boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >( electricalFactory->CreateInstance( rootparam ) ) );

    std::vector< double > lastRValues( twoPort->mChildren.size() );
    std::vector< double > lastTauValues( twoPort->mChildren.size() );
    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        twoPort->mDataStruct->mCurrentValue = current;
        for ( size_t j = 0; j < twoPort->mChildren.size(); ++j )
        {
            TS_ASSERT_DELTA( twoPort->mChildren.at( j )->GetCurrentValue(), 0.0, 0.000001 );
            auto child = boost::static_pointer_cast< electrical::ParallelRC< myMatrixType > >( twoPort->mChildren.at( j ) );
            auto valueR = child->mObject->GetValue();
            auto valueTau = child->mObjectTau->GetValue();
            TS_ASSERT( valueR > lastRValues[j] );
            TS_ASSERT( valueTau > lastTauValues[j] );
            lastRValues[j] = valueR;
            lastTauValues[j] = valueTau;
        }
    }
#endif
}


void TestElectricalFactory::TestFactroyCellElementReversibleHeat() const
{
    factory::FactoryBuilder< myMatrixType, ScalarUnit > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    std::vector< std::string > testStringVector = {
     "", "<ReversibleHeat class='ConstObj'><Value> 1e-4</Value></ReversibleHeat>",
     "<ReversibleHeat class='LookupObj1dWithState'><State cacheref='Soc' /><LookupData>-1, 1e-4, 1</LookupData><MeasurementPoints> \
      0, 20,40 </MeasurementPoints> </ReversibleHeat>",
     "<ReversibleHeat class='LookupObj2DWithState'><RowState cacheref='Soc' /><ColState cacheref='ThermalState' /><LookupData>-1,-1,-1; -1, 1e-4, 1; -1,-1,-1;</LookupData><MeasurementPointsRow> \
      0, 20,40 </MeasurementPointsRow> <MeasurementPointsColumn>0, 23,100 </MeasurementPointsColumn></ReversibleHeat>"};

    const double current = 10;
    for ( size_t i = 0; i < testStringVector.size(); ++i )
    {


        std::string xmlConfig =
         "<?xml version='1.0'?>\
        <Configuration>\
        <CellElement class='CellElement' observable='TRUE'>" +
         testStringVector.at( i ) +
         "<ThermalState class='ThermalState' cache='True'>\
                <InitialTemperature>23</InitialTemperature>\
            </ThermalState>\
            <Soc class='Soc' cache='True'>\
                <InitialCapacity>2.05</InitialCapacity>\
                <InitialSoc>20</InitialSoc>\
                <MeasurementPoints>5,10,20,23,35,50,65,80,90</MeasurementPoints>\
            </Soc>\
            <Children>\
            <Voltage class='VoltageSource'><Object class='ConstObj'><Value>0</Value></Object></Voltage> \
            <ZarcElement class='ZarcElement'>\
                <LookupTau class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    1.12980227037822e-01, 1.71749789914134e-01, 4.27878577005388e-01, 4.70584769553997e-01;\
                    2.88029269499999e-01, 2.03069924403468e-01, 7.59434298204209e-02, 5.38248547348156e-02;\
                    1.80149147116020e-01, 1.12630469688383e-01, 3.52185854766679e-02, 2.98230802877780e-02;\
                    1.47909096582298e-01, 8.33921914682953e-02, 2.57060565921076e-02, 2.22501291807582e-02;\
                    1.32900242338032e-01, 7.22938740353130e-02, 2.05480235442034e-02, 9.95886997764575e-02;\
                    1.19852623435413e-01, 6.56657395160509e-02, 2.36971844841798e-02, 1.02914420936082e-01;\
                    1.18273000586515e-01, 6.60491071602289e-02, 3.25921294643322e-02, 1.09197268465146e-01 \
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupTau>\
                <LookupOhmicResistance class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>\
                    0.0264994082415437, 0.0264994082415437, 0.0264994082415437, 0.0299913094135788;\
                    0.0675569999999998, 0.0313318160740531, 0.0127731516995969, 0.00404164451684630;\
                    0.0536386688114087, 0.0248200664672378, 0.00989750001969683, 0.00303781463624295;\
                    0.0478600636580172, 0.0219992555610513, 0.00877924658179266, 0.00234391473354453;\
                    0.0441171340633137, 0.0206405039153971, 0.00794555485936455, 0.00454415109298745;\
                    0.0418986584966986, 0.0203278573267527, 0.00866505912064747, 0.00344848953405180;\
                    0.0421316495020664, 0.0207584663601899, 0.0102011087727835, 0.00296383805146105\
                    </LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupOhmicResistance>\
                <LookupPhi class='LookupObj2DWithState'>\
                    <RowState cacheref='ThermalState'/>\
                    <ColState cacheref='Soc'/>\
                    <LookupData>0.574590000000000,0.516891694022437,0.408379079339744,0.110443124808620;0.574590000000000,0.516891694022437,0.568073405078093,0.566435038531618;0.616859078283881,0.579111165549142,0.662374262351899,0.620812008168125;0.625502091112064,0.610693210867254,0.697319797270455,0.671812046180353;0.621586176902636,0.620972340223097,0.721715387830978,0.323239015612757;0.620607929391110,0.630419920219851,0.671002168217511,0.371661290927877;0.620534462894923,0.628465004550853,0.585312180039373,0.410302314031108</LookupData>\
                    <MeasurementPointsRow>0,10,23,50</MeasurementPointsRow>\
                    <MeasurementPointsColumn>5,20,35,50,65,80,90</MeasurementPointsColumn>\
                </LookupPhi>\
            </ZarcElement>\
            </Children>\
        </CellElement>\
        <Options>\
        </Options>\
        </Configuration>";
        boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromMem( xmlConfig.c_str() );

        boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );

        auto rootport = electricalFactory->CreateInstance( rootparam->GetElementChild( "CellElement" ) );

        auto cell = boost::static_pointer_cast< electrical::Cellelement< myMatrixType > >( rootport );
        cell->mCurrentValue = current;
        if ( i > 0 )
        {
            TS_ASSERT_EQUALS( cell->HasReversibleHeat(), true );
#ifndef _SYMBOLIC_
            TS_ASSERT_DELTA( cell->CalculateReversibleHeat(), current * ( 23 + 273.15 ) * 1e-4, 0.0001 );
#endif
        }
        else
        {
            TS_ASSERT_EQUALS( cell->HasReversibleHeat(), false );
#ifdef _SYMBOLIC_
            TS_ASSERT_EQUALS( cell->CalculateReversibleHeat(), 0 );
#else
            TS_ASSERT_DELTA( cell->CalculateReversibleHeat(), 0, 0.0001 );
#endif
        }
    }
}

void TestElectricalFactory::TestAgingStateCreation() const
{
    /*
    boost::scoped_ptr< factory::Factory< state::State, factory::ArgumentTypeState > > stateFactory(
    factory::BuildStateFactory() );
    boost::scoped_ptr< factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > >
    objectFactory(
     factory::BuildObjectFactory< double >( stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< aging::AgingBase< myMatrixType >, factory::ArgumentTypeAging< myMatrixType > >
    > agingFactory(
     factory::BuildAgingFactory< myMatrixType >( objectFactory.get(), stateFactory.get() ) );
    boost::scoped_ptr< factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > >
    electricalFactory(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get(),
    agingFactory.get() ) );

    std::string xmlConfig =
     "<?xml version='1.0'?>\
        <Configuration>\
            <Options>\
                <Aging>\
                    <AgingStepTime>10</AgingStepTime>\
                </Aging>\
            </Options>\
            <Aging>\
                <AnodeLAM>0.2, 0.5</AnodeLAM>\
                <CathodeLAM>0.4, 0.7</CathodeLAM>\
                <LLI>0.1, 0.02</LLI>\
                <ResistanceFactor>0.3, 0.6</ResistanceFactor>\
                <CellAge>10, 15</CellAge>\
                <ChargeThroughput>12, 20</ChargeThroughput>\
            </Aging>\
            <RootElement class='CellElement'>\
                <Children/>\
                <ThermalState class='ThermalState' cache='True'>\
                    <InitialTemperature>23</InitialTemperature>\
                </ThermalState>\
                <Soc class='Soc' cache='True'>\
                    <InitialCapacity>2.05</InitialCapacity>\
                    <InitialSoc>20</InitialSoc>\
                </Soc>\
                <Aging/>\
            </RootElement>\
         </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );

    // factory should read the first aging state from the XML
    auto rootport = electricalFactory->CreateInstance( rootparam->GetElementChild( "RootElement" ) );
    auto agingState = boost::static_pointer_cast< electrical::Cellelement< myMatrixType > >( rootport
    )->GetAgingState();
    TS_ASSERT_DELTA( agingState->GetAnodeLam(), 0.2, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCathodeLam(), 0.4, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetLli(), 0.1, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetResistanceFactor(), 0.3, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCellAge(), 10, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetChargeThroughput(), 12, 0.0001 );

    // factory should read the second aging state from the XML
    rootport = electricalFactory->CreateInstance( rootparam->GetElementChild( "RootElement" ) );
    agingState = boost::static_pointer_cast< electrical::Cellelement< myMatrixType > >( rootport )->GetAgingState();
    TS_ASSERT_DELTA( agingState->GetAnodeLam(), 0.5, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCathodeLam(), 0.7, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetLli(), 0.02, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetResistanceFactor(), 0.6, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCellAge(), 15, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetChargeThroughput(), 20, 0.0001 );

    // resets the cell counter to 0
    electricalFactory.reset(
     factory::BuildElectricalFactory< myMatrixType, double >( objectFactory.get(), stateFactory.get(),
    agingFactory.get() ) );

    auto agingStateFromArg = boost::make_shared< state::AgingState >( 0.17, 0.23, 0.13, 0.0, 0.31, 16, 19 );
    boost::scoped_ptr< factory::ArgumentTypeElectrical > arg( new factory::ArgumentTypeElectrical() );
    arg->mAgingStates.push_back( agingStateFromArg );

    // factory should read the first aging state from the argument
    rootport = electricalFactory->CreateInstance( rootparam->GetElementChild( "RootElement" ), arg.get() );
    agingState = boost::static_pointer_cast< electrical::Cellelement< myMatrixType > >( rootport )->GetAgingState();
    TS_ASSERT_DELTA( agingState->GetAnodeLam(), 0.17, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCathodeLam(), 0.23, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetLli(), 0.13, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetResistanceFactor(), 0.31, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCellAge(), 16, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetChargeThroughput(), 19, 0.0001 );

    // factory should read the second aging state from the XML, since the aging state vector only contains one element
    rootport = electricalFactory->CreateInstance( rootparam->GetElementChild( "RootElement" ), arg.get() );
    agingState = boost::static_pointer_cast< electrical::Cellelement< myMatrixType > >( rootport )->GetAgingState();
    TS_ASSERT_DELTA( agingState->GetAnodeLam(), 0.5, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCathodeLam(), 0.7, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetLli(), 0.02, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetResistanceFactor(), 0.6, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCellAge(), 15, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetChargeThroughput(), 20, 0.0001 );

    // factory should create a new aging state, since the XML only contains two aging states
    rootport = electricalFactory->CreateInstance( rootparam->GetElementChild( "RootElement" ), arg.get() );
    agingState = boost::static_pointer_cast< electrical::Cellelement< myMatrixType > >( rootport )->GetAgingState();
    TS_ASSERT_DELTA( agingState->GetAnodeLam(), 0, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCathodeLam(), 0, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetLli(), 0, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetResistanceFactor(), 1, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetCellAge(), 0, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetChargeThroughput(), 0, 0.0001 );
    */
}
