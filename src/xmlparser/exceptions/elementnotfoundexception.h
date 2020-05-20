/*
 * elementnotfoundexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef ELEMENTNOTFOUNDEXCEPTION_H_
#define ELEMENTNOTFOUNDEXCEPTION_H_

#include <string>

namespace xmlparser
{

///Exception for a not found XMLParameter
class ElementNotFoundException : public std::exception
{
public:
	ElementNotFoundException( const char* childElementName,
	                          const char* elementName,
	                          size_t      lineCount );

	virtual ~ElementNotFoundException() throw( ) {}

	virtual const char* what() const throw ( );

protected:
	std::string mErrorMsg;
};

} /* namespace xmlparser */

#endif /* ELEMENTNOTFOUNDEXCEPTION_H_ */
