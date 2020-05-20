/*
 * configfilenotfoundexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__
#include "configfilenotfoundexception.h"
#include <boost/lexical_cast.hpp>

namespace xmlparser
{

ConfigFileNotFoundException::ConfigFileNotFoundException( const char* fileName )
{
	mErrorMsg = std::string("The following file could not be read/found by the parser: ") + std::string(fileName) + std::string(".");
}

const char* ConfigFileNotFoundException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
