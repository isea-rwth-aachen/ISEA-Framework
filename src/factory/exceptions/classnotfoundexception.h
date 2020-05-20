/*
 * classnotfoundexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef CLASSNOTFOUNDEXCEPTION_H_
#define CLASSNOTFOUNDEXCEPTION_H_

#ifdef __EXCEPTIONS__
#include <string>

namespace factory
{

///This exception is thrown if the desired class is not registered in the factory.
class ClassNotFoundException : public std::exception
{
public:
	ClassNotFoundException( const char* className );

	virtual ~ClassNotFoundException() throw( ) {}

	virtual const char* what() const throw ( );

protected:
	std::string mErrorMsg;
};

} /* namespace factory */

#endif /* __EXCEPTIONS__ */
#endif /* CLASSNOTFOUNDEXCEPTION_H_ */
