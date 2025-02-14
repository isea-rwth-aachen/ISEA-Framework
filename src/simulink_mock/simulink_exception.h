/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : simulink_exception.h
* Creation Date : 31-07-2014
* Last Modified : Fr 01 Aug 2014 12:38:44 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SIMULINK_EXCEPTION_
#define _SIMULINK_EXCEPTION_
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif


namespace simulink
{
void ErrorImpl(const char* function, const int line, const char* file, const char* message);

#ifdef __EXCEPTIONS__
/// DESCRIPTION
class SimulinkException: public std::exception
{
    public:
        SimulinkException(const char* function, const int line, const char* file, const char* message);

	virtual ~SimulinkException() throw( ) {}

	virtual const char* what() const throw ( );

    private:
        char mErrorMsg[4096];
};
#endif /* __EXCEPTIONS__ */
} /*simulink*/
#endif /* _SIMULINK_EXCEPTION_ */
