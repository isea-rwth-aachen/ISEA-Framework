#include "cinterface_ThEl_Aging.h"
#include "cinterface_ThEl.h"
#include "pointer_structure.h"

#include "../aging/aging_simulation.h"
#include "../aging/aging_twoport.h"
#include "../factory/factorybuilder_for_aging.h"
#include "../misc/matrixInclude.h"
#include "../thermal/simulation_data_outline.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"

#include <boost/scoped_ptr.hpp>

#ifndef _SYMBOLIC_

typedef myMatrixType myUnit;
typedef double myScalar;

#ifdef __cplusplus
extern "C"
{
#endif

    void InitializeSystem_ThEl_Aging( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();


        PointerStructure_ThEl_Aging *pointerStructure = new PointerStructure_ThEl_Aging;
        *pointerStructureAddress = (size_t)pointerStructure;

        factory::FactoryBuilderForAging< myUnit, myScalar > factoryBuilder;
        pointerStructure->mElectricalSimulation.reset(
         new simulation::ElectricalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0, nullptr, &factoryBuilder ) );

        pointerStructure->mThermalSimulation.reset(
         new simulation::ThermalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0, &pointerStructure->mThermalVisualizer,
                                                                       &pointerStructure->mElectricalSimulation->mThermalStates, 0 ) );

        pointerStructure->mAgingSimulation.reset(
         new simulation::AgingSimulation< myUnit, myScalar, false >( rootXmlNode, pointerStructure->mElectricalSimulation,
                                                                     pointerStructure->mThermalSimulation, {}, 0.0, &factoryBuilder ) );

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
            const size_t electricalElements = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.rows() - 1;
            misc::FastCopyMatrix( stateVector, pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector,
                                  electricalElements );
            memcpy( &( stateVector[electricalElements] ), &( pointerStructure->mThermalSimulation->mTemperatures[0] ),
                    pointerStructure->mThermalSimulation->mTemperatures.size() * sizeof( myScalar ) );
        }
    }

    void ResetSystem_ThEl_Aging( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector,
                                 real_T *voltageOutputVec, real_T *currentOutputVec, real_T *powerOutputVec,
                                 real_T *socOutputVec, real_T *socSurfaceOutputVec )
    {
        DeleteSystem_ThEl( pointerStructureAddress );
        InitializeSystem_ThEl_Aging( configStr, pointerStructureAddress, stateVector );
        SetOutputVectors_ThEl( pointerStructureAddress, voltageOutputVec, currentOutputVec, powerOutputVec,
                               socOutputVec, socSurfaceOutputVec );
    }

    void SetOutputVectors_ThEl_Aging( const size_t *pointerStructureAddress, real_T *voltageOutputVec, real_T *currentOutputVec,
                                      real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec )
    {
        PointerStructure_ThEl_Aging *pointerStructure = (PointerStructure_ThEl_Aging *)*pointerStructureAddress;

        std::vector< boost::shared_ptr< electrical::TwoPort< myUnit > > > outputTwoports;
        const auto &agingTwoports = pointerStructure->mAgingSimulation->mAgingTwoPorts;
        outputTwoports.reserve( agingTwoports.size() );
        for ( const auto &twoport : agingTwoports )
        {
            outputTwoports.push_back( twoport->GetTwoPort() );
        }

        pointerStructure->mElectricalSimulation->mObserver =
         CreateTwoPortObserver< std::vector< boost::shared_ptr< electrical::TwoPort< myUnit > > >, myUnit, false >(
          &outputTwoports, 0, voltageOutputVec, currentOutputVec, powerOutputVec, socOutputVec, socSurfaceOutputVec );
    }

    void GetRealSizes_ThEl_Aging( const char *configStr, const size_t *pointerStructureAddress, size_t *stateSize,
                                  size_t *numberOfCells, size_t *probeSize, size_t *agingTwoportSize )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();

        size_t themalStateSize = 0, electricalStateSize = 0;

        simulation::ThermalModelOutline< double >( rootXmlNode, numberOfCells, &themalStateSize, 0, probeSize );
        if ( pointerStructureAddress )
        {
            PointerStructure_ThEl_Aging *pointerStructure = (PointerStructure_ThEl_Aging *)*pointerStructureAddress;
            electricalStateSize = pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();
            *agingTwoportSize = pointerStructure->mAgingSimulation->mAgingTwoPorts.size();
        }
        else
        {
            PointerStructure_ThEl_Aging pointerStructure;
            factory::FactoryBuilderForAging< myUnit, myScalar > factoryBuilder;
            pointerStructure.mElectricalSimulation.reset(
             new simulation::ElectricalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0, nullptr, &factoryBuilder ) );
            pointerStructure.mAgingSimulation.reset(
             new simulation::AgingSimulation< myUnit, myScalar, false >( rootXmlNode, pointerStructure.mElectricalSimulation,
                                                                         nullptr, {}, 0, &factoryBuilder ) );
            electricalStateSize = pointerStructure.mElectricalSimulation->mStateSystemGroup.GetStateCount();
            *agingTwoportSize = pointerStructure.mAgingSimulation->mAgingTwoPorts.size();
        }
        if ( stateSize )
            *stateSize = themalStateSize + electricalStateSize;
    }

    void SetAging_ThEl_Aging( const size_t *pointerStructureAddress, const real_T *capFactors, const real_T *resFactors,
                              const real_T *socOffsets, const real_T dt )
    {
        PointerStructure_ThEl_Aging *pointerStructure = (PointerStructure_ThEl_Aging *)*pointerStructureAddress;
        for ( size_t i = 0; i < pointerStructure->mAgingSimulation->mAgingTwoPorts.size(); ++i )
        {
            double cap = capFactors[i];
            double res = resFactors[i];
            double offset = socOffsets[i];
            // if the factors are zero, assume the inputs are unconnected
            if ( cap == 0.0 )
                cap = 1.0;
            if ( res == 0.0 )
                res = 1.0;
            pointerStructure->mAgingSimulation->mAgingTwoPorts[i]->CollectData( dt );
            pointerStructure->mAgingSimulation->mAgingTwoPorts[i]->SetAging( cap, res, offset, dt );
        }
    }

    void OutputAgingData_ThEl_Aging( const size_t *pointerStructureAddress, real_T *capVector, real_T *ctpVector, real_T *socOffsetVector )
    {
        PointerStructure_ThEl_Aging *pointerStructure = (PointerStructure_ThEl_Aging *)*pointerStructureAddress;
        const auto &agingPorts = pointerStructure->mAgingSimulation->mAgingTwoPorts;
        for ( size_t i = 0; i < agingPorts.size(); ++i )
        {
            capVector[i] = agingPorts[i]->GetTwoPort()->GetTotalCapacity();
            ctpVector[i] = agingPorts[i]->GetAgingState()->GetChargeThroughput();
            socOffsetVector[i] = agingPorts[i]->GetAgingState()->GetSocOffset();
        }
    }

    void TriggerObserverAndTemperatureOutput_ThEl_Aging( const size_t *pointerStructureAddress, double t,
                                                         real_T *temperatureOutputVector, real_T *thermalProbeOutputVector )
    {
        PointerStructure_ThEl_Aging *pointerStructure = (PointerStructure_ThEl_Aging *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->mObserver->operator()( t );

        const auto &agingTwoports = pointerStructure->mAgingSimulation->mAgingTwoPorts;

        if ( temperatureOutputVector )
            for ( size_t i = 0; i < agingTwoports.size(); ++i )
                temperatureOutputVector[i] =
                 static_cast< real_T >( agingTwoports[i]->GetTwoPort()->GetThermalState()->GetValue() );

        if ( thermalProbeOutputVector )
        {
            size_t counter = 0;
            for ( std::vector< ::probe::ThermalProbe >::const_iterator probe =
                   pointerStructure->mThermalSimulation->mThermalProbes->begin();
                  probe != pointerStructure->mThermalSimulation->mThermalProbes->end(); ++probe )
            {
                thermalProbeOutputVector[counter] = probe->GetTemperature();
                ++counter;
            }
        }
    }

#ifdef __cplusplus
}
#endif
#endif
