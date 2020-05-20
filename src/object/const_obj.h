/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : const_obj.h
* Creation Date : 31-10-2012
* Last Modified : Mo 24 Nov 2014 18:23:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CONST_OBJ_
#define _CONST_OBJ_
#include "object.h"

namespace object
{
/// Simple value class, which always return the internal value it was initialized with
template < typename T = double >
class ConstObj : public Object< T >
{
    public:
    ConstObj( T value );
    ConstObj( T value, ConstObj< T >* CloneObj );

    ConstObj( const char* name, T value, ConstObj< T >* CloneObj );

    virtual ~ConstObj(){};

    virtual T GetValue() const;
    virtual T operator()() const;

    inline virtual T GetMaxValueOfLookup() { return this->mLastValue; };
    inline virtual T GetMinValueOfLookup() { return this->mLastValue; };

    virtual const char* GetName() const;

    virtual void SetFactor( const T factor );

    protected:
    const T mValue;
};

template < typename T >
ConstObj< T >::ConstObj( T value )
    : Object< T >()
    , mValue( value )
{
    this->mLastValue = value * this->mFactor;
}

template < typename T >
ConstObj< T >::ConstObj( T value, ConstObj< T >* CloneObj )
    : Object< T >()
    , mValue( value * CloneObj->mValue )
{
    this->mLastValue = value * CloneObj->mLastValue;
    this->mFactor = CloneObj->mFactor;
}

template < typename T >
T ConstObj< T >::GetValue() const
{
    return this->mLastValue;
}

template < typename T >
T ConstObj< T >::operator()() const
{
    return this->mLastValue;
}

template < typename T >
const char* ConstObj< T >::GetName() const
{
    return "ConstObject";
}

template < typename T >
void ConstObj< T >::SetFactor( const T factor )
{
    Object< T >::SetFactor( factor );
    this->mLastValue = this->mValue * factor;
}


} /* END NAMESPACE */
#endif /* _CONST_OBJ_ */
