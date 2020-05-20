/*
 * attributenotfoundexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__

#include "attributenotfoundexception.h"
#include <boost/lexical_cast.hpp>

namespace xmlparser
{

AttributeNotFoundException::AttributeNotFoundException( const char* attributeName,
                                                        const char* elementName,
                                                        size_t      lineCount )
{
	mErrorMsg = std::string("The Element ") + std::string(elementName) + std::string(" has no attribute ") + std::string(attributeName) + std::string(". Line: ") + boost::lexical_cast<std::string>(lineCount) + std::string(".");
}

const char* AttributeNotFoundException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
