/*
 * elementnotfoundexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__
#include "elementnotfoundexception.h"
#include <boost/lexical_cast.hpp>

namespace xmlparser
{

ElementNotFoundException::ElementNotFoundException( const char* childElementName,
                                                    const char* elementName,
                                                    size_t      lineCount )
{
	mErrorMsg = std::string("The Element ") + std::string(childElementName) + std::string(" could not be found within element ") + std::string(elementName) + std::string(". Line: ") +  boost::lexical_cast<std::string>(lineCount) + std::string(".");
}

const char* ElementNotFoundException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace xmlparser */
#endif /* __EXCEPTIONS__ */
