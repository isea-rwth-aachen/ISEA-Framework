/*
 * couldnotparseconfigexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__
#include "couldnotparseconfigexception.h"
#include <boost/lexical_cast.hpp>

namespace xmlparser
{

CouldNotParseConfigException::CouldNotParseConfigException()
	: mErrorMsg( "The configuration could not be parsed" )
{ }

CouldNotParseConfigException::CouldNotParseConfigException( const char* errorDescription,
                                                            size_t      lineCount )
{
	mErrorMsg = std::string("The following error ") + std::string(errorDescription) + std::string("did occur. Line: ") + boost::lexical_cast<std::string>(lineCount) + std::string(".");
}

const char* CouldNotParseConfigException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
