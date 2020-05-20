/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : toLower.cpp
* Creation Date : 10-10-2013
* Last Modified : Do 10 Okt 2013 18:11:06 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include <cctype>
#include <cstring>
#include "toLower.h"

namespace misc
{
    boost::shared_array<char> ToLower(const char * input)
    {
        const size_t sizeOfInput = strlen(input);
        boost::shared_array<char> returnArray( new char[sizeOfInput+1]);
        for( size_t i = 0 ; i < sizeOfInput ; ++i )
        {
            returnArray[i] = tolower(input[i]);
        }
        returnArray[sizeOfInput] = 0;
        return returnArray;
    }
}
