/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : object.h
* Creation Date : 31-10-2012
* Last Modified : Mi 14 Okt 2015 17:19:58 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OBJECT_
#define _OBJECT_

// ETC
#include "../cstring/strlcpy.h"
#include "../exceptions/error_proto.h"

namespace object
{
/// This class helps circuit elements to change their behavior, e.g. they can have constant values or depend on a lookup
template < typename T >
class Object
{
    public:
    Object();
    virtual ~Object(){};

    virtual T GetValue() const = 0;    ///< Return the current value, for higher level classes this interface will return the latest value
    virtual T GetValue( const T val );
    virtual T GetValue( const T val1, const T val2 );
    virtual T GetValue( const T val1, const T val2, const T val3 );
    virtual T GetValue( const T val1, const T val2, const T val3, const T val4 );
    virtual T operator()() const = 0;    ///< Return the current value, for higher level classes this interface will return the latest value
    virtual T operator()( const T val );
    virtual T operator()( const T val, const T val2 );
    virtual T operator()( const T val, const T val2, const T val3 );
    virtual T operator()( const T val, const T val2, const T val3, const T val4 );
    virtual void SetFactor( const T factor );    // Set a factor that all values are multiplied by

    inline virtual T GetMaxValueOfLookup()
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnsupportedOperation",
                                             "GetMaxValueOfLookup" );
        return T();
    }
    inline virtual T GetMinValueOfLookup()
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnsupportedOperation",
                                             "GetMinValueOfLookup" );
        return T();
    }

    /// functions for multimap simplification
    virtual const char* GetName() const;

    private:
    protected:
    T mLastValue;
    T mFactor;
};

template < typename T >
Object< T >::Object()
    : mLastValue( T() )
    , mFactor( T( 1 ) )
{
}

template < typename T >
T Object< T >::GetValue( const T /* val */ )
{
    return mLastValue;
}

template < typename T >
T Object< T >::GetValue( const T /* val1 */, const T /* val2 */ )
{
    return mLastValue;
}

template < typename T >
T Object< T >::GetValue( const T /* val1 */, const T /* val2 */, const T /* val3 */ )
{
    return mLastValue;
}

template < typename T >
T Object< T >::GetValue( const T /* val1 */, const T /* val2 */, const T /* val3 */, const T /* val4 */ )
{
    return mLastValue;
}


template < typename T >
T Object< T >::operator()( const T val )
{
    return GetValue( val );
}


template < typename T >
T Object< T >::operator()( const T val, const T val2 )
{
    return GetValue( val, val2 );
}

template < typename T >
T Object< T >::operator()( const T val, const T val2, const T val3 )
{
    return GetValue( val, val2, val3 );
}

template < typename T >
T Object< T >::operator()( const T val, const T val2, const T val3, const T val4 )
{
    return GetValue( val, val2, val3, val4 );
}

template < typename T >
const char* Object< T >::GetName() const
{
    return "Object";
}

template < typename T >
void Object< T >::SetFactor( const T factor )
{
    this->mFactor = factor;
}

}    // namespace object
#endif /* _OBJECT_ */
