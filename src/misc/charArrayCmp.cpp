/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : charArrayCmp.cpp
* Creation Date : 10-10-2013
* Last Modified : Fr 08 Nov 2013 16:33:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include <cstring>
#include <algorithm>
#include "toLower.h"

#include "charArrayCmp.h"

namespace misc
{
    int CaseInsensitiveStringCompare(const char* first,  const char* second)
    {
        boost::shared_array<char> firstLower = misc::ToLower(first);
        boost::shared_array<char> secondLower = misc::ToLower(second);
        return strcmp(firstLower.get(), secondLower.get());
    }

}
