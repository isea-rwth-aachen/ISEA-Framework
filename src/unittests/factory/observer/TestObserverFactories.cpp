/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestObserverFactories.cpp
* Creation Date : 10-08-2015
* Last Modified : Do 12 Mai 2016 13:30:14 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestObserverFactories.h"
#include "../../../factory/observer/observerfactorybuilder.h"
#include "../../../misc/tinydir.h"
#include "../../../xmlparser/tinyxml2/xmlparserimpl.h"

using namespace factory;
typedef Factory< observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >, ArgumentTypeObserver > fac_ob;

typedef observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > > FilterT;

void TestObserverFactories::TestObserverFactoryCSVCreation()
{
#ifndef _SYMBOLIC_
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"CSVFilter\">\
                        <printHeader> 0  </printHeader> \
                        <Filename> unittest.csv</Filename>\
                    </Filter1>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
#endif /* _SYMBOLIC_*/
}

void TestObserverFactories::TestObserverFactorySTDCreation()
{
#ifndef _SYMBOLIC_
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"StdoutFilter\"/>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
#endif /* _SYMBOLIC_*/
}

void TestObserverFactories::TestObserverFactoryDecimationFilterCreation()
{
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"DecimateFilter\">\
                        <TimeDelay> 0.1 </TimeDelay> \
                    </Filter1>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
}

void TestObserverFactories::TestObserverFactoryMatlabFilterCreation()
{
#ifndef _SYMBOLIC_
    {
        boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
        const char *xmlConfig =
         "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"MatlabFilter\">\
                        <Filename> unittest.mat </Filename> \
                        <maxSampleSize> 1 </maxSampleSize> \
                    </Filter1>\
                </Observer>\
             </Configuration>";
        boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
        parser->ReadFromMem( xmlConfig );
        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
         parser->GetRoot()->GetElementChildren( "Observer" );
        TS_ASSERT_EQUALS( children.size(), 1 );
        TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
        boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
    }
#endif
}

void TestObserverFactories::TestObserverFactorySqliteFilterCreation()
{
#ifndef _SYMBOLIC_
#ifdef _SQLITE_
    _tinydir_remove( "unittest.db" );
    boost::scoped_ptr< fac_ob > fact( factory::BuildObserverFactoryTwoPort< myMatrixType, true >() );
    const char *xmlConfig =
     "<?xml version='1.0'?>\
            <Configuration>\
                <Observer>\
                    <Filter1 class=\"SqliteFilter\">\
                        <Filename> unittest.db </Filename> \
                    </Filter1>\
                </Observer>\
             </Configuration>";
    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( xmlConfig );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
     parser->GetRoot()->GetElementChildren( "Observer" );
    TS_ASSERT_EQUALS( children.size(), 1 );
    TS_ASSERT_EQUALS( strcmp( children.at( 0 )->GetElementName(), "Filter1" ), 0 );
    boost::shared_ptr< FilterT > filt = fact->CreateInstance( children.at( 0 ) );
    tinydir_file file;
    TS_ASSERT_EQUALS( tinydir_file_open( &file, "unittest.db" ), 0 );
#endif
#endif
}
