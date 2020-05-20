/*
 * classnotcachedexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifdef __EXCEPTIONS__
#include "classnotcachedexception.h"

namespace factory
{

ClassNotCachedException::ClassNotCachedException( const char* className )
{
	mErrorMsg = std::string("The Factory has not cached the requested class ") + std::string(className) + std::string(".");
}

const char* ClassNotCachedException::what() const throw ( )
{
	return mErrorMsg.c_str();
}


} /* namespace factory */
#endif /* __EXCEPTIONS__ */
