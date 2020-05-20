#include "simulation_data_outline.h"


namespace simulation
{

template void ThermalModelOutline< double >( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode,
                                             size_t *numberOfCells, size_t *numberOfThermalElements,
                                             vector< geometry::Cartesian< double > > *emplacementPointsOfThermalBlocks,
                                             size_t *numberOfThermalProbes );

#ifndef _SYMBOLIC_
template void QuickVisualizeThermalModel< double >( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode );
#endif

}    // namespace simulation
