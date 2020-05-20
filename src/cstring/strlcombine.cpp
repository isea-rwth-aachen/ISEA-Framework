/*
 * strcombine.cpp
 *
 *  Created on: 29.08.2013
 *      Author: chris
 */

#include "strlcombine.h"
#include "strlcat.h"
#include <stdlib.h>

char* strlcombine(const char *src1, const char *src2, size_t siz)
{
    char* res = (char*)malloc(siz + 1);
    size_t tmplen = strlcat(res,src1,siz);
    strlcat(res + tmplen, src2, siz - tmplen);
    return res;
}
