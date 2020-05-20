#ifndef _SIMULATION_DATA_OUTLINE_
#define _SIMULATION_DATA_OUTLINE_

#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../factory/factorybuilder.h"

#include "../observer/filter/matlabFilter.h"
#include "../observer/thermal_observer.h"
#include "../thermal/blocks/thermal_block.h"
#include "../thermal/thermal_model.h"


namespace simulation
{

/// ThermalModelOutline() returns number of cells, number of thermal elements and the emplacement points of all thermal
/// blocks if the respective pointer is not set to null
template < typename T >
void ThermalModelOutline( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, size_t *numberOfCells,
                          size_t *numberOfThermalElements,
                          std::vector< geometry::Cartesian< T > > *emplacementPointsOfThermalBlocks, size_t *numberOfThermalProbes )
{
    // Build Factories
    factory::FactoryBuilder< myMatrixType, T > factoryBuilder;
    thermal::ThermalFactory< T > *thermalFactory = factoryBuilder.BuildThermalFactory();

    // Temporary variables to process creation of thermal system
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > heatedBlocks;
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > unheatedBlocks;
    std::vector< boost::shared_ptr< thermal::CoolingBlock< T > > > coolingBlocks;
    std::vector< std::vector< boost::shared_ptr< ::state::ThermalState< T > > > > thermalStatesOfCellBlocks;
    std::vector< ::probe::ThermalProbe > thermalProbes;

    // Create data
    thermalFactory->CreateThermalModel( rootXmlNode, heatedBlocks, unheatedBlocks, coolingBlocks, 0,
                                        &thermalStatesOfCellBlocks, &thermalProbes );

    // Evaluate data
    if ( numberOfCells )
        *numberOfCells = thermalStatesOfCellBlocks.size();
    if ( numberOfThermalElements )
    {
        size_t sum = 0;
        BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalBlock< T > > &block, heatedBlocks )
            sum += block->GetNumberOfThermalElements();
        BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalBlock< T > > &block, unheatedBlocks )
            sum += block->GetNumberOfThermalElements();
        *numberOfThermalElements = sum;
    }
    if ( emplacementPointsOfThermalBlocks )
    {
        emplacementPointsOfThermalBlocks->clear();
        emplacementPointsOfThermalBlocks->reserve( heatedBlocks.size() );
        BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalBlock< T > > &block, heatedBlocks )
            emplacementPointsOfThermalBlocks->push_back( block->GetEmplacementPoint() );
    }

    if ( numberOfThermalProbes )
    {
        *numberOfThermalProbes = thermalProbes.size();
    }
}


#ifndef _SYMBOLIC_
/// Visualizes thermal model at t=0.0 and prints to console if there are any collision in it
template < typename T >
void QuickVisualizeThermalModel( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode )
{
    bool showLateralSurfaces = false;
    // Evaluate Options node
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = rootXmlNode->GetElementChild( "Options" );
    if ( optionsNode->HasElementDirectChild( "ThermalVisualizer" ) &&
         optionsNode->GetElementChild( "ThermalVisualizer" )->HasElementDirectChild( "ShowLateralSurfaces" ) )
        showLateralSurfaces =
         optionsNode->GetElementChild( "ThermalVisualizer" )->GetElementBoolValue( "ShowLateralSurfaces", false );

    // Build Factories
    factory::FactoryBuilder< myMatrixType, T > factoryBuilder;
    thermal::ThermalFactory< T > *thermalFactory = factoryBuilder.BuildThermalFactory();

    // Temporary variables to process creation of thermal system
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > heatedBlocks;
    std::vector< boost::shared_ptr< thermal::ThermalBlock< T > > > unheatedBlocks;
    std::vector< boost::shared_ptr< thermal::CoolingBlock< T > > > coolingBlocks;

    std::vector< boost::shared_ptr< thermal::ThermalElement< T > > > thermalElements;
    std::vector< std::vector< thermal::IndexedValue< T > > > conductivityMatrix;
    std::vector< thermal::IndexedArea< T > > surfaceElements;
    std::vector< thermal::IndexedInnerArea< T > > innerSurfaceElements;
    boost::shared_ptr< thermal::BlockGeometry< T > > blockGeometry;

    misc::StrCont description;

    // Create thermal model
    thermalFactory->CreateThermalModel( rootXmlNode, heatedBlocks, unheatedBlocks, coolingBlocks, 0, 0 );
    thermal::Tolerance<> tolerance( 0.000001, geometry::Angle<>::Deg( 0.001 ), 0.0003 );
    thermal::ThermalModel< T > thermalModel( tolerance, thermal::ThermalModel< T >::AGGREGATE_BY_PLANE );
    thermalModel.ClearAndSetNumberOfBlocksAndCoolings( heatedBlocks.size() + unheatedBlocks.size(), 0 );
    BOOST_FOREACH ( boost::shared_ptr< thermal::ThermalBlock< T > > &block, heatedBlocks )
    {
        block->CreateData( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );
        block->GetDescription( description );
        if ( showLateralSurfaces )
            block->GetInnerSurfaceAreas( innerSurfaceElements );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceElements, blockGeometry, innerSurfaceElements, description );
    }
    BOOST_FOREACH ( boost::shared_ptr< thermal::ThermalBlock< T > > &block, unheatedBlocks )
    {
        block->CreateData( thermalElements, conductivityMatrix, surfaceElements, blockGeometry );
        block->GetDescription( description );
        if ( showLateralSurfaces )
            block->GetInnerSurfaceAreas( innerSurfaceElements );
        thermalModel.AddBlock( thermalElements, conductivityMatrix, surfaceElements, blockGeometry, innerSurfaceElements, description );
    }

    // Visualize
    vector< thermal::ThermalElement< double > * > thermalElementsOfAreas;
    auto areas = boost::make_shared< vector< vector< size_t > > >();
    auto volumes = boost::make_shared< vector< vector< size_t > > >();
    auto volumeNames = boost::make_shared< vector< misc::StrCont > >();
    auto vertices = boost::make_shared< vector< geometry::Cartesian< double > > >();
    auto conductivity = boost::make_shared< vector< vector< double > > >();
    auto thermalProbes = boost::make_shared< std::vector< ::probe::ThermalProbe > >();
    thermalModel.CreateDataForVisualization( thermalElementsOfAreas, *areas, *volumes, *volumeNames, *vertices, showLateralSurfaces );
    observer::ThermalObserver< T > thermalVisualizer( thermalElementsOfAreas, areas, volumes, volumeNames, vertices,
                                                      thermalProbes, conductivity );
#ifndef _DS1006
    thermalVisualizer.AddFilter(
     boost::make_shared< observer::MatlabFilterThermal< double > >( "Patch_Temperatures.mat" ) );
#endif

#ifdef __EXCEPTIONS__
    try
    {
        thermalModel.BlocksDimensionTest();
        thermalModel.BlocksCollisionTest();
    }
    catch ( const std::runtime_error &e )
    {
        printf( "%s", e.what() );
    }
#endif
    thermalVisualizer( 0 );
}
#endif
}    // namespace simulation
#endif    // Include guard
