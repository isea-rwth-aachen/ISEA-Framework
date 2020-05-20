/*
 * attributenotfoundexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef ATTRIBUTENOTFOUNDEXCEPTION_H_
#define ATTRIBUTENOTFOUNDEXCEPTION_H_

#ifdef __EXCEPTIONS__

#include <string>

namespace xmlparser
{

///Exception for a not found XLAttribute
class AttributeNotFoundException : public std::exception
{
public:
	AttributeNotFoundException( const char* attributeName,
	                            const char* elementName,
	                            size_t      lineCount );

	virtual ~AttributeNotFoundException() throw( ) {}

	virtual const char* what() const throw ( );

protected:
	std::string mErrorMsg;
};

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */

#endif /* ATTRIBUTENOTFOUNDEXCEPTION_H_ */
