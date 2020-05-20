/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestXMLExceptions.cpp
* Creation Date : 21-02-2014
* Last Modified : Do 15 Jun 2017 15:50:30 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestXMLExceptions.h"

#include "../../misc/matrixInclude.h"

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../../factory/factorybuilder.h"

#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

using namespace ::state;

void TestXMLExceptions::testXMLParserWrongFilename()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromFile( "blabla" ), const xmlparser::ConfigFileNotFoundException& );
#endif
}

void TestXMLExceptions::testXMLParserNullFilename()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromFile( 0 ), const xmlparser::ConfigFileNotFoundException& );
#endif
}

void TestXMLExceptions::testXMLParserWrongMemAddr()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromMem( "blabla" ), const xmlparser::CouldNotParseConfigException& );
#endif
}

void TestXMLExceptions::testXMLParserNullMemAddr()
{
#ifdef __EXCEPTIONS__
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromMem( 0 ), const xmlparser::ConfigFileNotFoundException& );
#endif
}

void TestXMLExceptions::testXMLNotMatchingElements()
{
#ifdef __EXCEPTIONS__
    const char* xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             <MyCapacity class='Capacity' observable='TRUE'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyCapacity>\
                             </CustomDefinitions>\
                             <RootElement ref='MyCapacity'/>\
                             </Configuration121>";

    boost::shared_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    TS_ASSERT_THROWS( parser->ReadFromMem( xmlConfig ), const xmlparser::CouldNotParseConfigException& );
#endif
}


void TestXMLExceptions::testXMLNoRootElement()
{
#ifdef __EXCEPTIONS__
    const char* xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             <MyCapacity class='Capacity' observable='TRUE'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyCapacity>\
                             </CustomDefinitions>\
                             </Configuration>";

    boost::shared_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    TS_ASSERT_THROWS( parser->GetRoot()->GetElementChild( "RootElement" ), const std::exception& );

#endif
}


void TestXMLExceptions::testXMLNoReferencedElement()
{
#ifdef __EXCEPTIONS__
    const char* xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             <MyCapacity class='Capacity' observable='TRUE'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyCapacity>\
                             </CustomDefinitions>\
                             <RootElement ref='MyCapacity123'/>\
                             </Configuration>";

    boost::shared_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    TS_ASSERT_THROWS( parser->GetRoot()->GetElementChild( "RootElement" ), const std::exception& );
#endif
}


void TestXMLExceptions::testXMLCacheRefInstance()
{
#ifdef __EXCEPTIONS__
    factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    const char* xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class='ParallelTwoPort'>\
                             <Children>\
                             <MyCapacity class='Capacity' cache='True'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </MyCapacity>\
                             <NormaleCapacity cacheref='MyCapacity'/>\
                             <IrgendeineCapacity cacheref='MyCaca'/>\
                             <MeineCapacity cacheref='MaCici'/>\
                             </Children>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), const factory::ClassNotCachedException& );
#endif
}


void TestXMLExceptions::testXMLClassNotFound()
{
#ifdef __EXCEPTIONS__
    factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    const char* xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class='Capacity123'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), const factory::ClassNotFoundException& );
#endif
}


void TestXMLExceptions::testXMLAttributeNotFound()
{
#ifdef __EXCEPTIONS__
    factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    const char* xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class123='Capacity'>\
                             <Object class='ConstObj'>\
                             <Value>10</Value>\
                             </Object>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), const std::exception& );
#endif
}


void TestXMLExceptions::testXMLElementContentNotReadable()
{
#ifdef __EXCEPTIONS__
    factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    const char* xmlConfig =
     "<?xml version='1.0'?>\
                             <Configuration>\
                             <CustomDefinitions>\
                             </CustomDefinitions>\
                             <RootElement class='Capacity'>\
                             <Object class='ConstObj'>\
                             <Value></Value>\
                             </Object>\
                             </RootElement>\
                             </Configuration>";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( electricalFactory->CreateInstance( rootparam ), const std::exception& );
#endif
}

void TestXMLExceptions::testXMLCatchNegativeValue()
{
#ifdef __EXCEPTIONS__
    factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    const char* xmlConfig =
     "<?xml version='1.0'?>\n\
                            <Configuration>\n\
                            <CustomDefinitions>\n\
                            <MyWarburgTanh class='WarburgTanh' RCCounter='-1' >\n\
                            <Tau class='ConstObj'>\n\
                            <Value> 10 </Value>\n\
                            </Tau>\n\
                            <OhmicResistance class='ConstObj'>\n\
                            <Value> 5 </Value>\n\
                            </OhmicResistance>\n\
                            </MyWarburgTanh>\n\
                            </CustomDefinitions>\n\
                            <RootElement ref='MyWarburgTanh'>\n\
                            </RootElement>\n\
                            </Configuration>\n\0";

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    TS_ASSERT_THROWS( boost::shared_ptr< electrical::WarburgTanh< myMatrixType > > rootport(
                       boost::static_pointer_cast< electrical::WarburgTanh< myMatrixType > >(
                        electricalFactory->CreateInstance( rootparam ) ) ),
                      const std::runtime_error& );
#endif
}
