#ifndef _THERMAL_VISUALIZER_
#define _THERMAL_VISUALIZER_

#include "../thermal/blocks/elements/thermal_element.h"
#include <boost/foreach.hpp>
#include <vector>


#include "../misc/macros.h"
#include "observer.h"


namespace observer
{
using std::vector;
using thermal::Cartesian;
using thermal::ThermalElement;


/// ThermalObserver creates output data as CSV files that can be processed to visualize the simulation
template < typename T = double >
class ThermalObserver : public Observer< T, ThermalElement, ThermalPreperation >
{
    public:
    /**
     * param[in] thermalElementsOfAreas Thermal elements belonging to areas, ordered by the same sequence, to later
     * extract temperature for areas
     * param[in] areas All areas to be displayed, depicted as a set of vertices
     * param[in] vertices All vertices from which areas can be constructed
     * param[in] fileNameTemperatures,fileNameVertices,fileNameAreas Data is written into these csv-files
     */
    ThermalObserver( vector< ThermalElement< T > * > thermalElementsOfAreas,
                     const boost::shared_ptr< vector< vector< size_t > > > &areas,
                     const boost::shared_ptr< vector< vector< size_t > > > &volumes,
                     const boost::shared_ptr< vector< misc::StrCont > > &volumeNames,
                     const boost::shared_ptr< vector< Cartesian< T > > > &vertices,
                     const boost::shared_ptr< std::vector< ::probe::ThermalProbe > > &probes,
                     const boost::shared_ptr< std::vector< std::vector< double > > > &conductivtymatrix,
                     double surfaceArea = 0, double heatCapacity = 0, const std::string &uuid = "" );

    virtual void PrepareFilter( Filter< T, ThermalElement, ThermalPreperation > *filt );

    virtual void operator()( double t );
    virtual ~ThermalObserver();

    private:
    ThermalPreperation mPrepareParameter;
    std::vector< ThermalElement< T > * > mThermalElementsOfAreas;
};

template < typename T >
ThermalObserver< T >::ThermalObserver( vector< ThermalElement< T > * > thermalElementsOfAreas,
                                       const boost::shared_ptr< vector< vector< size_t > > > &areas,
                                       const boost::shared_ptr< vector< vector< size_t > > > &volumes,
                                       const boost::shared_ptr< vector< misc::StrCont > > &volumeNames,
                                       const boost::shared_ptr< vector< Cartesian< T > > > &vertices,
                                       const boost::shared_ptr< std::vector< ::probe::ThermalProbe > > &probes,
                                       const boost::shared_ptr< std::vector< std::vector< double > > > &conductivtymatrix,
                                       double surfaceArea, double heatCapacity, const std::string &uuid )
    : Observer< T, ThermalElement, ThermalPreperation >( uuid )
    , mPrepareParameter( uuid, areas, volumes, volumeNames, vertices, probes, conductivtymatrix, surfaceArea, heatCapacity )
    , mThermalElementsOfAreas( thermalElementsOfAreas )
{
    BOOST_FOREACH ( const vector< size_t > &area, *areas )
    {
        if ( area.size() == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyArea" );
    }

    if ( volumes->size() != volumeNames->size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "volumeNamesVecNonEqualVolumesVec" );

    BOOST_FOREACH ( const vector< size_t > &volume, *volumes )
    {
        if ( volume.size() == 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyVolume" );
    }
}

template < typename T >
void ThermalObserver< T >::PrepareFilter( Filter< T, ThermalElement, ThermalPreperation > *filt )
{
    filt->PrepareFilter( mPrepareParameter );
}


template < typename T >
void ThermalObserver< T >::operator()( double t )
{
    if ( Observer< T, ThermalElement, ThermalPreperation >::mBegin )
    {
        Observer< T, ThermalElement, ThermalPreperation >::mBegin->ProcessData( mThermalElementsOfAreas, t );
    }
}

template < typename T >
ThermalObserver< T >::~ThermalObserver()
{
}
}    // namespace observer
#endif
