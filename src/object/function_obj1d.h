/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : function_obj1d.h
* Creation Date : 14-05-2013
* Last Modified : Mo 24 Nov 2014 18:51:02 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _FUNCTION_OBJ1D_
#define _FUNCTION_OBJ1D_
#include "object.h"

#include "object.h"

namespace object
{
/// This Class allowes one to use function that take one arguement to be used by an object class
/// The invokation can be done through the operator(T) or GetValue(T)
template < typename T, T ( *FuncToCall )( T ) >
class FunctionObj : public Object< T >
{
    public:
    FunctionObj();
    virtual ~FunctionObj() {}
    virtual T GetValue() const;
    virtual T operator()() const;
    virtual T GetValue( const T val );
    virtual T operator()( const T val );

    virtual const char *GetName() const;
}; /* FunctionObj */

template < typename T, T ( *FuncToCall )( T ) >
FunctionObj< T, FuncToCall >::FunctionObj()
    : Object< T >()
{
}

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::operator()() const
{
    return this->mLastValue;
}

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::GetValue() const
{
    return this->mLastValue;
}

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::operator()( const T val )
{
    return FunctionObj< T, FuncToCall >::GetValue( val );
};

template < typename T, T ( *FuncToCall )( T ) >
T FunctionObj< T, FuncToCall >::GetValue( const T val )
{
    this->mLastValue = FuncToCall( val ) * this->mFactor;
    return FunctionObj< T, FuncToCall >::GetValue();
}

template < typename T, T ( *FuncToCall )( T ) >
const char *FunctionObj< T, FuncToCall >::GetName() const
{
    return "FunctionObj";
}


} /* object */
#endif /* _FUNCTION_OBJ1D_ */
