/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : verification.cpp
* Creation Date : 14-11-2016
* Last Modified : Mo 14 Nov 2016 16:36:55 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "findSimulationType.h"
#include "../factory/factorybuilder_for_aging.h"

simulation::SimulationType simulation::FindSimulationType( boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode,
                                                           CreatedSimulations *simulations )
{
    const double socStopCriterion = 0.1;             // percent
    const double thermalStateStopCriterion = 5.0;    // degree C

    boost::shared_ptr< simulation::ElectricalSimulation< myMatrixType, double, false > > electricalSimulation;
    boost::shared_ptr< simulation::ThermalSimulation< myMatrixType, double, false > > thermalSimulation;
    std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
    boost::shared_ptr< observer::ThermalObserver< double > > thermalVisualizer;

    bool electricalSimulationFound = false;
    bool thermalSimulationFound = false;
    bool agingSimulationFound = false;
    const double stepTime = 0.1;

    try
    {
        electricalSimulation.reset(
         new simulation::ElectricalSimulation< myMatrixType, double, false >( rootXmlNode, stepTime, 0.0, socStopCriterion, &cells ) );
        electricalSimulationFound = true;
    }
    catch ( ... )
    {
    }

    try
    {
        std::vector< std::vector< boost::shared_ptr< ThermalState< double > > > > thermalStatesOfCellBlocks;
        if ( electricalSimulation )
        {
            thermalSimulation.reset(
             new simulation::ThermalSimulation< myMatrixType, double, false >( rootXmlNode, stepTime, 0, thermalStateStopCriterion,
                                                                               &thermalVisualizer, &electricalSimulation->mThermalStates,
                                                                               &thermalStatesOfCellBlocks ) );
        }
        else
        {
            thermalSimulation.reset( new simulation::ThermalSimulation< myMatrixType, double, false >(
             rootXmlNode, stepTime, 0, thermalStateStopCriterion, &thermalVisualizer, 0, &thermalStatesOfCellBlocks ) );
        }
        thermalSimulationFound = true;
    }
    catch ( ... )
    {
    }

#ifdef BUILD_AGING_SIMULATION
    boost::shared_ptr< simulation::AgingSimulation< myMatrixType, double, false > > agingSimulation;

    if ( electricalSimulationFound && thermalSimulationFound )
    {
        try
        {
            factory::FactoryBuilderForAging< myMatrixType, double > factoryBuilder;
            electricalSimulation.reset(
             new simulation::ElectricalSimulation< myMatrixType, double, false >( rootXmlNode, stepTime, 0.0, socStopCriterion,
                                                                                  &cells, &factoryBuilder ) );

            double agingStepTime = rootXmlNode->GetElementChild( "Options" )
                                    ->GetElementChild( "Aging" )
                                    ->GetElementDoubleValue( "AgingStepTime" );

            agingSimulation.reset(
             new simulation::AgingSimulation< myMatrixType, double, false >( rootXmlNode, electricalSimulation, thermalSimulation,
                                                                             cells, agingStepTime, &factoryBuilder ) );
            agingSimulationFound = true;
        }
        catch ( ... )
        {
        }
    }
#endif

    if ( simulations )    // copy created simulations to output struct for further use
    {
        simulations->electricalSimulation = electricalSimulation;
        simulations->thermalSimulation = thermalSimulation;
#ifdef BUILD_AGING_SIMULATION
        simulations->agingSimulation = agingSimulation;
#endif
    }

    if ( agingSimulationFound )
        return AGING;
    if ( electricalSimulationFound && thermalSimulationFound )
        return THERMAL_ELECTRICAL;
    if ( thermalSimulationFound )
        return THERMAL;
    if ( electricalSimulationFound )
        return ELECTRICAL;
    return NONE;
}
