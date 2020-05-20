// File: aging_xml_validator.h
// Author: Christopher Abele

#ifndef _AGING_XML_VALIDATOR_
#define _AGING_XML_VALIDATOR_

#include "../../xmlparser/tinyxml2/xmlparameterimpl.h"


namespace validator
{

// Validation functions
void ValidateCalendarianAgingNode( const xmlparser::XmlParameter* calendarianNode );

} // End namespace validator

#endif // _AGING_XML_VALIDATOR_
