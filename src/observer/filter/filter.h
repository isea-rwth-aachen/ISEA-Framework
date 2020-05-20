/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : filter.h
* Creation Date : 18-11-2012
* Last Modified : Di 13 Jun 2017 19:02:35 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _FILTER_
#define _FILTER_

// STD
#include <vector>

// BOOST
#include <boost/shared_ptr.hpp>

// ETC
#include "../../geometry/cartesian.h"
#include "../../misc/StrCont.h"
#include "../../misc/macros.h"
#include "../../misc/matrixInclude.h"
#include "../../probes/thermal_probe.h"

namespace electrical
{
template < class T >
class TwoPort;
}

namespace observer
{
template < typename T = myMatrixType >
struct PreparationType
{
    public:
    PreparationType( const size_t numberOfElements, electrical::TwoPort< T > *rootPort = 0 )
        : mNumberOfElements( numberOfElements )
        , mRootPort( rootPort ){};

    const size_t mNumberOfElements;
    electrical::TwoPort< T > *mRootPort;
};

struct ThermalPreperation
{
    ThermalPreperation( const boost::shared_ptr< std::vector< std::vector< size_t > > > &areas,
                        const boost::shared_ptr< std::vector< std::vector< size_t > > > &volumes,
                        const boost::shared_ptr< std::vector< misc::StrCont > > &volumeNames,
                        const boost::shared_ptr< std::vector< geometry::Cartesian< double > > > &vertices,
                        const boost::shared_ptr< std::vector< ::probe::ThermalProbe > > &probes,
                        const boost::shared_ptr< std::vector< std::vector< double > > > &conductivtymatrix,
                        double surfaceArea = 0, double heatCapacity = 0 )
        : mAreas( areas )
        , mVolumes( volumes )
        , mVolumeNames( volumeNames )
        , mVertices( vertices )
        , mProbes( probes )
        , mConductivityMatrix( conductivtymatrix )
        , mSurfaceArea( surfaceArea )
        , mTotalHeatCapacity( heatCapacity ){};

    public:
    boost::shared_ptr< std::vector< std::vector< size_t > > > mAreas;
    boost::shared_ptr< std::vector< std::vector< size_t > > > mVolumes;
    boost::shared_ptr< std::vector< misc::StrCont > > mVolumeNames;
    boost::shared_ptr< std::vector< geometry::Cartesian< double > > > mVertices;
    boost::shared_ptr< std::vector< ::probe::ThermalProbe > > mProbes;
    boost::shared_ptr< std::vector< std::vector< double > > > mConductivityMatrix;
    double mSurfaceArea = 0;
    double mTotalHeatCapacity = 0;
};


/// Base Class for filtering the results of the observer

template < typename T, template < typename > class TConcrete, typename ArgumentType >
class Filter
{
    public:
    typedef typename std::vector< TConcrete< T > * > Data_t;

    Filter();
    virtual ~Filter(){};
    virtual void ProcessData( const typename Filter< T, TConcrete, ArgumentType >::Data_t &data, const double t );
    virtual void PrepareFilter( ArgumentType &prePareData ) { UNUSED( prePareData ); };

    void SetNext( Filter< T, TConcrete, ArgumentType > *newNext );

    private:
    protected:
    Filter< T, TConcrete, ArgumentType > *mNext;
};

template < typename T, template < typename > class TConcrete, typename ArgumentType >
Filter< T, TConcrete, ArgumentType >::Filter()
    : mNext( 0 )
{
}


template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Filter< T, TConcrete, ArgumentType >::ProcessData( const typename Filter< T, TConcrete, ArgumentType >::Data_t &data, double t )
{
    if ( mNext )
        mNext->ProcessData( data, t );
}

template < typename T, template < typename > class TConcrete, typename ArgumentType >
void Filter< T, TConcrete, ArgumentType >::SetNext( Filter< T, TConcrete, ArgumentType > *newNext )
{
    mNext = newNext;
}

}    // namespace observer
#endif /* _FILTER_ */
