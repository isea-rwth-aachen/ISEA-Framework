/*
 * xmlparserimpl.cpp
 *
 *  Created on: 18.09.2013
 *      Author: chris
 */

#include "xmlparserimpl.h"
#include "xmlparameterimpl.h"

namespace xmlparser
{
namespace tinyxml2
{

using namespace ::tinyxml2;

XmlParserImpl::XmlParserImpl()
    : mFilename( "" )
{
}

XmlParserImpl::~XmlParserImpl() { mDoc.Clear(); }

void XmlParserImpl::ReadFromFile( const char* fileName )
{
    if ( !fileName )
    {
#ifdef __EXCEPTIONS__
        throw( ConfigFileNotFoundException( "Memory" ) );
#endif
        abort();
    }

    XMLError err = mDoc.LoadFile( fileName );

#ifdef __EXCEPTIONS__
    switch ( err )
    {
        case XML_SUCCESS:
            break;

        case XML_ERROR_FILE_NOT_FOUND:
        case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
        case XML_ERROR_FILE_READ_ERROR:
            throw( ConfigFileNotFoundException( fileName ) );

        case XML_ERROR_PARSING_ELEMENT:
            throw( CouldNotParseConfigException( "PARSING_ELEMENT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_ATTRIBUTE:
            throw( CouldNotParseConfigException( "PARSING_ATTRIBUTE", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_TEXT:
            throw( CouldNotParseConfigException( "PARSING_TEXT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_CDATA:
            throw( CouldNotParseConfigException( "PARSING_CDATA", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_COMMENT:
            throw( CouldNotParseConfigException( "PARSING_COMMENT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_DECLARATION:
            throw( CouldNotParseConfigException( "PARSING_DECLARATION", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_UNKNOWN:
            throw( CouldNotParseConfigException( "PARSING_UNKNOWN", mDoc.ErrorLineNum() ) );
        case XML_ERROR_EMPTY_DOCUMENT:
            throw( CouldNotParseConfigException( "EMPTY_DOCUMENT" ) );
        case XML_ERROR_MISMATCHED_ELEMENT:
            throw( CouldNotParseConfigException( "MISMATCHED_ELEMENT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING:
            throw( CouldNotParseConfigException( "PARSING", mDoc.ErrorLineNum() ) );

        default:
        {
            throw( CouldNotParseConfigException() );
        }
    }
#else
    if ( err )
        abort();
#endif

    mFilename = fileName;
}

void XmlParserImpl::ReadFromFile( const std::string fileName ) { ReadFromFile( fileName.c_str() ); }

const std::string& XmlParserImpl::GetFilename() const { return mFilename; }

void XmlParserImpl::ReadFromMem( const char* xmlData )
{
    if ( !xmlData )
    {
#ifdef __EXCEPTIONS__
        throw( ConfigFileNotFoundException( "Memory" ) );
#endif
        abort();
    }

    XMLError err = mDoc.Parse( xmlData, strlen( xmlData ) );

#ifdef __EXCEPTIONS__
    switch ( err )
    {
        case XML_SUCCESS:
            break;

        case XML_ERROR_FILE_NOT_FOUND:
        case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
        case XML_ERROR_FILE_READ_ERROR:
            throw( ConfigFileNotFoundException( "Memory" ) );

        case XML_ERROR_PARSING_ELEMENT:
            throw( CouldNotParseConfigException( "PARSING_ELEMENT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_ATTRIBUTE:
            throw( CouldNotParseConfigException( "PARSING_ATTRIBUTE", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_TEXT:
            throw( CouldNotParseConfigException( "PARSING_TEXT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_CDATA:
            throw( CouldNotParseConfigException( "PARSING_CDATA", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_COMMENT:
            throw( CouldNotParseConfigException( "PARSING_COMMENT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_DECLARATION:
            throw( CouldNotParseConfigException( "PARSING_DECLARATION", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING_UNKNOWN:
            throw( CouldNotParseConfigException( "PARSING_UNKNOWN", mDoc.ErrorLineNum() ) );
        case XML_ERROR_EMPTY_DOCUMENT:
            throw( CouldNotParseConfigException( "EMPTY_DOCUMENT" ) );
        case XML_ERROR_MISMATCHED_ELEMENT:
            throw( CouldNotParseConfigException( "MISMATCHED_ELEMENT", mDoc.ErrorLineNum() ) );
        case XML_ERROR_PARSING:
            throw( CouldNotParseConfigException( "PARSING", mDoc.ErrorLineNum() ) );

        default:
        {
            throw( CouldNotParseConfigException() );
        }
    }
#else
    if ( err )
        abort();
#endif
}

boost::shared_ptr< XmlParameter > XmlParserImpl::GetRoot()
{
    XMLElement* root = mDoc.FirstChildElement();

    if ( !root )
    {

#ifdef __EXCEPTIONS__
        throw( CouldNotParseConfigException() );
#endif
        abort();
    }

    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( root ) );
}

boost::shared_ptr< XmlParameter > XmlParserImpl::CreateRootElement( const char* name )
{
    XMLElement* newElement = mDoc.NewElement( name );
    mDoc.InsertEndChild( newElement );
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( newElement ) );
}

void XmlParserImpl::CreateXmlFile( const char* filename )
{
    std::string sFilename( filename );
    size_t found = sFilename.find( ".xml" );


    if ( found == std::string::npos )
    {
        sFilename += ".xml";
    }

    if ( !mDoc.RootElement() )
    {
        mDoc.LinkEndChild( mDoc.NewDeclaration() );
    }

    mFilename = sFilename;

    mDoc.SaveFile( sFilename.c_str() );
}

void XmlParserImpl::CreateXmlFile( const std::string filename ) { CreateXmlFile( filename.c_str() ); }

void XmlParserImpl::SaveFile() { mDoc.SaveFile( mFilename.c_str() ); }

std::string XmlParserImpl::ToString()
{
    XMLPrinter printer;
    mDoc.Print( &printer );
    return std::string( printer.CStr() );
}

}    // namespace tinyxml2
} /* namespace xmlparser */
