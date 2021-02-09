/*
 * xmlparserimpl.h
 *
 *  Created on: 18.09.2013
 *      Author: chris
 */

#ifndef XMLPARSERIMPLTINY_H_
#define XMLPARSERIMPLTINY_H_

#include "../../../libraries/tinyxml2/tinyxml2.h"
#include "../xmlparser.h"

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

    const std::string& GetFilename() const;

    void ReadFromMem( const char* xmldata );

    boost::shared_ptr< XmlParameter > GetRoot();
    boost::shared_ptr< XmlParameter > CreateRootElement( const char* name );

    void CreateXmlFile( const char* filename );
    void CreateXmlFile( const std::string filename );

    void SaveFile();

    std::string ToString();

    private:
    ::tinyxml2::XMLDocument mDoc;

    std::string mFilename;
};
}    // namespace tinyxml2
}    // namespace xmlparser
#endif /* XMLPARSERIMPL_H_ */
