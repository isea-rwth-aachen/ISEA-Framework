/*
 * nochildrenexception.h
 *
 *  Created on: 09.10.2013
 *      Author: chris
 */

#ifndef NOCHILDRENEXCEPTION_H_
#define NOCHILDRENEXCEPTION_H_

#include <string>

namespace factory
{

/// This exception is thrown if a electrical::TwoPortWithChild has not got any children added.
class NoChildrenException : public std::exception
{
    public:
    NoChildrenException( const char* className );

    virtual ~NoChildrenException() throw() {}

    virtual const char* what() const throw();

    protected:
    std::string mErrorMsg;
};

} /* namespace factory */

#endif /* NOCHILDRENEXCEPTION_H_ */
