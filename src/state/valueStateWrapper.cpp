/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : valueStateWrapper.cpp
* Creation Date : 20-02-2014
* Last Modified : Do 20 Feb 2014 14:53:18 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "valueStateWrapper.h"

template class state::ValueStateWrapper< double >;

#ifdef _SYMBOLIC_
template class state::ValueStateWrapper< ScalarUnit >;

namespace state
{
template <>
double ValueStateWrapper< ScalarUnit >::GetValue() const
{
    return 0;
};    // Not supported

template <>
double ValueStateWrapper< ScalarUnit >::operator()() const
{
    return 0;
};    // Not supported
}

#endif /* _SYMBOLIC_ */
