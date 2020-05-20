/*
 * alreadyInitializedException.h
 *
 *  Created on: 27.11.2013
 *      Author: chris
 */

#ifndef ALREADYINITIALIZEDEXCEPTION_H_
#define ALREADYINITIALIZEDEXCEPTION_H_

#include<exception>

namespace systm
{

///Exception for a second, not allowed, initialization of a StateSystem or StateSystemGroup
class AlreadyInitializedException : public std::exception
{
public:
    AlreadyInitializedException() {}

    virtual ~AlreadyInitializedException() throw( ) {}

    virtual const char* what() const throw ( )
    {
        return "The StateSystemGroup has already been initialized";
    }
};

} /* namespace systm */

#endif /* ALREADYINITIALIZEDEXCEPTION_H_ */
