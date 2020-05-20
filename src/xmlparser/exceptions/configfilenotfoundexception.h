/*
 * configfilenotfoundexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef CONFIGFILENOTFOUNDEXCEPTION_H_
#define CONFIGFILENOTFOUNDEXCEPTION_H_

#ifdef __EXCEPTIONS__
#include <string>

namespace xmlparser
{

///This namespace contains XML related classes.
class ConfigFileNotFoundException : public std::exception
{
public:
	ConfigFileNotFoundException( const char* fileName );

	virtual ~ConfigFileNotFoundException() throw( ) {}

	virtual const char* what() const throw ( );

protected:
	std::string mErrorMsg;
};

} /* namespace xmlparser */

#endif /* __EXCEPTIONS__ */
#endif /* CONFIGFILENOTFOUNDEXCEPTION_H_ */
