#ifndef _EXCEPTION_TESTER_
#define _EXCEPTION_TESTER_

#include <exception>
#include <cstddef>



///Returns 0 if comparing is suceesfull
size_t CmpInit(const std::exception &e, const char *message, const char *function);

///Returns 0 if comparing is suceesfull
size_t CmpUnexp(const std::exception &e, const char *message, const char *function);

///Returns 0 if comparing is suceesfull
size_t CmpParam(const std::exception &e, const char *message, const char *location = 0);

#endif
