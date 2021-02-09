#ifndef _METADATA_FILTER_
#define _METADATA_FILTER_

#include "../../aging/aging_twoport.h"
#include "../../misc/checksum.h"
#include "../../time_series/time_series.h"
#include "../../version/version.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"
#include "filter.h"

#include <cctype>
#include <chrono>
#include <iterator>
#include <stdexcept>

namespace observer
{

template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class MetadataFilter : public Filter< T, TConcrete, ArgumentType >
{
    public:
    MetadataFilter( const std::string &filename, const xmlparser::XmlParser &simulationXml,
                    const electrical::TimeSeries< double, electrical::EvalNoInterpolation > &simulationProfile );
    virtual ~MetadataFilter();
    virtual void PrepareFilter( ArgumentType &prePareData );

    protected:
    const std::string mFilename;
    const std::chrono::time_point< std::chrono::system_clock > mStartTime;
    xmlparser::tinyxml2::XmlParserImpl mParser;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
MetadataFilter< T, TConcrete, ArgumentType >::MetadataFilter( const std::string &filename, const xmlparser::XmlParser &simulationXml,
                                                              const electrical::TimeSeries< double, electrical::EvalNoInterpolation > &simulationProfile )
    : mFilename( filename )
    , mStartTime( std::chrono::system_clock::now() )
{
    std::string username;
    char *env_username = std::getenv( "USER" );
    if ( !env_username )
    {
        env_username = std::getenv( "USERNAME" );
    }
    if ( env_username )
    {
        username = env_username;
    }
    else
    {
        username = "";
    }
    auto root = mParser.CreateRootElement( "Metadata" );
    root->AddChild( "User" )->SetValue( username.c_str() );
    root->AddChild( "CommitHash" )->SetValue( CMAKE_REPO_VERSION );
    root->AddChild( "Version" )->SetValue( CMAKE_ISEAFrameVERSION );
    std::stringstream timestamp;
    timestamp << std::chrono::duration_cast< std::chrono::seconds >( mStartTime.time_since_epoch() ).count();
    root->AddChild( "StartTime" )->SetValue( timestamp.str().c_str() );
    std::string xmlFile = simulationXml.GetFilename();
    if ( !xmlFile.empty() )
    {
        auto xmlElement = root->AddChild( "XML" );
        xmlparser::tinyxml2::XmlParserImpl simulationXmlParser;
        simulationXmlParser.ReadFromFile( xmlFile );
        try
        {
            auto metadataElement = simulationXmlParser.GetRoot()->GetElementChild( "MetaData" );
            xmlElement->AddChild( "OriginalHash" )->SetValue( metadataElement->GetElementStringValue( "Hash" ).c_str() );
            xmlElement->AddChild( "UUID" )->SetValue( metadataElement->GetElementStringValue( "UUID" ).c_str() );
        }
        catch ( std::logic_error )
        {
        }
        std::string actualHash = misc::GetXMLChecksum( simulationXmlParser );
        xmlElement->AddChild( "ActualHash" )->SetValue( actualHash.c_str() );
    }
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
MetadataFilter< T, TConcrete, ArgumentType >::~MetadataFilter()
{
    std::stringstream duration;
    duration << std::chrono::duration_cast< std::chrono::seconds >( std::chrono::system_clock::now() - mStartTime ).count();
    mParser.GetRoot()->AddChild( "Duration" )->SetValue( duration.str().c_str() );
    mParser.CreateXmlFile( mFilename );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void MetadataFilter< T, TConcrete, ArgumentType >::PrepareFilter( ArgumentType &prePareData )
{
    mParser.GetRoot()->AddChild( "UUID" )->SetValue( prePareData.mUUID.c_str() );
}

template < typename T >
using MetadataFilterTwoPort = MetadataFilter< T, electrical::TwoPort, PreparationType< T > >;
template < typename T >
using MetadataFilterThermal = MetadataFilter< T, thermal::ThermalElement, ThermalPreperation >;
template < typename T >
using MetadataFilterAging = MetadataFilter< T, aging::AgingTwoPort, PreparationType< T > >;

}    // namespace observer

#endif