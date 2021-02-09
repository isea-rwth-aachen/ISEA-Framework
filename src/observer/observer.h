/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : observer.h
* Creation Date : 18-11-2012
* Last Modified : Mi 02 Sep 2015 18:31:13 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OBSERVER_
#define _OBSERVER_

// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

#include "../misc/macros.h"
#include "filter/filter.h"

/// The observer namespace for handling the output
namespace observer
{

/// This is the Generic observer class and should be specialized for further use.
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class Observer
{
    public:
    Observer( const std::string& uuid );
    virtual ~Observer(){};
    virtual void operator()( double t );
    void AddFilter( Filter< T, TConcrete, ArgumentType >* filt );
    void AddFilter( boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > filt );
    void DeleteFilters();

    virtual void PrepareFilter( Filter< T, TConcrete, ArgumentType >* filt ) = 0;
    std::vector< boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > >& GetFilterChain() { return mFilterChain; };

    protected:
    std::vector< boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > > mFilterChain;

    Filter< T, TConcrete, ArgumentType >* mBegin;
    Filter< T, TConcrete, ArgumentType >* mEnd;
    const std::string mUUID;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
Observer< T, TConcrete, ArgumentType >::Observer( const std::string& uuid )
    : mBegin( 0 )
    , mEnd( 0 )
    , mUUID( uuid )
{
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Observer< T, TConcrete, ArgumentType >::AddFilter( boost::shared_ptr< Filter< T, TConcrete, ArgumentType > > filt )
{
    if ( !mBegin )
    {
        mBegin = filt.get();
        mEnd = filt.get();
    }
    else
    {
        mEnd->SetNext( filt.get() );
        mEnd = filt.get();
    }
    mFilterChain.push_back( filt );
    PrepareFilter( filt.get() );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Observer< T, TConcrete, ArgumentType >::AddFilter( Filter< T, TConcrete, ArgumentType >* filt )
{
    this->AddFilter( boost::shared_ptr< Filter< T, TConcrete, ArgumentType > >( filt ) );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Observer< T, TConcrete, ArgumentType >::DeleteFilters()
{
    mFilterChain.clear();
    mBegin = nullptr;
    mEnd = nullptr;
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Observer< T, TConcrete, ArgumentType >::operator()( double t )
{
    UNUSED( t );
}

}    // namespace observer
#endif /* _OBSERVER_ */
