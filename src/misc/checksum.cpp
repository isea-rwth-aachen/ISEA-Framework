#include "checksum.h"

#include "../xmlparser/tinyxml2/xmlparserimpl.h"

#include <boost/algorithm/hex.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <iomanip>
#include <type_traits>

namespace misc
{

using boost::uuids::detail::sha1;

std::string Checksum( const std::string &text )
{
    sha1 hash;
    hash.process_bytes( text.data(), text.size() );
    sha1::digest_type digest;
    hash.get_digest( digest );
    const char *charDigest = reinterpret_cast< const char * >( digest );
    std::stringstream result;
    int hashLength = sizeof( sha1::digest_type ) / sizeof( unsigned int );
    for ( unsigned int *i = digest; i < digest + hashLength; ++i )
    {
        result << std::setfill( '0' ) << std::setw( sizeof( unsigned int ) * 2 ) << std::hex << *i;
    }
    return result.str();
}

std::string GetXMLChecksum( xmlparser::XmlParser &parser )
{
    try
    {
        parser.GetRoot()->GetElementChild( "MetaData" )->GetElementChild( "Hash" )->DeleteChildren();
    }
    catch ( std::logic_error )
    {
    }
    std::string xmlString = parser.ToString();
    xmlString.erase( std::remove_if( xmlString.begin(), xmlString.end(),
                                     []( unsigned char x ) { return std::isspace( x ); } ),
                     xmlString.end() );
    return Checksum( xmlString );
}

std::string GetXMLChecksum( const std::string &filename )
{
    xmlparser::tinyxml2::XmlParserImpl parser;
    parser.ReadFromFile( filename );
    return GetXMLChecksum( parser );
}

std::string AddXMLChecksum( xmlparser::XmlParser &parser )
{
    std::string hash = GetXMLChecksum( parser );
    parser.GetRoot()->GetOrCreateElementChild( "MetaData" )->GetOrCreateElementChild( "Hash" )->SetValue( hash.c_str() );
    return hash;
}

std::string GetOriginalXMLChecksum( xmlparser::XmlParser &parser )
{
    try
    {
        return parser.GetRoot()->GetElementChild( "MetaData" )->GetElementStringValue( "Hash" );
    }
    catch ( std::logic_error )
    {
        return "";
    }
}

}    // namespace misc