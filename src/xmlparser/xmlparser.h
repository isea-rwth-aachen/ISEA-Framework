/*
 * xmlparser.h
 *
 *  Created on: 18.09.2013
 *      Author: chris
 */

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

#include "xmlparameter.h"


/// This namespace contains XML related classes.
namespace xmlparser
{

/// Interface for a parser
class XmlParser
{
    public:
    virtual ~XmlParser() {}

    /// Read XML document from file
    virtual void ReadFromFile( const char* fileName ) = 0;
    virtual void ReadFromFile( const std::string fileName ) = 0;

    /// Returns the filename if the document was read from a file, or an empty string otherwise
    virtual const std::string& GetFilename() const = 0;

    /// Read XML document from memory
    virtual void ReadFromMem( const char* xmldata ) = 0;

    /// Returns the documents root.
    virtual boost::shared_ptr< XmlParameter > GetRoot() = 0;

    /// Creates a new root element in the document
    virtual boost::shared_ptr< XmlParameter > CreateRootElement( const char* name ) = 0;

    /// Creates a new XML File
    virtual void CreateXmlFile( const char* filename ) = 0;
    virtual void CreateXmlFile( const std::string filename ) = 0;

    /// Saves changes for the actual XML File
    virtual void SaveFile() = 0;

    /// Converts the XML document to a string
    virtual std::string ToString() = 0;
};
}    // namespace xmlparser

#endif /* XMLPARSER_H_ */
