/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : boolVecOper.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:21:23 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "boolVecOper.h"
#include <cstring>

bool Any(std::vector<bool> container)
{
    for( size_t i = 0 ; i < container.size() ; ++i )
    {
        if ( container[i] )
            return true;
    }
    return false;
}

bool All(std::vector<bool> container)
{
    for( size_t i = 0 ; i < container.size() ; ++i )
    {
        if ( !container[i] )
            return false;
    }
    return true;
}

size_t Sum(std::vector<bool> container)
{
    size_t counter = 0;
    for( size_t i = 0 ; i < container.size() ; ++i )
    {
        if ( container[i] )
            ++counter;
    }
    return counter;
}
