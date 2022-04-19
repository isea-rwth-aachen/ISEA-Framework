// File: xmlparameter.h
// Author: chris, Christopher Abele

#ifndef XMLPARAMETER_H_
#define XMLPARAMETER_H_

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#ifdef __EXCEPTIONS__
#include "exceptions/attributenotfoundexception.h"
#include "exceptions/configfilenotfoundexception.h"
#include "exceptions/couldnotparseconfigexception.h"
#include "exceptions/couldnotreadelementcontentexception.h"
#include "exceptions/elementnotfoundexception.h"
#endif

namespace xmlparser
{

class XmlParameter;

/// Interface for a parameter node
class XmlParameter
{
    public:
    virtual ~XmlParameter() {}

    /* Navigation through the xml file */

    /// Returns the documents root node
    virtual boost::shared_ptr< XmlParameter > GetConfigurationRoot() const = 0;

    /// Finds a node with the given name. Throws exception.
    virtual boost::shared_ptr< XmlParameter > GetElementChild( const char* elementName = nullptr ) const = 0;

    /// Returns the parent node of the underlying node. Throws exception.
    virtual boost::shared_ptr< XmlParameter > GetElementParent() const = 0;

    /// Returns all children of a node with the given name. Throws exception.
    virtual std::vector< boost::shared_ptr< XmlParameter > > GetElementChildren( const char* elementName = nullptr ) const = 0;

    /// Returns all children of a node without referencing functionality with the given name. Throws exception.
    virtual std::vector< boost::shared_ptr< XmlParameter > >
    GetUnreferencedElementChildren( const char* elementName = nullptr ) const = 0;


    /* General Information about a xml node */

    /// Checks if the underlying node has a direct child with a particular name
    virtual bool HasElementDirectChild( const char* childName ) const = 0;

    /// Checks if the underlying node has a direct child with a particular attribute
    virtual bool HasElementDirectChildWithAttribute( const char* attributeName, const char* attributeValue = nullptr ) const = 0;

    /// Checks if the underlying node or the given startNode has a child/grandchild etc. with a particular name
    virtual bool HasElementAnyChild( const char* childName,
                                     const boost::shared_ptr< xmlparser::XmlParameter >& startNode = nullptr ) const = 0;

    /// Checks if the underlying node or the given startNode has a child/grandchild etc. with a particular attribute
    virtual bool HasElementAnyChildWithAttribute( const char* attributeName, const char* attributeValue,
                                                  const boost::shared_ptr< xmlparser::XmlParameter >& start = nullptr ) const = 0;

    /// Checks if the underlying node has an attribute with the given name
    virtual bool HasElementAttribute( const char* attributeName, const char* attributeValue = nullptr ) const = 0;

    /// Returns the number of the children of the underlying node
    virtual size_t GetNumberOfChildren() const = 0;

    /// Returns the name of the underlying node
    virtual const char* GetElementName() const = 0;

    /// Gets line number of the underlying node in xml-file
    virtual size_t GetLineNumber() const = 0;


    /* Accessing the content of the node */

    /// Finds a node with the given name and returns its value as bool
    virtual bool GetElementBoolValue( const char* elementName = nullptr, bool defaultValue = false ) const = 0;

    /// Finds a node with the given name and returns its value as double. Returns default value if node is not found.
    virtual double GetElementDoubleValue( const char* elementName, double defaultValue ) const = 0;
    /// Finds a node with the given name and returns its value as double. Throws ParameterNotFoundException.
    virtual double GetElementDoubleValue( const char* elementName = 0 ) const = 0;

    /// Finds a node with the given name and returns its value as int. Throws ParameterNotFoundException
    virtual int GetElementIntValue( const char* elementName = nullptr, int defaultValue = 0 ) const = 0;

    /// Finds a node with the given name and returns its value as size_t. Throws ParameterNotFoundException
    virtual size_t GetElementUnsignedIntValue( const char* elementName = nullptr, size_t defaultValue = 0 ) const = 0;

    /// Finds a node with the given name and returns its value as std::string and trimmed. Throws
    /// ParameterNotFoundException
    virtual std::string GetElementStringValue( const char* elementName = nullptr, const char* defaultValue = "" ) const = 0;

    /// Finds a node with the given name and returns its value as std::vector of int. Throws ParameterNotFoundException
    virtual std::vector< size_t > GetElementUnsignedIntVecValue( const char* elementName = nullptr ) const = 0;

    /// Finds a node with the given name and returns its value as std::vector of double. Throws
    /// ParameterNotFoundException
    virtual std::vector< double > GetElementDoubleVecValue( const char* elementName = nullptr ) const = 0;

    /// Finds a node with the given name and returns its value as std::vector of std::vector of double. Throws
    /// ParameterNotFoundException
    virtual std::vector< std::vector< double > > GetElementDoubleVecVecValue( const char* elementName = nullptr ) const = 0;


    /* Accessing the attribute of the node */

    /// Finds an attribute with the given name and return its value. Throws AttributeNotFoundException
    virtual const char* GetElementAttributeCharPtrValue( const char* attributeName ) const = 0;

    /// Finds an attribute with the given name and return its value as bool. Returns default value if attribute is not found.
    virtual bool GetElementAttributeBoolValue( const char* attributeName, bool defaultValue ) const = 0;
    /// Finds an attribute with the given name and return its value as bool. Throws exception if attribute is not found.
    virtual bool GetElementAttributeBoolValue( const char* attributeName ) const = 0;

    /// Finds an attribute with the given name and return its value as int. Returns default value if attribute is not found.
    virtual int GetElementAttributeIntValue( const char* attributeName, int defaultValue ) const = 0;
    /// Finds an attribute with the given name and return its value as int. Throws exception if attribute is not found.
    virtual int GetElementAttributeIntValue( const char* attributeName ) const = 0;

    /// Finds an attribute with the given name and return its value as size_t. Returns default value if attribute is not found.
    virtual size_t GetElementAttributeUnsignedIntValue( const char* attributeName, size_t defaultValue ) const = 0;
    /// Finds an attribute with the given name and return its value as size_t. Throws exception if attribute is not found.
    virtual size_t GetElementAttributeUnsignedIntValue( const char* attributeName ) const = 0;

    /// Finds an attribute with the given name and return its value as double. Returns default value if attribute is not found.
    virtual double GetElementAttributeDoubleValue( const char* attributeName, double defaultValue ) const = 0;
    /// Finds an attribute with the given name and return its value as double. Throws exception if attribute is not found.
    virtual double GetElementAttributeDoubleValue( const char* attributeName ) const = 0;


    /* Writing to the xml file */

    /// Writes a const char* value to the xml file
    virtual void SetValue( const char* value ) = 0;

    /// Writes a double value to the xml file
    virtual void SetDoubleValue( const double& value ) = 0;

    /// Writes the double values from a std::vector<double> to the xml file
    virtual void SetDoubleVecValues( const std::vector< double >& values ) = 0;

    /// Writes the size_t values from a std::vector<size_t> to the xml file
    virtual void SetUnsignedIntVecValues( const std::vector< size_t >& values ) = 0;

    /// Adds a child at the end of the element and returns the new child
    virtual boost::shared_ptr< XmlParameter > AddChild( const char* childName ) = 0;

    /// Returns the child with the given name if it exists, or adds a new child and returns it
    virtual boost::shared_ptr< XmlParameter > GetOrCreateElementChild( const char* childName ) = 0;

    /// Removes all children from the element
    virtual void DeleteChildren() = 0;

    /// Writes the whole document to a file
    virtual void SaveDocument( const char* filename ) const = 0;
};

}    // namespace xmlparser

#endif    // XMLPARAMETER_H_
