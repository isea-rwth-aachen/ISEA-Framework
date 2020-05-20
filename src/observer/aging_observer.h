// Filename: aging_observer.h
// Autor: Christopher Abele

#ifndef _AGINGOBSERVER_
#define _AGINGOBSERVER_

#ifdef BUILD_AGING_SIMULATION

// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

#include "observer.h"

#include "../aging/aging_twoport.h"
#include "../misc/matrixInclude.h"


/// The observer namespace for handling the output
namespace observer
{

/// The AgingObserver observes the aging data of the underlying cells
template < typename T = myMatrixType >
class AgingObserver : public Observer< T, aging::AgingTwoPort, PreparationType< T > >
{

    public:
    typedef Filter< T, aging::AgingTwoPort, PreparationType< T > > FilterT;

    /// Construtctor
    AgingObserver( std::vector< boost::shared_ptr< aging::AgingTwoPort< T > > >& observableTwoPorts );

    /// Desctructor
    ~AgingObserver(){};

    /// Observe the underlying cells
    virtual void operator()( double t );

    /// Prepare the assigned filter
    virtual void PrepareFilter( Filter< T, aging::AgingTwoPort, PreparationType< T > >* filt );

    /// Resets the observer, should be called if the content of "mObservableTwoPort" has been changed
    virtual void ResetObserver();

    private:
    const std::vector< boost::shared_ptr< aging::AgingTwoPort< T > > >& mObservableTwoPorts;
    typedef typename std::vector< aging::AgingTwoPort< T >* > ProcessDataT;

    ProcessDataT mObservedTwoPortsPtr;
};

template < typename T >
AgingObserver< T >::AgingObserver( std::vector< boost::shared_ptr< aging::AgingTwoPort< T > > >& observableTwoPorts )
    : Observer< T, aging::AgingTwoPort, PreparationType< T > >()
    , mObservableTwoPorts( observableTwoPorts )
{
    ResetObserver();
}

template < typename T >
void AgingObserver< T >::PrepareFilter( Filter< T, aging::AgingTwoPort, PreparationType< T > >* filt )
{
    PreparationType< T > prepType( mObservableTwoPorts.size() );
    filt->PrepareFilter( prepType );
}

template < typename T >
void AgingObserver< T >::operator()( double t )
{
    if ( this->mBegin )
    {
        this->mBegin->ProcessData( mObservedTwoPortsPtr, t );
    }
}

template < typename T >
void AgingObserver< T >::ResetObserver()
{
    // Resets the vector "mObservedTwoPortsPtr"
    mObservedTwoPortsPtr.clear();
    mObservedTwoPortsPtr.resize( mObservableTwoPorts.size() );

    // Reassigns the observed cells to the vector "mObservedTwoPortsPtr"
    for ( size_t i = 0; i < mObservableTwoPorts.size(); ++i )
    {
        mObservedTwoPortsPtr[i] = mObservableTwoPorts[i].get();
    }
}

}    // End namespace observer

#endif    // _AGINGOBSERVER_

#endif    // BUILD_AGING_SIMULATION
