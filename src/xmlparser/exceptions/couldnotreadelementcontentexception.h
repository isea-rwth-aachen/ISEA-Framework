/*
 * couldnotreadelementcontentexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef COULDNOTREADELEMENTCONTENTEXCEPTION_H_
#define COULDNOTREADELEMENTCONTENTEXCEPTION_H_

#ifdef __EXCEPTIONS__
#include <string>

namespace xmlparser
{

///Exception for an element without expected content (string)
class CouldNotReadElementContentException : public std::exception
{
public:
	CouldNotReadElementContentException( const char* expectedType,
	                                     const char* elementName,
	                                     size_t      lineCount );

	virtual ~CouldNotReadElementContentException() throw( ) {}

	virtual const char* what() const throw ( );

protected:
	std::string mErrorMsg;
};

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
#endif /* COULDNOTREADELEMENTCONTENTEXCEPTION_H_ */
