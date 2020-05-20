/*
 * xmlparserimpl.h
 *
 *  Created on: 18.09.2013
 *      Author: chris
 */

#ifndef XMLPARSERIMPLTINY_H_
#define XMLPARSERIMPLTINY_H_

#include "../xmlparser.h"
#include "tinyxml2.h"

namespace xmlparser
{
namespace tinyxml2
{

/// Implementation of XmlParser based on libxml2
class XmlParserImpl : public xmlparser::XmlParser
{
    public:
    XmlParserImpl();

    virtual ~XmlParserImpl();

    void ReadFromFile( const char* fileName );
    void ReadFromFile( const std::string fileName );

    void ReadFromMem( const char* xmldata );

    boost::shared_ptr< XmlParameter > GetRoot();

    void CreateXmlFile( const char* filename );
    void CreateXmlFile( const std::string filename );

    void SaveFile();

    private:
    ::tinyxml2::XMLDocument mDoc;

    std::string mFilename;
};
}
}
#endif /* XMLPARSERIMPL_H_ */
