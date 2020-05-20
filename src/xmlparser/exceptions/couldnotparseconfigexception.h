/*
 * couldnotparseconfigexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef COULDNOTPARSECONFIGEXCEPTION_H_
#define COULDNOTPARSECONFIGEXCEPTION_H_

#ifdef __EXCEPTIONS__
#include <string>

namespace xmlparser
{

///Exception for not being able to parse the xmldocument
class CouldNotParseConfigException : public std::exception
{
public:
	CouldNotParseConfigException();

	CouldNotParseConfigException( const char* errorDescription,
	                              size_t      lineCount = 0 );

	virtual ~CouldNotParseConfigException() throw( ) {}

	virtual const char* what() const throw ( );

protected:
	std::string mErrorMsg;
};

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
#endif /* COULDNOTPARSECONFIGEXCEPTION_H_ */
