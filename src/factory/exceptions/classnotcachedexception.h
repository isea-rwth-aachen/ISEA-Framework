/*
 * classnotcachedexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef CLASSNOTCACHEDEXCEPTION_H_
#define CLASSNOTCACHEDEXCEPTION_H_

#ifdef __EXCEPTIONS__
#include <string>

namespace factory
{

///This exception is thrown if the desired class is not cached in the factory.
class ClassNotCachedException : public std::exception
{
public:
	ClassNotCachedException( const char* className );

	virtual ~ClassNotCachedException() throw( ) {}

	virtual const char* what() const throw ( );

protected:
	std::string mErrorMsg;
};

} /* namespace factory */

#endif /* __EXCEPTIONS__ */
#endif /* CLASSNOTCACHEDEXCEPTION_H_ */
