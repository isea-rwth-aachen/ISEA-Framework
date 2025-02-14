/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : valueStateWrapper.h
* Creation Date : 19-02-2014
* Last Modified : Mo 08 Sep 2014 15:27:43 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _VALUESTATEWRAPPER_
#define _VALUESTATEWRAPPER_

#include <limits>
#include "../misc/matrixInclude.h"
#include "dgl_state.h"

#include "../misc/macros.h"


namespace state
{
// typedef double ValueT;
/// This states wrapps an arbitrary pointer within and evalutes it

template < typename ValueT >
class ValueStateWrapper : public State
{
    public:
    ValueStateWrapper( ValueT *value );
    virtual ~ValueStateWrapper(){};

    virtual double GetMaxDt() const
    {
        return ::std::numeric_limits< double >::max();
    };    // returns the Maximum dt step that can be done before any major change in the state/lookup occures

    virtual double GetValue() const { return *static_cast< double * >( mValue ); };
    virtual double GetLastValue() const { return *static_cast< double * >( mLastValue ); };

    virtual double operator()() const { return *static_cast< double * >( mValue ); };

    private:
    ValueStateWrapper();
    ValueT *mValue;
    ValueT *mLastValue;
    protected:
};

template < typename ValueT >
ValueStateWrapper< ValueT >::ValueStateWrapper( ValueT *value )
    : State()
    , mValue( value )
    , mLastValue( value )
{
}

#ifdef _SYMBOLIC_
template <>
double ValueStateWrapper< ScalarUnit >::GetValue() const;

template <>
double ValueStateWrapper< ScalarUnit >::operator()() const;

#endif /* _SYMBOLIC_ */

} /*state*/
#endif /* _VALUESTATEWRAPPER_ */
