#ifndef _CHECKSUM_
#define _CHECKSUM_

#include "../xmlparser/xmlparser.h"

#include <string>

namespace misc
{

/// generate the hash of the given string
std::string Checksum( const std::string &text );

/// generate the hash of the given XML file in the same way that the hash value stored in the XML file is generated
std::string GetXMLChecksum( xmlparser::XmlParser &parser );
std::string GetXMLChecksum( const std::string &filename );

/// generate the hash of the given XML file, add it to the file and return it
std::string AddXMLChecksum( xmlparser::XmlParser &parser );

/// get the hash value that was originally written to the XML file. If no hash is found an empty string is returned.
std::string GetOriginalXMLChecksum( xmlparser::XmlParser &parser );

}    // namespace misc

#endif