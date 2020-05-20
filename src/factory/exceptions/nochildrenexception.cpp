/*
 * nochildrenexception.cpp
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */
#ifdef __EXCEPTIONS__

#include "nochildrenexception.h"

namespace factory
{

NoChildrenException::NoChildrenException( const char* className )
{
	mErrorMsg = std::string("An instance of ") + std::string(className) + std::string(" has no children.");
}

const char* NoChildrenException::what() const throw ( )
{
	return mErrorMsg.c_str();
}

} /* namespace factory */
#endif /* __EXCEPTIONS__ */
