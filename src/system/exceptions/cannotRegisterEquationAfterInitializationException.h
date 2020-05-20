/*
 * cannotRegisterEquationAfterInitializationException.h
 *
 *  Created on: 27.11.2013
 *      Author: chris
 */

#ifndef CANNOTREGISTEREQUATIONAFTERINITIALIZATIONEXCEPTION_H_
#define CANNOTREGISTEREQUATIONAFTERINITIALIZATIONEXCEPTION_H_

#include<exception>

namespace systm
{

///Exception for registering a new equation after the initialization of a StateSystem
class CannotRegisterEquationAfterInitializationException : public std::exception
{
public:
    CannotRegisterEquationAfterInitializationException() {}

    virtual ~CannotRegisterEquationAfterInitializationException() throw( ) {}

    virtual const char* what() const throw ( )
    {
        return "Cannot register a new equation after the system is initialized";
    }
};

} /* namespace systm */

#endif /* CANNOTREGISTEREQUATIONAFTERINITIALIZATIONEXCEPTION_H_ */
