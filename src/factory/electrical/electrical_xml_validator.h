// File: electrical_xml_validator.h
// Author: Christopher Abele

#ifndef _ELECTRICAL_XML_VALIDATOR_
#define _ELECTRICAL_XML_VALIDATOR_

#include "../../xmlparser/tinyxml2/xmlparameterimpl.h"


namespace validator
{

// Helpfunctions
void CheckForChildrenOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* parentNodeName );

void CheckForChildrenOrException( const std::initializer_list< boost::shared_ptr< xmlparser::XmlParameter > >& nodeList, const char* parentNodeName );

void CheckForAttributeOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* attributeName, const char* attributeValue, const char* parentNodeName );

void CheckForChildAttributeOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* attributeName, const char* attributeValue, const char* parentNodeName, const bool positiveCheck = true );

void CheckForChildAttributeOrException( const std::initializer_list< boost::shared_ptr< xmlparser::XmlParameter > >& nodeList, const char* attributeName, const char* attributeValue, const char* parentNodeName, const bool positiveCheck = true );

void CheckForChildAttributeOrException( const boost::shared_ptr< xmlparser::XmlParameter >& node, const char* attributeName, const std::initializer_list< const char* >& attributeValueList, const char* parentNodeName, const bool positiveCheck = true );

void CheckForEqualVectorSizeOrException( const std::initializer_list< std::vector< double > >& vectorList, const size_t size, const std::initializer_list< const char* >& nodeNames, const char* parentNodeName );


// Validation functions
void ValidateCellelementNode( const xmlparser::XmlParameter* cellelementNode, const size_t cellCounter, const bool aging );

void ValidateEletrodeElementNode( const xmlparser::XmlParameter* electrodeElementNode );

void ValidateAnodeElementNode( const xmlparser::XmlParameter* anodeElementNode, const size_t anodeElementCounter );

void ValidateTwoPortWithChild( const xmlparser::XmlParameter* twoPortWithChildNode );

void ValidateSerialTwoPortNode( const xmlparser::XmlParameter* serialTwoPortNode );

void ValidateParallelTwoPortNode( const xmlparser::XmlParameter* parallelTwoPortNode );

} // End namespace validator

#endif // _ELECTRICAL_XML_VALIDATOR_
