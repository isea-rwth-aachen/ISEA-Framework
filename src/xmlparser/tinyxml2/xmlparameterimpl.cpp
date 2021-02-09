// File: xmlparameterimpl.cpp
// Author: chris, Christopher Abele

#include "xmlparameterimpl.h"
#include "xmlparserimpl.h"

#include "../../cstring/strlcpy.h"
#include "../../cstring/strtok_rbsd.h"
#include "../../exceptions/error_proto.h"
#include "../../misc/charArrayCmp.h"

#include <boost/algorithm/string.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>

#include <iostream>

namespace xmlparser
{

namespace tinyxml2
{

using namespace ::tinyxml2;

XmlParameterImpl::XmlParameterImpl( XMLElement* nodePtr )
    : mNodePtr( nodePtr )
{
}

XmlParameterImpl::~XmlParameterImpl() {}

boost::shared_ptr< XmlParameter > XmlParameterImpl::GetConfigurationRoot() const
{
    XMLElement* root = mNodePtr->GetDocument()->FirstChildElement();
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( root ) );
}

boost::shared_ptr< XmlParameter > XmlParameterImpl::GetElementChild( const char* elementName ) const
{
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( GetRawElement( elementName ) ) );
}

boost::shared_ptr< XmlParameter > XmlParameterImpl::GetElementParent() const
{
    XMLElement* parent = mNodePtr->Parent()->ToElement();

    if ( !parent )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoParentNodeException", this->GetElementName() );

    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( parent ) );
}

std::vector< boost::shared_ptr< XmlParameter > > XmlParameterImpl::GetElementChildren( const char* elementName ) const
{
    std::vector< boost::shared_ptr< XmlParameter > > vec;

    std::vector< XMLElement* > children = GetRawElementChildren( elementName );
    typedef std::vector< XMLElement* >::iterator it_t;

    for ( it_t it = children.begin(); it != children.end(); ++it )
        vec.push_back( boost::shared_ptr< XmlParameter >( new XmlParameterImpl( *it ) ) );

    return vec;
}

std::vector< boost::shared_ptr< XmlParameter > > XmlParameterImpl::GetUnreferencedElementChildren( const char* elementName ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName )->FirstChildElement() : mNodePtr->FirstChildElement();

    std::vector< XMLElement* > children;

    while ( node )
    {
        children.push_back( node );
        node = node->NextSiblingElement();
    }


    std::vector< boost::shared_ptr< XmlParameter > > vec;

    typedef std::vector< XMLElement* >::iterator it_t;
    for ( it_t it = children.begin(); it != children.end(); ++it )
        vec.push_back( boost::shared_ptr< XmlParameter >( new XmlParameterImpl( *it ) ) );

    return vec;
}

bool XmlParameterImpl::HasElementDirectChild( const char* elementName ) const
{
    return GetRawElement( elementName, false ) != 0;
}

bool XmlParameterImpl::HasElementDirectChildWithAttribute( const char* attributeName, const char* attributeValue ) const
{
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = this->GetElementChildren();

    for ( const auto& child : children )
    {
        if ( child->HasElementAttribute( attributeName, attributeValue ) )
            return true;
    }

    return false;
}

bool XmlParameterImpl::HasElementAnyChild( const char* childName, const boost::shared_ptr< xmlparser::XmlParameter >& startNode ) const
{
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children;

    if ( !startNode )
        children = this->GetElementChildren();

    else
        children = startNode->GetElementChildren();

    for ( boost::shared_ptr< xmlparser::XmlParameter > child : children )
    {
        if ( strcmp( child->GetElementName(), childName ) == 0 )
            return true;

        else
        {
            if ( HasElementAnyChild( childName, child ) )
                return true;
        }
    }

    return false;
}

bool XmlParameterImpl::HasElementAnyChildWithAttribute( const char* attributeName, const char* attributeValue,
                                                        const boost::shared_ptr< xmlparser::XmlParameter >& startNode ) const
{
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children;

    if ( !startNode )
        children = this->GetElementChildren();

    else
        children = startNode->GetElementChildren();

    for ( boost::shared_ptr< xmlparser::XmlParameter >& child : children )
    {
        if ( child->HasElementAttribute( attributeName, attributeValue ) )
            return true;

        else
        {
            if ( HasElementAnyChildWithAttribute( attributeName, attributeValue, child ) )
                return true;
        }
    }

    return false;
}

bool XmlParameterImpl::HasElementAttribute( const char* attributeName, const char* attributeValue ) const
{
    const bool hasAttribute = HasAttribute( mNodePtr, attributeName );

    if ( attributeValue && hasAttribute )
    {
        if ( strcmp( this->GetElementAttributeCharPtrValue( attributeName ), attributeValue ) == 0 )
            return true;

        else
            return false;
    }

    return hasAttribute;
}

size_t XmlParameterImpl::GetNumberOfChildren() const { return this->GetElementChildren().size(); }

const char* XmlParameterImpl::GetElementName() const { return (const char*)mNodePtr->Name(); }

size_t XmlParameterImpl::GetLineNumber() const { return mNodePtr->GetLineNum(); }

bool XmlParameterImpl::GetElementBoolValue( const char* elementName, bool defaultValue ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName ) : mNodePtr;

    bool res;
    XMLError err = node->QueryBoolText( &res );

    if ( err )
    {
        if ( defaultValue )
            return defaultValue;

        else
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                               elementName, this->GetElementName() );
    }

    return res;
}

int XmlParameterImpl::GetElementIntValue( const char* elementName, int defaultValue ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName ) : mNodePtr;

    int res;
    XMLError err = node->QueryIntText( &res );

    if ( err )
    {
        if ( defaultValue )
            return defaultValue;

        else
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                               elementName, this->GetElementName() );
    }

    return res;
}

size_t XmlParameterImpl::GetElementUnsignedIntValue( const char* elementName, size_t defaultValue ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName ) : mNodePtr;

    int res;
    XMLError err = node->QueryIntText( &res );

    if ( err )
    {
        if ( defaultValue )
            return defaultValue;

        else
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                               elementName, this->GetElementName() );
    }

    return size_t( abs( res ) );
}

double XmlParameterImpl::GetElementDoubleValue( const char* elementName, double defaultValue ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName ) : mNodePtr;

    double res;
    XMLError err = node->QueryDoubleText( &res );

    if ( err )
    {
        if ( defaultValue )
            return defaultValue;

        else
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                               elementName, this->GetElementName() );
    }

    return res;
}

std::string XmlParameterImpl::GetElementStringValue( const char* elementName, const char* defaultValue ) const
{

    bool throwOnMiss = strlen( defaultValue ) == 0;
    XMLElement* node = elementName ? GetRawElement( elementName, throwOnMiss ) : mNodePtr;
    if ( !node && defaultValue != 0 )
    {
        return defaultValue;
    }

    const char* res = node->GetText();

    if ( res == 0 )
    {
        if ( defaultValue )
            res = defaultValue;

        else
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                               elementName, this->GetElementName() );
    }

    std::string returnValue( res );
    boost::trim( returnValue );

    return returnValue;
}

std::vector< size_t > XmlParameterImpl::GetElementUnsignedIntVecValue( const char* elementName ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName ) : mNodePtr;

    const char* ctmp = node->GetText();

    if ( ctmp == 0 )
        ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                           elementName, this->GetElementName() );

    std::vector< size_t > res;
    const size_t sizeOfString = strlen( ctmp ) + 1;
    boost::scoped_array< char > input( new char[sizeOfString] );
    strcpy( input.get(), ctmp );

    char* save_ptr;
    char* split = strtok_rbsd( input.get(), ",", &save_ptr );

    char* endOfString = 0;

    while ( split )
    {

        size_t value = std::abs( strtol( split, &endOfString, 10 ) );    // signed long integer, stored as unsigned by
                                                                         // abs, this way -2 = 2, valid numbers are
                                                                         // positive long ints;
        if ( endOfString == split && value == 0 )
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                               elementName, this->GetElementName() );

        if ( value == 0 && !TestStringHasChar( split, '0' ) )
            break;

        res.push_back( value );

        split = strtok_rbsd( 0, ",", &save_ptr );
    }

    return res;
}

std::vector< double > XmlParameterImpl::GetElementDoubleVecValue( const char* elementName ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName ) : mNodePtr;
    std::vector< double > res;

    const char* ctmp = node->GetText();
    if ( ctmp == 0 )
        ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                           elementName, this->GetElementName() );


    const size_t sizeOfString = strlen( ctmp ) + 1;
    boost::scoped_array< char > input( new char[sizeOfString] );
    strlcpy( input.get(), ctmp, sizeOfString );

    char* save_ptr;
    char* split = strtok_rbsd( input.get(), ",", &save_ptr );

    char* endOfString = 0;
    while ( split )
    {

        double value = strtod( split, &endOfString );    // atof( split );

        if ( endOfString == split && value == 0 )
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                               elementName, this->GetElementName() );


        if ( value == 0 && !TestStringHasChar( split, '0' ) )
            break;

        res.push_back( value );

        split = strtok_rbsd( 0, ",", &save_ptr );
    }

    return res;
}

std::vector< std::vector< double > > XmlParameterImpl::GetElementDoubleVecVecValue( const char* elementName ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName ) : mNodePtr;
    std::vector< std::vector< double > > res;

    const char* ctmp = node->GetText();

    if ( ctmp == 0 )
        ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                           elementName, this->GetElementName() );

    const size_t sizeOfString = strlen( ctmp ) + 1;
    boost::scoped_array< char > input( new char[sizeOfString] );
    strlcpy( input.get(), ctmp, sizeOfString );

    char* outer_save_ptr;
    char* inner_save_ptr;
    char* outersplit = strtok_rbsd( input.get(), ";", &outer_save_ptr );

    while ( outersplit )
    {
        res.push_back( std::vector< double >() );

        char* innersplit = strtok_rbsd( outersplit, ",", &inner_save_ptr );

        while ( innersplit )
        {
            char* endOfString = 0;
            double value = strtod( innersplit, &endOfString );    // atof( split );

            if ( endOfString == innersplit && value == 0 )
                ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "CouldNotParseXmlContentException",
                                                   elementName, this->GetElementName() );

            if ( value == 0 && !TestStringHasChar( innersplit, '0' ) )
                break;
            // double value = atof( innersplit );
            res.back().push_back( value );

            innersplit = strtok_rbsd( 0, ",", &inner_save_ptr );
        }
        if ( !TestStringHasChar( outer_save_ptr, ';' ) && !TestStringHasChar( outer_save_ptr, ',' ) )
            break;
        outersplit = strtok_rbsd( 0, ";", &outer_save_ptr );
    }

    return res;
}

const char* XmlParameterImpl::GetElementAttributeCharPtrValue( const char* attributeName ) const
{
    if ( !HasAttribute( mNodePtr, attributeName ) )
    {
        return 0;
    }
    return GetAttribute( mNodePtr, attributeName );
}

bool XmlParameterImpl::GetElementAttributeBoolValue( const char* attributeName, bool defaultValue ) const
{
    const char* ctmp = GetAttribute( mNodePtr, attributeName, false );

    if ( !ctmp )
    {
        if ( defaultValue )
            return defaultValue;
        else
            return false;
    }

    return !misc::CaseInsensitiveStringCompare( (const char*)ctmp, "TRUE" );
}

int XmlParameterImpl::GetElementAttributeIntValue( const char* attributeName, int defaultValue ) const
{
    const char* ctmp = GetAttribute( mNodePtr, attributeName, false );

    if ( !ctmp )
    {
        if ( defaultValue )
            return defaultValue;

        else
            return 0;
    }
    return atoi( GetAttribute( mNodePtr, attributeName ) );
}

size_t XmlParameterImpl::GetElementAttributeUnsignedIntValue( const char* attributeName, size_t defaultValue ) const
{
    const size_t attribute = size_t( abs( this->GetElementAttributeIntValue( attributeName, defaultValue ) ) );

    if ( attribute < 1 )
    {
        if ( defaultValue )
            return defaultValue;

        else
            return 0;
    }
    else
        return attribute;
}


double XmlParameterImpl::GetElementAttributeDoubleValue( const char* attributeName, double defaultValue ) const
{
    const char* ctmp = GetAttribute( mNodePtr, attributeName, false );

    if ( !ctmp )
    {
        if ( defaultValue )
            return defaultValue;

        else
            return 0;
    }
    return atof( GetAttribute( mNodePtr, attributeName ) );
}


// Private

XMLElement* XmlParameterImpl::GetRawElement( const char* elementName, bool throwOnMiss ) const
{
    XMLElement* node = mNodePtr->FirstChildElement( elementName );

    if ( !node && throwOnMiss )
        ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "NodeRequiredException", elementName,
                                           this->GetElementName() );

    if ( node && IsReference( node ) )
        node = GetReferencedRawElement( node );

    return node;
}

XMLElement* XmlParameterImpl::GetReferencedRawElement( XMLElement* node ) const
{
    const char* referencename = GetAttribute( node, "ref" );

    XMLElement* root = node->GetDocument()->FirstChildElement();
    std::vector< XMLElement* > customelems = XmlParameterImpl( root ).GetRawElementChildren( "CustomDefinitions" );

    typedef std::vector< XMLElement* >::iterator it_t;

    for ( it_t it = customelems.begin(); it != customelems.end(); ++it )
        if ( !misc::CaseInsensitiveStringCompare( (const char*)( *it )->Name(), referencename ) )
            return *it;

    ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "NodeRequiredException", node->Name(),
                                       this->GetElementName() );

    return 0;
}

std::vector< XMLElement* > XmlParameterImpl::GetRawElementChildren( const char* elementName ) const
{
    XMLElement* node = elementName ? GetRawElement( elementName )->FirstChildElement() : mNodePtr->FirstChildElement();

    std::vector< XMLElement* > vec;
    while ( node )
    {
        if ( IsReference( node ) )
            vec.push_back( GetReferencedRawElement( node ) );
        else
            vec.push_back( node );

        node = node->NextSiblingElement();
    }
    return vec;
}

bool XmlParameterImpl::IsReference( XMLElement* node ) const { return HasAttribute( node, "ref" ); }

bool XmlParameterImpl::HasAttribute( XMLElement* node, const char* attrName ) const
{
    const char* attr = node->Attribute( attrName );
    return attr != 0;
}

const char* XmlParameterImpl::GetAttribute( XMLElement* node, const char* attrName, bool throwOnMiss ) const
{
    const char* attr = node->Attribute( attrName );

    if ( !attr && throwOnMiss )
    {
        std::string parentName;

        try
        {
            parentName = this->GetElementParent()->GetElementName();
        }

        catch ( const std::logic_error& e )
        {
            parentName = "/";
        }

        ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "AttributeRequiredException",
                                           this->GetElementName(), parentName.c_str(), attrName );
    }
    return attr;
}

bool XmlParameterImpl::TestStringHasChar( const char* str, const char c ) const
{
    if ( !str )
        return false;

    const size_t length = strlen( str );

    for ( size_t i = 0; i < length; ++i )
    {
        if ( str[i] == c )
            return true;
    }

    return false;
}

void XmlParameterImpl::SetValue( const char* value )
{
    mNodePtr->SetText( value );
    // mNodePtr->GetDocument()->SaveFile();
}

void XmlParameterImpl::SetDoubleValue( const double& value )
{
    std::string text = std::to_string( value );
    mNodePtr->SetText( text.c_str() );
}

void XmlParameterImpl::SetDoubleVecValues( const std::vector< double >& values )
{
    std::string text;

    text += " ";

    for ( size_t i = 0; i < values.size(); i++ )
    {
        char str_tmp[50];
        snprintf( str_tmp, 50, "%.6f", values[i] );
        std::string valueString( str_tmp );

        text += valueString;

        if ( i == ( values.size() - 1 ) )
            text += " ";

        else
            text += ", ";
    }

    const char* ctext = text.c_str();

    mNodePtr->SetText( ctext );
    // mNodePtr->GetDocument()->SaveFile();
}

void XmlParameterImpl::SetUnsignedIntVecValues( const std::vector< size_t >& values )
{
    std::string text;

    for ( const auto& value : values )
    {
        text += std::to_string( value );

        if ( value != values.back() )
            text += ", ";
    }

    mNodePtr->SetText( text.c_str() );
    // mNodePtr->GetDocument()->SaveFile();
}

boost::shared_ptr< XmlParameter > XmlParameterImpl::AddChild( const char* childName )
{
    XMLElement* childElement = mNodePtr->GetDocument()->NewElement( childName );
    mNodePtr->InsertEndChild( childElement );
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( childElement ) );
}

boost::shared_ptr< XmlParameter > XmlParameterImpl::GetOrCreateElementChild( const char* childName )
{
    tinyxml2::XMLElement* child = GetRawElement( childName, false );
    if ( !child )
    {
        child = mNodePtr->GetDocument()->NewElement( childName );
        mNodePtr->InsertEndChild( child );
    }
    return boost::shared_ptr< XmlParameter >( new XmlParameterImpl( child ) );
}

void XmlParameterImpl::DeleteChildren() { mNodePtr->DeleteChildren(); }

void XmlParameterImpl::SaveDocument( const char* filename ) const { mNodePtr->GetDocument()->SaveFile( filename ); }

}    // End namespace tinyxml2

}    // End namespace xmlparser
