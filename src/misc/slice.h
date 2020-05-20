/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : slice.h
* Creation Date : 08-04-2015
* Last Modified : Do 25 Jun 2015 11:19:58 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SLICE_
#define _SLICE_

// STD
#include <vector>

/// SliceView gives the ability to operate on a vector<T*> elements like on
template < typename T >
class SliceView
{
    public:
    SliceView( std::vector< T * > &slice );

    /// Change the element persistent
    T &operator[]( size_t x );

    /// Give back an element that does not change the slicedview
    T operator[]( size_t x ) const;

    // SliceView<T>& operator=(const &SliceView<T>);
    private:
    std::vector< T * > &mSlice;
};

template < typename T >
SliceView< T >::SliceView( std::vector< T * > &slice )
    : mSlice( slice ){};

template < typename T >
T &SliceView< T >::operator[]( size_t x )
{
    if ( x < mSlice.size() )
        return *mSlice[x];
    return *mSlice.at( x );
}

template < typename T >
T SliceView< T >::operator[]( size_t x ) const
{
    if ( x < mSlice.size() )
        return *mSlice[x];
    return *mSlice.at( x );
};
#endif /* _SLICE_ */
