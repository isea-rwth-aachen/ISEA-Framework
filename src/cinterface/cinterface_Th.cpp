/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.cpp
* Creation Date : 23-05-2013
* Last Modified : Di 06 Okt 2015 18:54:20 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "cinterface_Th.h"
#include "pointer_structure.h"

// BOOST
#include <boost/scoped_ptr.hpp>

// ETC
#include "../thermal/simulation_data_outline.h"
#include "../thermal/thermal_simulation.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"

#ifndef _SYMBOLIC_

typedef myMatrixType myUnit;
typedef double myScalar;

#ifdef __cplusplus
extern "C"
{
#endif
    void GetRealSizes_Th( const char *configStr, size_t *numberOfCells, size_t *stateSize )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();
        simulation::ThermalModelOutline< double >( rootXmlNode, numberOfCells, stateSize, 0, 0 );
    }

    void SetEmplacementPoints_Th( const char *configStr, real_T *emplacementPointsOutputVector )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();

        vector< geometry::Cartesian< double > > emplacementPointsOfThermalBlocks;
        simulation::ThermalModelOutline< double >( rootXmlNode, 0, 0, &emplacementPointsOfThermalBlocks, 0 );
        for ( size_t i = 0; i < emplacementPointsOfThermalBlocks.size(); ++i )
        {
            emplacementPointsOutputVector[i * 3] = emplacementPointsOfThermalBlocks[i].GetX();
            emplacementPointsOutputVector[i * 3 + 1] = emplacementPointsOfThermalBlocks[i].GetY();
            emplacementPointsOutputVector[i * 3 + 2] = emplacementPointsOfThermalBlocks[i].GetZ();
        }
    }

    void InitializeSystem_Th( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();


        PointerStructure_Th *pointerStructure = new PointerStructure_Th;
        *pointerStructureAddress = (size_t)pointerStructure;
        pointerStructure->mThermalSimulation.reset(
         new simulation::ThermalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0,
                                                                       &pointerStructure->mThermalVisualizer, 0, 0 ) );

        if ( rootXmlNode->GetElementChild( "Options" )->HasElementDirectChild( "ThermalVisualizer" ) )
        {
            pointerStructure->mVisualizerCounter = 1;
            boost::shared_ptr< xmlparser::XmlParameter > visualizerNode =
             rootXmlNode->GetElementChild( "Options" )->GetElementChild( "ThermalVisualizer" );
            pointerStructure->mVisualizerTimeDiffFrames =
             visualizerNode->GetElementDoubleValue( "TimeBetweenFramesInSec" );
            pointerStructure->mVisualizerMaxNumberOfFrames = visualizerNode->GetElementIntValue( "MaxNumberOfFrames" );
        }
        else
        {
            pointerStructure->mVisualizerCounter = 0;
            pointerStructure->mVisualizerTimeDiffFrames = 0.0;
            pointerStructure->mVisualizerMaxNumberOfFrames = 0;
        }


        if ( stateVector )
        {
            myUnit initstate( pointerStructure->mThermalSimulation->mTemperatures.size(), 1 );
            memcpy( (void *)stateVector, &pointerStructure->mThermalSimulation->mTemperatures[0],
                    initstate.rows() * sizeof( real_T ) );
        }
    }

    void ResetSystem_Th( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector, real_T *cellTemperatureOutputVector )
    {
        DeleteSystem_Th( pointerStructureAddress );
        InitializeSystem_Th( configStr, pointerStructureAddress, stateVector );
        CellTemperatureOutput_Th( pointerStructureAddress, cellTemperatureOutputVector );
    }

    void CellTemperatureOutput_Th( const size_t *pointerStructureAddress, real_T *cellTemperatureOutputVector, double )
    {
        PointerStructure_Th *pointerStructure = (PointerStructure_Th *)*pointerStructureAddress;
        for ( size_t i = 0; i < pointerStructure->mThermalSimulation->mThermalStates.size(); ++i )
            cellTemperatureOutputVector[i] =
             static_cast< real_T >( pointerStructure->mThermalSimulation->mThermalStates[i]->GetValue() );
    }

    void TriggerVisualizer_Th( const size_t *pointerStructureAddress, double time, bool forceTriggering )
    {
        PointerStructure_Th *pointerStructure = (PointerStructure_Th *)*pointerStructureAddress;
        if ( pointerStructure->mVisualizerTimeDiffFrames != 0.0 )
        {
            if ( ( forceTriggering && pointerStructure->mVisualizerCounter < pointerStructure->mVisualizerMaxNumberOfFrames ) ||
                 pointerStructure->mVisualizerTimeDiffFrames < 0.0 ||
                 ( time >= pointerStructure->mVisualizerTimeDiffFrames * pointerStructure->mVisualizerCounter &&
                   pointerStructure->mVisualizerCounter < pointerStructure->mVisualizerMaxNumberOfFrames ) )
            {
                ++pointerStructure->mVisualizerCounter;
                ( *pointerStructure->mThermalVisualizer )( time );
            }
        }
    }

    void SetPowerDissipation_Th( const size_t *pointerStructureAddress, const real_T *const *powerDissipationInputVector )
    {
        PointerStructure_Th *pointerStructure = (PointerStructure_Th *)*pointerStructureAddress;
        const size_t numberOfCells = pointerStructure->mThermalSimulation->mThermalStates.size();
        for ( size_t i = 0; i < numberOfCells; ++i )
            pointerStructure->mThermalSimulation->mThermalStates[i]->SetFixedPowerDissipation( *powerDissipationInputVector[i] );
    }

    void SetAirTemperature_Th( const size_t *pointerStructureAddress, const real_T *airTemperature )
    {
        PointerStructure_Th *pointerStructure = (PointerStructure_Th *)*pointerStructureAddress;
        pointerStructure->mThermalSimulation->mThermalSystem->ResetAirTemperature( *airTemperature );
    }

    void RunSystem_Th( const size_t *pointerStructureAddress, real_T *stateVector, real_T *dxdt )
    {
        if ( stateVector == 0 )
        {
            abort();    // TODO implement later: Falls kein StateVector von aussen gegeben ist wird nur der interne benutzt.
        }

        PointerStructure_Th *pointerStructure = (PointerStructure_Th *)*pointerStructureAddress;
        const size_t stateSize = pointerStructure->mThermalSimulation->mThermalSystem->GetOdeSystemSize();
        std::vector< myScalar > internalDxdt( stateSize );


        // Write simulink states into internal states
        memcpy( &pointerStructure->mThermalSimulation->mTemperatures[0], stateVector, stateSize * sizeof( real_T ) );

        // Calculate
        pointerStructure->mThermalSimulation->mThermalSystem->Update( 0.0, 0.0 );    // If SetFixedPowerDissipation() is not
                                                                                     // used, then Update(time + dt, dt);
        pointerStructure->mThermalSimulation->mThermalSystem->operator()( pointerStructure->mThermalSimulation->mTemperatures,
                                                                          internalDxdt, 0.0 );
        pointerStructure->mThermalSimulation->mThermalSystem->SetTemperatureVector( pointerStructure->mThermalSimulation->mTemperatures );
        pointerStructure->mThermalSimulation->UpdateAllThermalStatesTemperatures();


        // Write calculated internal states and dxdt into simulink states and dxdt
        memcpy( (void *)dxdt, &internalDxdt[0], stateSize * sizeof( real_T ) );
        memcpy( (void *)stateVector, &pointerStructure->mThermalSimulation->mTemperatures[0], stateSize * sizeof( real_T ) );
    }

    void DeleteSystem_Th( size_t *pointerStructureAddress )
    {
        if ( !pointerStructureAddress )
            return;

        PointerStructure_Th *pointerStructure = (PointerStructure_Th *)*pointerStructureAddress;
        delete pointerStructure;
    }

#ifdef __cplusplus
}
#endif
#endif
