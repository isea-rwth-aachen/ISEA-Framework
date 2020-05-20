/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : toLower.h
* Creation Date : 10-10-2013
* Last Modified : Do 10 Okt 2013 17:12:18 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TOLOWER_
#define _TOLOWER_
#include <boost/shared_array.hpp>

namespace misc
{
/// Converts the input string to a lower string which is given back as shared array
    boost::shared_array<char> ToLower(const char * input);
}
#endif /* _TOLOWER_ */
