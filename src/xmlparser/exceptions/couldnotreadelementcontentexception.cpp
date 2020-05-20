/*
 * couldnotreadelementcontentexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__
#include "couldnotreadelementcontentexception.h"
#include <boost/lexical_cast.hpp>

namespace xmlparser
{

CouldNotReadElementContentException::CouldNotReadElementContentException( const char* expectedType,
                                                                          const char* elementName,
                                                                          size_t      lineCount )
{
	mErrorMsg = std::string("The Element ") + std::string(elementName) + std::string(" has not the correct content: ") + std::string(expectedType) + std::string(". Line: ") +  boost::lexical_cast<std::string>(lineCount) + std::string(".");
}

const char* CouldNotReadElementContentException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
