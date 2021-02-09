/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : createObserver.h
* Creation Date : 11-08-2015
* Last Modified : Do 15 Jun 2017 16:53:40 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CREATEOBSERVER_
#define _CREATEOBSERVER_

// ETC
#include "../../misc/matrixInclude.h"
#include "../../observer/aging_observer.h"
#include "../../observer/filter/vcpfilter.h"
#include "../../observer/thermal_observer.h"
#include "../../observer/twoPortObserver.h"
#include "observerclasswrapper.h"
#include "observerfactorybuilder.h"

template < class containerType, typename matrixType, bool FilterTypeChoice >
boost::shared_ptr< observer::TwoPortObserver< matrixType > >
CreateTwoPortObserver( const containerType* observablePorts, const xmlparser::XmlParameter* rootparam,
                       ScalarUnit* voltageOutputVec = 0, ScalarUnit* currentOutputVec = 0,
                       ScalarUnit* powerOutputVec = 0, ScalarUnit* socOutputVec = 0, ScalarUnit* socSurfaceOutputVec = 0,
                       const boost::shared_ptr< electrical::TwoPort< matrixType > >& twoPortRoot = 0,
                       const std::string& uuid = "" )
{

    boost::shared_ptr< observer::TwoPortObserver< matrixType > > obs;
    if ( observablePorts )
    {
        obs = boost::make_shared< observer::TwoPortObserver< matrixType > >( *observablePorts, twoPortRoot, uuid );
    }
    else
    {
        obs = boost::make_shared< observer::TwoPortObserver< matrixType > >( twoPortRoot, uuid );
    }

    if ( FilterTypeChoice )
    {
        if ( rootparam )
        {
            boost::scoped_ptr< factory::Factory< observer::Filter< matrixType, electrical::TwoPort, observer::PreparationType< matrixType > >, factory::ArgumentTypeObserver > > fct(
             factory::BuildObserverFactoryTwoPort< matrixType, FilterTypeChoice >() );

            if ( rootparam->HasElementDirectChild( "Observer" ) &&
                 rootparam->GetElementChild( "Observer" )->HasElementDirectChild( "Electrical" ) )
            {
                std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
                 rootparam->GetElementChild( "Observer" )->GetElementChildren( "Electrical" );
                for ( size_t i = 0; i < children.size(); ++i )
                {
                    obs->AddFilter( fct->CreateInstance( children.at( i ) ) );
                }
            }
        }
    }
    else if ( voltageOutputVec && currentOutputVec && powerOutputVec && socOutputVec && socSurfaceOutputVec )
    {
#ifndef _SYMBOLIC_
        obs->AddFilter( new observer::VCPFilter< matrixType >( voltageOutputVec, currentOutputVec, powerOutputVec,
                                                               socOutputVec, socSurfaceOutputVec ) );
#endif /* _SYMBOLIC_ */
    }
    return obs;
}

template < typename matrixType, bool FilterTypeChoice >
boost::shared_ptr< observer::ThermalObserver< matrixType > >
CreateThermalObserver( const xmlparser::XmlParameter* rootparam,
                       std::vector< thermal::ThermalElement< matrixType >* > thermalElementsOfAreas,
                       const boost::shared_ptr< std::vector< std::vector< size_t > > >& areas,
                       const boost::shared_ptr< std::vector< std::vector< size_t > > >& volumes,
                       const boost::shared_ptr< std::vector< misc::StrCont > >& volumeNames,
                       const boost::shared_ptr< std::vector< geometry::Cartesian< matrixType > > >& vertices,
                       const boost::shared_ptr< std::vector< ::probe::ThermalProbe > >& probes,
                       const boost::shared_ptr< std::vector< std::vector< double > > >& conductivtymatrix,
                       double surfaceArea, double heatCapacity, const std::string& uuid = "" )
{

    boost::shared_ptr< observer::ThermalObserver< matrixType > > obs(
     new observer::ThermalObserver< matrixType >( thermalElementsOfAreas, areas, volumes, volumeNames, vertices, probes,
                                                  conductivtymatrix, surfaceArea, heatCapacity, uuid ) );

    boost::scoped_ptr< factory::Factory< observer::Filter< matrixType, thermal::ThermalElement, observer::ThermalPreperation >, factory::ArgumentTypeObserver > > fct(
     factory::BuildObserverFactoryThermal< matrixType, FilterTypeChoice >() );

    if ( FilterTypeChoice )
    {
        if ( rootparam->HasElementDirectChild( "Observer" ) &&
             rootparam->GetElementChild( "Observer" )->HasElementDirectChild( "Thermal" ) )
        {

            std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
             rootparam->GetElementChild( "Observer" )->GetElementChildren( "Thermal" );
            for ( size_t i = 0; i < children.size(); ++i )
            {
                obs->AddFilter( fct->CreateInstance( children.at( i ) ) );
            }
        }
    }
    return obs;
}

/// recreate the observer filters from the observer factory, used to change output filenames after aging steps
template < typename matrixType >
void CreateNewFilters( observer::TwoPortObserver< matrixType >& observer, const xmlparser::XmlParameter& rootXmlNode )
{
    observer.DeleteFilters();

    boost::scoped_ptr< factory::Factory< observer::Filter< matrixType, electrical::TwoPort, observer::PreparationType< matrixType > >, factory::ArgumentTypeObserver > > fct(
     factory::BuildObserverFactoryTwoPort< matrixType, true >() );

    if ( rootXmlNode.HasElementDirectChild( "Observer" ) &&
         rootXmlNode.GetElementChild( "Observer" )->HasElementDirectChild( "Electrical" ) )
    {
        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
         rootXmlNode.GetElementChild( "Observer" )->GetElementChildren( "Electrical" );
        for ( size_t i = 0; i < children.size(); ++i )
        {
            observer.AddFilter( fct->CreateInstance( children.at( i ) ) );
        }
    }
}

/// recreate the observer filters from the observer factory, used to change output filenames after aging steps
template < typename matrixType >
void CreateNewFilters( observer::ThermalObserver< matrixType >& observer, const xmlparser::XmlParameter& rootXmlNode )
{
    observer.DeleteFilters();

    boost::scoped_ptr< factory::Factory< observer::Filter< matrixType, thermal::ThermalElement, observer::ThermalPreperation >, factory::ArgumentTypeObserver > > fct(
     factory::BuildObserverFactoryThermal< matrixType, true >() );

    if ( rootXmlNode.HasElementDirectChild( "Observer" ) && rootXmlNode.GetElementChild( "Observer" )->HasElementDirectChild( "Thermal" ) )
    {
        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
         rootXmlNode.GetElementChild( "Observer" )->GetElementChildren( "Thermal" );
        for ( size_t i = 0; i < children.size(); ++i )
        {
            observer.AddFilter( fct->CreateInstance( children.at( i ) ) );
        }
    }
}

#ifdef BUILD_AGING_SIMULATION

template < class containerType, typename matrixType, bool FilterTypeChoice >
boost::shared_ptr< observer::AgingObserver< matrixType > >
CreateAgingObserver( containerType& observablePorts, const xmlparser::XmlParameter* rootparam,
                     const std::string& uuid = "" )
{
    boost::shared_ptr< observer::AgingObserver< matrixType > > obs( new observer::AgingObserver< matrixType >( observablePorts, uuid ) );

    if ( FilterTypeChoice )
    {
        if ( rootparam )
        {
            boost::scoped_ptr< factory::Factory< observer::Filter< matrixType, aging::AgingTwoPort, observer::PreparationType< matrixType > >, factory::ArgumentTypeObserver > > fct(
             factory::BuildObserverFactoryAging< matrixType, FilterTypeChoice >() );

            if ( rootparam->HasElementDirectChild( "Observer" ) &&
                 rootparam->GetElementChild( "Observer" )->HasElementDirectChild( "Aging" ) )
            {
                std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children =
                 rootparam->GetElementChild( "Observer" )->GetElementChildren( "Aging" );
                for ( size_t i = 0; i < children.size(); ++i )
                {
                    obs->AddFilter( fct->CreateInstance( children.at( i ) ) );
                }
            }
        }
    }

    return obs;
}

#endif    // BUILD_AGING_SIMULATION

/// inserts all observable twoports inside root into the vector twoports if they are not already found in the vector
/// for building the twoport observer, the vector should already contain all cellements before calling this function so that they are always printed first
template < typename matrixType >
void FindObservableTwoports( std::vector< boost::shared_ptr< electrical::TwoPort< matrixType > > >& twoports,
                             const boost::shared_ptr< electrical::TwoPort< matrixType > >& root )
{
    if ( root->IsObservable() )
    {
        if ( std::find( twoports.begin(), twoports.end(), root ) == twoports.end() )
            twoports.push_back( root );
    }

    if ( root->HasChildren() )
    {
        const electrical::TwoPortWithChild< matrixType >& tpWithChild =
         static_cast< const electrical::TwoPortWithChild< matrixType >& >( *root );
        for ( const auto& child : tpWithChild )
        {
            FindObservableTwoports( twoports, child );
        }
    }
}

#endif /* _CREATEOBSERVER_ */
