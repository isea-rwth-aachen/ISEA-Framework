/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : baseExport.h
* Creation Date : 14-01-2014
* Last Modified : Fr 06 Mai 2016 12:18:22 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _BASEEXPORT_
#define _BASEEXPORT_

#include <cstdlib>
#include <stdexcept>

#include "../electrical/twoport.h"

/// Baseclass for all exports
/// Exports take framework data and create another kind of data
/// e.g. create a spice model or a standalone simulink modell

template < typename matType >
class BaseExport
{
    public:
    BaseExport( const electrical::TwoPort< matType > *rootTwoport );
    virtual void WritePreamble() = 0;
    virtual void LoopOverAllTwoPorts( const electrical::TwoPort< matType > *twoPort ) = 0;
    virtual void WriteEnding() = 0;
    virtual ~BaseExport(){};

    private:
    protected:
    BaseExport() { abort(); };
    const electrical::TwoPort< matType > *mRootPort;
};

template < typename matType >
BaseExport< matType >::BaseExport( const electrical::TwoPort< matType > *rootTwoport )
    : mRootPort( rootTwoport )
{
    if ( !this->mRootPort )
    {
#ifdef __EXCEPTIONS__
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "NoRoot" );
#else
        abort();
#endif
    }
}

#endif /* _BASEEXPORT_ */
