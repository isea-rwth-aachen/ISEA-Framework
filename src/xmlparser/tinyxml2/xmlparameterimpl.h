// File: xmlparserimpl.h
// Author: chris, Christopher Abele

#ifndef XMLPARAMETERRIMPLTINY_H_
#define XMLPARAMETERRIMPLTINY_H_

#include "../../../libraries/tinyxml2/tinyxml2.h"
#include "../xmlparameter.h"

namespace xmlparser
{

namespace tinyxml2
{

/// Implementation of XMLParameter based on tinyxml2
class XmlParameterImpl : public xmlparser::XmlParameter
{
    public:
    XmlParameterImpl( ::tinyxml2::XMLElement* nodePtr );

    virtual ~XmlParameterImpl();


    // Navigation through xml file

    boost::shared_ptr< XmlParameter > GetConfigurationRoot() const;

    boost::shared_ptr< XmlParameter > GetElementChild( const char* elementName ) const;

    boost::shared_ptr< XmlParameter > GetElementParent() const;

    std::vector< boost::shared_ptr< XmlParameter > > GetElementChildren( const char* elementName = 0 ) const;

    std::vector< boost::shared_ptr< XmlParameter > > GetUnreferencedElementChildren( const char* elementName = 0 ) const;


    // General information about a xml node

    bool HasElementDirectChild( const char* childName ) const;

    bool HasElementDirectChildWithAttribute( const char* attributeName, const char* attributeValue = 0 ) const;

    bool HasElementAnyChild( const char* childName, const boost::shared_ptr< xmlparser::XmlParameter >& startNode = 0 ) const;

    bool HasElementAnyChildWithAttribute( const char* attributeName, const char* attributeValue,
                                          const boost::shared_ptr< xmlparser::XmlParameter >& start = 0 ) const;

    bool HasElementAttribute( const char* attributeName, const char* attributeValue = 0 ) const;

    size_t GetNumberOfChildren() const;

    const char* GetElementName() const;

    size_t GetLineNumber() const;


    // Accessing the content of the node

    bool GetElementBoolValue( const char* elementName = 0, bool defaultValue = false ) const;

    double GetElementDoubleValue( const char* elementName = 0 ) const;

    double GetElementDoubleValue( const char* elementName, double defaultValue ) const;

    int GetElementIntValue( const char* elementName = 0, int defaultValue = 0 ) const;

    size_t GetElementUnsignedIntValue( const char* elementName = 0, size_t defaultValue = 0 ) const;

    std::string GetElementStringValue( const char* elementName = 0, const char* defaultValue = "" ) const;

    std::vector< size_t > GetElementUnsignedIntVecValue( const char* elementName = 0 ) const;

    std::vector< double > GetElementDoubleVecValue( const char* elementName = 0 ) const;

    std::vector< std::vector< double > > GetElementDoubleVecVecValue( const char* elementName = 0 ) const;


    // Accessing the attribute of the node

    const char* GetElementAttributeCharPtrValue( const char* attributeName ) const;

    bool GetElementAttributeBoolValue( const char* attributeName, bool defaultValue ) const;
    bool GetElementAttributeBoolValue( const char* attributeName ) const;

    int GetElementAttributeIntValue( const char* attributeName, int defaultValue ) const;
    int GetElementAttributeIntValue( const char* attributeName ) const;

    size_t GetElementAttributeUnsignedIntValue( const char* attributeName, size_t defaultValue ) const;
    size_t GetElementAttributeUnsignedIntValue( const char* attributeName ) const;

    double GetElementAttributeDoubleValue( const char* attributeName, double defaultValue ) const;
    double GetElementAttributeDoubleValue( const char* attributeName ) const;


    // Writing to the xml file

    void SetValue( const char* value );

    void SetDoubleValue( const double& value );

    void SetDoubleVecValues( const std::vector< double >& values );

    void SetUnsignedIntVecValues( const std::vector< size_t >& values );

    boost::shared_ptr< XmlParameter > AddChild( const char* childName );

    boost::shared_ptr< XmlParameter > GetOrCreateElementChild( const char* childName );

    void DeleteChildren();

    void SaveDocument( const char* filename ) const;

    private:
    ::tinyxml2::XMLElement* GetRawElement( const char* elementName, bool throwOnMiss = true ) const;

    ::tinyxml2::XMLElement* GetReferencedRawElement( ::tinyxml2::XMLElement* node ) const;

    std::vector< ::tinyxml2::XMLElement* > GetRawElementChildren( const char* elementName ) const;

    bool IsReference( ::tinyxml2::XMLElement* node ) const;

    bool HasAttribute( ::tinyxml2::XMLElement* param, const char* attrName ) const;

    const char* GetAttribute( ::tinyxml2::XMLElement* param, const char* attrName, bool throwOnMiss = true ) const;

    bool TestStringHasChar( const char* str, const char c ) const;

    ::tinyxml2::XMLElement* mNodePtr;
};
}    // namespace tinyxml2

}    // namespace xmlparser

#endif    // XMLPARAMETERRIMPL_H_
