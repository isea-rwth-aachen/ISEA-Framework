/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : crate.h
* Creation Date : 08-07-2022
* Last Modified : Fr 8 Jul 2022 13:50 CET
* Created By : Jonas Rinner
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _Crate_
#define _Crate_

// STD
#include <vector>
#include <iostream>

// ETC
#include "../exceptions/error_proto.h"

#include "dgl_state.h"

#include <limits>
#include <vector>


#include "../misc/macros.h"
#include "../misc/matrixInclude.h"

//class TestCrate;
//class TestXML;

/*namespace electrical
{
template < typename T >
class TwoPort;
}*/

namespace state
{
template < typename T = double >
class CRateState : public State
{
    public:
    CRateState();
    virtual ~CRateState();

    T GetValue() const;
    T GetLastValue() const;
    void SetValue( const double value );

    T mActualCrate;
    T mLastCrate;
};

template < typename T >
CRateState< T >::CRateState()
    : mActualCrate(0.0)
{};

template < typename T >
CRateState< T >::~CRateState()
{
};


template < typename T >
T CRateState< T >::GetValue() const
{
    return mActualCrate;
}

template < typename T >
T CRateState< T >::GetLastValue() const
{
    return mLastCrate;
}

template < typename T >
void CRateState< T >::SetValue( const double value )
{
    mLastCrate = mActualCrate;
    mActualCrate = value;
}

}    // namespace state

#endif