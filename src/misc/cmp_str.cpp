#include "cmp_str.h"
#include "charArrayCmp.h"

namespace misc
{

bool cmp_str::operator()( const char *first,
                          const char *second ) const
{
    return CaseInsensitiveStringCompare( first, second ) < 0;
}

bool equal_str::operator()( const char *first,
                          const char *second ) const
{
    return CaseInsensitiveStringCompare( first, second ) == 0;
}

}
