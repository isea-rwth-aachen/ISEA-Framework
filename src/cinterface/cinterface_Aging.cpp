#include "cinterface_Aging.h"
#include "pointer_structure.h"

#include "../aging/aging_simulation.h"
#include "../aging/aging_twoport.h"
#include "../factory/factorybuilder_without_electrical_elements.h"
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

    void InitializeSystem_Aging( const char *configStr, size_t *pointerStructureAddress, real_T *agingStepTime )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();

        double xmlAgingSteptime =
         rootXmlNode->GetElementChild( "Options" )->GetElementChild( "Aging" )->GetElementDoubleValue( "AgingStepTime" );

        PointerStructure_Aging *pointerStructure = new PointerStructure_Aging;
        *pointerStructureAddress = (size_t)pointerStructure;
        factory::FactoryBuilderWithoutElectricalElements< myUnit, myScalar > factoryBuilder;

        auto electricalSimulation =
         boost::make_shared< simulation::ElectricalSimulation< myUnit, myScalar, false > >( rootXmlNode, 0.0, 0.0, 0.0,
                                                                                            nullptr, &factoryBuilder );

        pointerStructure->mAgingSimulation.reset(
         new simulation::AgingSimulation< myUnit, myScalar, false >( rootXmlNode, electricalSimulation, nullptr, {},
                                                                     xmlAgingSteptime, &factoryBuilder ) );

        if ( agingStepTime )
            *agingStepTime = xmlAgingSteptime;
    }

    void DeleteSystem_Aging( size_t *pointerStructureAddress )
    {
        if ( !pointerStructureAddress )
            return;

        PointerStructure_Aging *pointerStructure = (PointerStructure_Aging *)*pointerStructureAddress;
        delete pointerStructure;
        *pointerStructureAddress = 0;
    }

    void ResetSystem_Aging( const char *configStr, size_t *pointerStructureAddress )
    {
        DeleteSystem_Aging( pointerStructureAddress );
        InitializeSystem_Aging( configStr, pointerStructureAddress, nullptr );
    }

    void GetRealSizes_Aging( const char *configStr, const size_t *pointerStructureAddress, size_t *agingTwoportSize )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();

        if ( pointerStructureAddress )
        {
            PointerStructure_Aging *pointerStructure = (PointerStructure_Aging *)*pointerStructureAddress;
            *agingTwoportSize = pointerStructure->mAgingSimulation->mAgingTwoPorts.size();
        }
        else
        {
            PointerStructure_Aging pointerStructure;

            factory::FactoryBuilderWithoutElectricalElements< myUnit, myScalar > factoryBuilder;
            auto electricalSimulation =
             boost::make_shared< simulation::ElectricalSimulation< myUnit, myScalar, false > >( rootXmlNode, 0.0, 0.0, 0.0,
                                                                                                nullptr, &factoryBuilder );

            pointerStructure.mAgingSimulation.reset(
             new simulation::AgingSimulation< myUnit, myScalar, false >( rootXmlNode, electricalSimulation, nullptr, {},
                                                                         0.0, &factoryBuilder ) );
            *agingTwoportSize = pointerStructure.mAgingSimulation->mAgingTwoPorts.size();
        }
    }

    void CollectData_Aging( const size_t *pointerStructureAddress, const real_T *voltageVector, const real_T *currentVector,
                            const real_T *socVector, const real_T *temperatureVector, const real_T dt )
    {
        PointerStructure_Aging *pointerStructure = (PointerStructure_Aging *)*pointerStructureAddress;
        for ( size_t i = 0; i < pointerStructure->mAgingSimulation->mAgingTwoPorts.size(); ++i )
        {
            electrical::TwoPortWithState< myUnit > &twoport =
             *pointerStructure->mAgingSimulation->mAgingTwoPorts[i]->GetTwoPort();
            twoport.LoadInternalData( currentVector[i], voltageVector[i], 0.0 );
            twoport.GetSoc()->SetStoredEnergy< state::SocSetFormat::FACTOR >( socVector[i] / 100.0 );
            twoport.GetThermalState()->SetTemperature( temperatureVector[i] );

            pointerStructure->mAgingSimulation->mAgingTwoPorts[i]->CollectData( dt );
        }
    }

    void ResetToPointInTime_Aging( const size_t *pointerStructureAddress, const real_T time )
    {
        PointerStructure_Aging *pointerStructure = (PointerStructure_Aging *)*pointerStructureAddress;
        pointerStructure->mAgingSimulation->ResetToPointInTime( time );
    }

    void Calculate_Aging( const size_t *pointerStructureAddress, real_T *relCapVector, real_T *relResVector,
                          real_T *socOffsetVector, real_T timestep )
    {
        PointerStructure_Aging *pointerStructure = (PointerStructure_Aging *)*pointerStructureAddress;
        pointerStructure->mAgingSimulation->CalculateAging( timestep, true );
        for ( size_t i = 0; i < pointerStructure->mAgingSimulation->mAgingTwoPorts.size(); ++i )
        {
            state::AgingState &agingState = *pointerStructure->mAgingSimulation->mAgingTwoPorts[i]->GetAgingState();
            relCapVector[i] = agingState.mCapacityFactor;
            relResVector[i] = agingState.mResistanceFactor;
            socOffsetVector[i] = agingState.mSocOffset;
        }
    }

    void Calculate_Aging_Variable_Steptime( const size_t *pointerStructureAddress, real_T *relCapVector, real_T *relResVector,
                                            real_T *socOffsetVector, real_T timestep, real_T scaleFactor )
    {
        PointerStructure_Aging *pointerStructure = (PointerStructure_Aging *)*pointerStructureAddress;
        pointerStructure->mAgingSimulation->CalculateAging( timestep, scaleFactor );
        for ( size_t i = 0; i < pointerStructure->mAgingSimulation->mAgingTwoPorts.size(); ++i )
        {
            state::AgingState &agingState = *pointerStructure->mAgingSimulation->mAgingTwoPorts[i]->GetAgingState();
            relCapVector[i] = agingState.mCapacityFactor;
            relResVector[i] = agingState.mResistanceFactor;
            socOffsetVector[i] = agingState.mSocOffset;
        }
    }

#ifdef __cplusplus
}
#endif
#endif
