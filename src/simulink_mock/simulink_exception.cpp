/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : simulink_exception.cpp
* Creation Date : 31-07-2014
* Last Modified : Mi 06 Aug 2014 11:13:17 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "simulink_exception.h"

#include <cstdlib>

#ifdef __EXCEPTIONS__
#include <boost/lexical_cast.hpp>
#include "../cstring/strlcat.h"
#include <string>
#endif

namespace simulink
{

void ErrorImpl(const char* function, const int line, const char* file, const char* message)
{
#ifdef __EXCEPTIONS__
    throw( SimulinkException(function,line, file, message));
#else
    abort();
#endif
}

#ifdef __EXCEPTIONS__
SimulinkException::SimulinkException(const char* function, const int line, const char* file, const char* message)
{
    const size_t maxSize = 4096;
    mErrorMsg[0] = '\0';
    strlcat(mErrorMsg, "Error in function ", maxSize);
    strlcat(mErrorMsg, function, maxSize);
    strlcat(mErrorMsg, ". The error occured in ", maxSize );
    strlcat(mErrorMsg, file, maxSize);
    strlcat(mErrorMsg, ":", maxSize);
    strlcat(mErrorMsg, boost::lexical_cast<std::string>(line).c_str(), maxSize);
    strlcat(mErrorMsg, "\n ", maxSize);
    strlcat(mErrorMsg, message, maxSize);
    strlcat(mErrorMsg, "\n ", maxSize);
}

const char* SimulinkException::what() const throw ( )
{
    return mErrorMsg;
}
#endif /*__EXCEPTIONS__ */

}
