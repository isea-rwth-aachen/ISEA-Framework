/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : twoPortObserver.h
* Creation Date : 31-08-2015
* Last Modified : Mi 02 Sep 2015 18:46:33 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TWOPORTOBSERVER_
#define _TWOPORTOBSERVER_
// STD
#include <vector>

// BOOST
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include "observer.h"

#include "../electrical/twoport_withchild.h"
#include "../misc/matrixInclude.h"


/// The observer namespace for handling the output
namespace observer
{
template < typename T = myMatrixType >
class TwoPortObserver : public Observer< T, electrical::TwoPort, PreparationType< T > >
{

    public:
    typedef Filter< T, electrical::TwoPort, PreparationType< T > > FilterT;

    TwoPortObserver( const boost::shared_ptr< electrical::TwoPort< T > >& rootPort, const std::string& uuid );
    TwoPortObserver( const std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& observableTwoPorts,
                     const boost::shared_ptr< electrical::TwoPort< T > >& twoPortRoot = nullptr,
                     const std::string& uuid = "" );

    ~TwoPortObserver(){};
    virtual void operator()( double t );

    virtual void PrepareFilter( Filter< T, electrical::TwoPort, PreparationType< T > >* filt );

    typename FilterT::Data_t& GetObservedTwoPortsPtr() { return mObservedTwoPortsPtr; };

    const std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& GetObservedTwoPorts()
    {
        return mObservableTwoPorts;
    };
    void SetRoot( const boost::shared_ptr< electrical::TwoPort< T > >& root ) { mRootPort = root; };

    void ResetObserver( const boost::shared_ptr< electrical::TwoPort< T > >& root );
    void ResetObserver( const std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& observableTwoPorts,
                        const boost::shared_ptr< electrical::TwoPort< T > >& root = 0 );

    private:
    boost::shared_ptr< electrical::TwoPort< T > > mRootPort;
    std::vector< boost::shared_ptr< electrical::TwoPort< T > > > mObservableTwoPorts;
    typedef typename std::vector< electrical::TwoPort< T >* > ProcessDataT;

    void RegisterObservableTwoPort( const boost::shared_ptr< electrical::TwoPort< T > >& tp );
    void ResetFilter();

    ProcessDataT mObservedTwoPortsPtr;
};

template < typename T >
TwoPortObserver< T >::TwoPortObserver( const boost::shared_ptr< electrical::TwoPort< T > >& rootPort, const std::string& uuid )
    : Observer< T, electrical::TwoPort, PreparationType< T > >( uuid )
    , mRootPort( rootPort )
{
    ResetObserver( rootPort );
}

template < typename T >
TwoPortObserver< T >::TwoPortObserver( const std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& observableTwoPorts,
                                       const boost::shared_ptr< electrical::TwoPort< T > >& twoPortRoot, const std::string& uuid )
    : Observer< T, electrical::TwoPort, PreparationType< T > >( uuid )
    , mRootPort( twoPortRoot )
    , mObservableTwoPorts( observableTwoPorts )
{
    ResetObserver( observableTwoPorts, twoPortRoot );
}

template < typename T >
void TwoPortObserver< T >::PrepareFilter( Filter< T, electrical::TwoPort, PreparationType< T > >* filt )
{
    PreparationType< T > prepType( mObservableTwoPorts.size(), mRootPort.get(), this->mUUID );
    filt->PrepareFilter( prepType );
}

template < typename T >
void TwoPortObserver< T >::operator()( double t )
{
    if ( this->mBegin )
    {
        this->mBegin->ProcessData( mObservedTwoPortsPtr, t );
    }
}

template < typename T >
void TwoPortObserver< T >::RegisterObservableTwoPort( const boost::shared_ptr< electrical::TwoPort< T > >& twoPort )
{
    if ( !twoPort )
        return;

    if ( twoPort->IsObservable() )
        mObservableTwoPorts.push_back( twoPort );

    if ( !twoPort->HasChildren() )
        return;

    electrical::TwoPortWithChild< T >* tpwc = static_cast< electrical::TwoPortWithChild< T >* >( twoPort.get() );

    for ( size_t i = 0; i < tpwc->size(); ++i )
        RegisterObservableTwoPort( tpwc->shared_at( i ) );
}

template < typename T >
void TwoPortObserver< T >::ResetObserver( const boost::shared_ptr< electrical::TwoPort< T > >& root )
{
    if ( !root )
        return;

    SetRoot( root );
    RegisterObservableTwoPort( root );

    ResetFilter();
}

template < typename T >
void TwoPortObserver< T >::ResetObserver( const std::vector< boost::shared_ptr< electrical::TwoPort< T > > >& observableTwoPorts,
                                          const boost::shared_ptr< electrical::TwoPort< T > >& root )
{
    if ( root )
        SetRoot( root );

    mObservableTwoPorts = observableTwoPorts;

    ResetFilter();
}

template < typename T >
void TwoPortObserver< T >::ResetFilter()
{
    mObservedTwoPortsPtr.clear();
    mObservedTwoPortsPtr.resize( mObservableTwoPorts.size() );

    for ( size_t i = 0; i < mObservableTwoPorts.size(); ++i )
    {
        mObservedTwoPortsPtr[i] = mObservableTwoPorts[i].get();
    }

    for ( const auto& filter : this->mFilterChain )
        PrepareFilter( filter.get() );
}

}    // namespace observer
#endif /* _TWOPORTOBSERVER_ */
