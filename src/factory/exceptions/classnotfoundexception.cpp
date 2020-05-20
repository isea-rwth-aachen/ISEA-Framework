/*
 * classnotfoundexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__

#include "classnotfoundexception.h"

namespace factory
{

ClassNotFoundException::ClassNotFoundException( const char* className )
{
	mErrorMsg = std::string("The Factory could not find the requested class ") + std::string(className) + std::string(".");
}

const char* ClassNotFoundException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace factory */

#endif /* __EXCEPTIONS__ */
