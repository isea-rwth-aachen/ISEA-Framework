/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : crate.h
* Creation Date : 08-07-2022
* Last Modified : Fr 8 Jul 2022 13:50 CET
* Created By : Jonas Rinner
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _Cdirection_
#define _Cdirection_

// STD
#include <iostream>
#include <vector>

// ETC
#include "../exceptions/error_proto.h"

#include "dgl_state.h"

#include <limits>
#include <vector>


#include "../misc/macros.h"
#include "../misc/matrixInclude.h"

// class TestCrate;
// class TestXML;

/*namespace electrical
{
template < typename T >
class TwoPort;
}*/

namespace state
{
template < typename T = double >
class CDirection : public State
{
    public:
    CDirection();
    virtual ~CDirection();

    T GetValue() const;
    T GetLastValue() const;
    void SetValue( const double value );

    T mActualCDirection;
    T mLastCDirection;
};

template < typename T >
CDirection< T >::CDirection()
    : mActualCDirection( -1 ){};

template < typename T >
CDirection< T >::~CDirection(){};


template < typename T >
T CDirection< T >::GetValue() const
{
    return mActualCDirection;
}

template < typename T >
T CDirection< T >::GetLastValue() const
{
    return mLastCDirection;
}

template < typename T >
void CDirection< T >::SetValue( const double value )
{
    mLastCDirection = mActualCDirection;
    mActualCDirection = value;
}

}    // namespace state

#endif