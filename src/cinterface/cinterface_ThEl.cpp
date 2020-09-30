/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.cpp
* Creation Date : 23-05-2013
* Last Modified : Di 06 Okt 2015 18:54:34 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "cinterface_ThEl.h"
#include "pointer_structure.h"

// BOOST
#include <boost/scoped_ptr.hpp>

// ETC
#include "../misc/fast_copy_matrix.h"
#include "../misc/matrixInclude.h"
#include "../observer/filter/filter.h"
#include "../operators/vectorOperator.h"

#include "../electrical/electrical_simulation.h"
#include "../export/esbVisualizer.h"
#include "../export/spiceExport.h"
#include "../observer/filter/vcpfilter.h"
#include "../thermal/simulation_data_outline.h"
#include "../thermal/thermal_simulation.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"

#include "../misc/macros.h"

#ifndef _SYMBOLIC_

#ifndef __NO_STRING__
#include <fstream>
#endif

#ifdef __EXCEPTIONS__
#include <string>
#endif


typedef myMatrixType myUnit;
typedef double myScalar;


#ifdef __cplusplus
extern "C"
{
#endif

    void InitializeSystem_ThEl( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();


        PointerStructure_ThEl *pointerStructure = new PointerStructure_ThEl;
        *pointerStructureAddress = (size_t)pointerStructure;

        std::vector< boost::shared_ptr< ::electrical::TwoPort< myUnit > > > cells;
        pointerStructure->mElectricalSimulation.reset(
         new simulation::ElectricalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0, &cells ) );


        pointerStructure->mThermalSimulation.reset(
         new simulation::ThermalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0, &pointerStructure->mThermalVisualizer,
                                                                       &pointerStructure->mElectricalSimulation->mThermalStates, 0 ) );

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

    void ResetSystem_ThEl( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector, real_T *voltageOutputVec,
                           real_T *currentOutputVec, real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec )
    {
        DeleteSystem_ThEl( pointerStructureAddress );
        InitializeSystem_ThEl( configStr, pointerStructureAddress, stateVector );
        SetOutputVectors_ThEl( pointerStructureAddress, voltageOutputVec, currentOutputVec, powerOutputVec,
                               socOutputVec, socSurfaceOutputVec );
    }

    void TriggerObserverAndCellTemperatureOutput_ThEl( const size_t *pointerStructureAddress, double t,
                                                       real_T *cellTemperatureOutputVector, real_T *thermalProbeOutputVector )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->mObserver->operator()( t );

        if ( cellTemperatureOutputVector )
            for ( size_t i = 0; i < pointerStructure->mThermalSimulation->mThermalStates.size(); ++i )
                cellTemperatureOutputVector[i] =
                 static_cast< real_T >( pointerStructure->mThermalSimulation->mThermalStates[i]->GetValue() );

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

    void TriggerVisualizer_ThEl( const size_t *pointerStructureAddress, double t, bool forceTriggering )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        if ( pointerStructure->mVisualizerTimeDiffFrames != 0.0 )
        {
            if ( ( forceTriggering && pointerStructure->mVisualizerCounter < pointerStructure->mVisualizerMaxNumberOfFrames ) ||
                 pointerStructure->mVisualizerTimeDiffFrames < 0.0 ||
                 ( t >= pointerStructure->mVisualizerTimeDiffFrames * pointerStructure->mVisualizerCounter &&
                   pointerStructure->mVisualizerCounter < pointerStructure->mVisualizerMaxNumberOfFrames ) )
            {
                ++pointerStructure->mVisualizerCounter;
                ( *pointerStructure->mThermalVisualizer )( t );
            }
        }
    }

    void OutputRootElementData_ThEl( const size_t *pointerStructureAddress, real_T *totalVoltageOutput,
                                     real_T *totalCurrentOutput, real_T *totalPowerOutput )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;

        *totalVoltageOutput = pointerStructure->mElectricalSimulation->mRootTwoPort->GetVoltageValue();
        *totalCurrentOutput = pointerStructure->mElectricalSimulation->mRootTwoPort->GetCurrentValue();
        *totalPowerOutput = pointerStructure->mElectricalSimulation->mRootTwoPort->GetPowerValue();
    }

    void SetOutputVectors_ThEl( const size_t *pointerStructureAddress, real_T *voltageOutputVec, real_T *currentOutputVec,
                                real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;

        size_t numberOfCellelements = pointerStructure->mElectricalSimulation->mCellElements.size();
        const auto &observedTwoports = pointerStructure->mElectricalSimulation->mObserver->GetObservedTwoPorts();
        if ( observedTwoports.size() > numberOfCellelements )
        {
            // Reorder the observed twoports so that all elements inside one cell end up in the same row in the output
            // matrix. The vector is interpreted as a column-major matrix with the number of rows equal to the number of cells
            std::vector< boost::shared_ptr< electrical::TwoPort< myMatrixType > > > twoportVector( observedTwoports.size(), nullptr );
            size_t outputRows = numberOfCellelements;
            size_t outputColumns = std::ceil( (double)observedTwoports.size() / outputRows );
            // first column has all the cellements, so they can just be copied
            for ( size_t i = 0; i < numberOfCellelements; ++i )
            {
                twoportVector[i] = observedTwoports[i];
            }
            for ( size_t i = numberOfCellelements; i < observedTwoports.size(); ++i )
            {
                size_t row = ( i - numberOfCellelements ) / ( outputColumns - 1 );
                size_t column = ( i - numberOfCellelements ) % ( outputColumns - 1 ) + 1;
                twoportVector[column * outputRows + row] = observedTwoports[i];
            }
            pointerStructure->mElectricalSimulation->mObserver =
             CreateTwoPortObserver< std::vector< boost::shared_ptr< electrical::TwoPort< myUnit > > >, myUnit, false >(
              &twoportVector, 0, voltageOutputVec, currentOutputVec, powerOutputVec, socOutputVec, socSurfaceOutputVec );
        }
        else
        {
            pointerStructure->mElectricalSimulation->mObserver =
             CreateTwoPortObserver< std::vector< boost::shared_ptr< electrical::TwoPort< myUnit > > >, myUnit, false >(
              &observedTwoports, 0, voltageOutputVec, currentOutputVec, powerOutputVec, socOutputVec, socSurfaceOutputVec );
        }
    }

    void GetRealSizes_ThEl( const char *configStr, const size_t *pointerStructureAddress, size_t *stateSize,
                            size_t *observerSize, size_t *numberOfCells, size_t *probeSize )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();

        size_t themalStateSize = 0, electricalStateSize = 0, observerSizeSave = 0;

        simulation::ThermalModelOutline< double >( rootXmlNode, numberOfCells, &themalStateSize, 0, probeSize );
        if ( pointerStructureAddress )
        {
            PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
            electricalStateSize = pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();
            observerSizeSave = pointerStructure->mElectricalSimulation->mObserver->GetObservedTwoPorts().size();
        }
        else
        {
            PointerStructure_ThEl pointerStructure;
            std::vector< boost::shared_ptr< ::electrical::TwoPort< myMatrixType > > > cells;
            pointerStructure.mElectricalSimulation.reset(
             new simulation::ElectricalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0, &cells ) );
            electricalStateSize = pointerStructure.mElectricalSimulation->mStateSystemGroup.GetStateCount();
            observerSizeSave = pointerStructure.mElectricalSimulation->mObserver->GetObservedTwoPorts().size();
        }
        if ( observerSize )
            *observerSize = observerSizeSave;
        if ( stateSize )
            *stateSize = themalStateSize + electricalStateSize;
    }

    size_t GetNumberOfStatesForShowInternalStates( const size_t *pointerStructureAddress )
    {
        if ( pointerStructureAddress )
        {
            PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
            return pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount() +
                   pointerStructure->mThermalSimulation->mTemperatures.size();
        }
        else
        {
            abort();
        }
    }


    void SetCurrent_ThEl( const size_t *pointerStructureAddress, const double current )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->mRootTwoPort->SetCurrent( current );
    }

    void ResetSoc_ThEl( const size_t *pointerStructureAddress )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->LoadCapacityForLaterReset();
    }

    void SaveSoc_ThEl( const size_t *pointerStructureAddress )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->SaveCapacityForLaterReset();
    }


    void RunSystem_ThEl( const size_t *pointerStructureAddress, real_T *stateVector, real_T *dxdt )
    {
        // time input parameter has been erased, because UpdateAllThermalStatesPowerDissipationWithoutTime() is used
        // instead of UpdateAllThermalStatesPowerDissipation()
        if ( stateVector == 0 )
        {
            abort();    // TODO implement later: Falls kein StateVector von aussen gegeben ist wird nur der interne benutzt.
        }


        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        const size_t electricalStateSize = pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();
        const size_t temperatureSize = pointerStructure->mThermalSimulation->mThermalSystem->GetOdeSystemSize();
        const size_t stateSize = electricalStateSize + temperatureSize;


        // Inititalization of variables
        myUnit dxdtElectrical( electricalStateSize, 1 );
        std::vector< myScalar > dxdtThermal( temperatureSize );
        myUnit internalStateVector( stateSize, 1 );
        myUnit internalDxdt( stateSize, 1 );


// Electrical, only ode part, algebraic part already solved by RunAlgebraicElectricalSystemPart_ThEl
#ifdef _EIGEN_
        pointerStructure->mElectricalSimulation->mEqSystem->operator()(
         pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.topRows(
          pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.rows() - 1 ),
         dxdtElectrical, 0.0 );
#else
    pointerStructure->mElectricalSimulation->mEqSystem->operator()(
     pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.submat(
      0, 0, pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.rows() - 2, 0 ),
     dxdtElectrical, 0.0 );
#endif


        // Thermal
        pointerStructure->mThermalSimulation->mThermalSystem->Update( 0.0, 0.0 );    // because
        // UpdateAllThermalStatesPowerDissipationWithoutTime()
        // is used, else Update(time + dt, dt)
        pointerStructure->mThermalSimulation->mThermalSystem->operator()( pointerStructure->mThermalSimulation->mTemperatures,
                                                                          dxdtThermal, 0.0 );
        pointerStructure->mThermalSimulation->mThermalSystem->SetTemperatureVector( pointerStructure->mThermalSimulation->mTemperatures );
        pointerStructure->mThermalSimulation->UpdateAllThermalStatesTemperatures();


        // Write calculated internal states and dxdt into simulink states and dxdt

        // DXDT rausreichen
        misc::FastCopyMatrix( dxdt, dxdtElectrical, dxdtElectrical.rows() );
        memcpy( &( dxdt[dxdtElectrical.rows()] ), &( dxdtThermal[0] ), dxdtThermal.size() * sizeof( myScalar ) );

        // StateVector rausreichen
        const size_t electricalElements = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.rows() - 1;
        misc::FastCopyMatrix( stateVector, pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector, electricalElements );
        memcpy( &( stateVector[electricalElements] ), &( pointerStructure->mThermalSimulation->mTemperatures[0] ),
                pointerStructure->mThermalSimulation->mTemperatures.size() * sizeof( myScalar ) );
    }

    void RunAlgebraicElectricalSystemPart_ThEl( const size_t *pointerStructureAddress, real_T *stateVector, const double dt )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;

        const size_t electricalStateSize = pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();
        const size_t temperatureSize = pointerStructure->mThermalSimulation->mThermalSystem->GetOdeSystemSize();


        // Write simulink states into software states
        misc::FastCopyMatrix( pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector, stateVector, electricalStateSize );
        memcpy( &pointerStructure->mThermalSimulation->mTemperatures[0], &( stateVector[electricalStateSize] ),
                temperatureSize * sizeof( myScalar ) );
        pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.coeffRef(
         pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.rows() - 1, 0 ) = 1;


        pointerStructure->mElectricalSimulation->mStateSystemGroup.mDt = dt;
        pointerStructure->mElectricalSimulation->UpdateSystem();
        pointerStructure->mElectricalSimulation->UpdateSystemValues();
        pointerStructure->mElectricalSimulation->UpdateAllThermalStatesPowerDissipationWithoutTime();
    }

    void RunAlgebraicElectricalSystemPartDiscret_ThEl( const size_t *pointerStructureAddress, const double dt )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;

        pointerStructure->mElectricalSimulation->mStateSystemGroup.mDt = dt;
        pointerStructure->mElectricalSimulation->UpdateSystem();
        pointerStructure->mElectricalSimulation->UpdateSystemValues();
        pointerStructure->mElectricalSimulation->UpdateAllThermalStatesPowerDissipationWithoutTime();
    }


    void IntegrateOneDiscreteStep( const size_t *pointerStructureAddress, const double dt, real_T *stateVector, real_T *dxdt )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        const size_t electricalStateSize = pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();
        const size_t temperatureSize = pointerStructure->mThermalSimulation->mThermalSystem->GetOdeSystemSize();
        const size_t stateSize = electricalStateSize + temperatureSize;

        // Inititalization of variables
        myUnit dxdtElectrical( electricalStateSize, 1 );
        std::vector< myScalar > dxdtThermal( temperatureSize );

        myUnit &electricalStates = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector;
        std::vector< myScalar > &thermalTemperatures = pointerStructure->mThermalSimulation->mTemperatures;
        myUnit internalStateVector( stateSize, 1 );


// Electrical, only ode part, algebraic part already solved by RunAlgebraicElectricalSystemPart_ThEl
#ifdef _EIGEN_
        pointerStructure->mElectricalSimulation->mEqSystem->operator()(
         pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.topRows( dxdtElectrical.rows() ),
         dxdtElectrical, 0.0 );
#else
    pointerStructure->mElectricalSimulation->mEqSystem->operator()(
     pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.submat( 0, 0, dxdtElectrical.rows() - 1, 0 ),
     dxdtElectrical, 0.0 );
#endif


        // Thermal
        pointerStructure->mThermalSimulation->mThermalSystem->Update( 0.0, 0.0 );    // because
        // UpdateAllThermalStatesPowerDissipationWithoutTime()
        // is used, else Update(time + dt, dt)
        pointerStructure->mThermalSimulation->mThermalSystem->operator()( pointerStructure->mThermalSimulation->mTemperatures,
                                                                          dxdtThermal, 0.0 );

        // Do the Euler step internally

#ifdef _EIGEN_
        electricalStates.topRows( dxdtElectrical.rows() ) += dt * dxdtElectrical;
#else
    electricalStates.submat( 0, 0, dxdtElectrical.rows() - 1, 0 ) += dt * dxdtElectrical;
#endif
        thermalTemperatures = thermalTemperatures + dxdtThermal * dt;

        RunAlgebraicElectricalSystemPartDiscret_ThEl( pointerStructureAddress, dt );

        pointerStructure->mThermalSimulation->mThermalSystem->SetTemperatureVector( pointerStructure->mThermalSimulation->mTemperatures );
        pointerStructure->mThermalSimulation->UpdateAllThermalStatesTemperatures();

        // Write calculated internal states and dxdt into simulink states and dxdt
        // COPY START

        // StateVector rausreichen
        const size_t electricalElements = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.rows() - 1;
        misc::FastCopyMatrix( stateVector, pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector, electricalElements );
        memcpy( &( stateVector[electricalElements] ), &( pointerStructure->mThermalSimulation->mTemperatures[0] ),
                pointerStructure->mThermalSimulation->mTemperatures.size() * sizeof( myScalar ) );
        /// <----- COPY END


        if ( dxdt )
        {
            misc::FastCopyMatrix( dxdt, dxdtElectrical, dxdtElectrical.rows() );
            memcpy( &( dxdt[dxdtElectrical.rows()] ), &( dxdtThermal[0] ), dxdtThermal.size() * sizeof( myScalar ) );
        }
    }

    void SetBalancingCurrent_ThEl( const size_t *pointerStructureAddress, const real_T *const *balancingCurrentVector )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        size_t i = 0;

        std::vector< electrical::TwoPort< myUnit > * > &observedCellelements =
         pointerStructure->mElectricalSimulation->mObserver->GetObservedTwoPortsPtr();
        for ( observer::Filter< myUnit, electrical::TwoPort, observer::PreparationType< myUnit > >::Data_t::iterator it =
               observedCellelements.begin();
              it != observedCellelements.end(); ++it )
        {
            if ( ( *it )->IsCellelement() )
                static_cast< electrical::Cellelement< myUnit > * >( *it )->SetCurrentFromActiveBalancer(
                 *balancingCurrentVector[i++] );
        }
    }

    void SetAirTemperature_ThEl( const size_t *pointerStructureAddress, const real_T *airTemperature )
    {
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        pointerStructure->mThermalSimulation->mThermalSystem->ResetAirTemperature( *airTemperature );
    }

    void DeleteSystem_ThEl( size_t *pointerStructureAddress )
    {
        if ( !pointerStructureAddress )
            return;

        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        delete pointerStructure;
        *pointerStructureAddress = 0;
    }

    void PrintDotOutput_ThEl( size_t *pointerStructureAddress )
    {
        UNUSED( pointerStructureAddress );
#ifndef __NO_STRING__
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        if ( !pointerStructure )
            return;

        std::ofstream outstream;
        outstream.open( "esb.dot" );
        visualizer::EsbVisualizer< myUnit > testVisualizer( pointerStructure->mElectricalSimulation->mRootTwoPort.get(), &outstream );
#endif
    }

    void PrintSpiceOutput_ThEl( size_t *pointerStructureAddress )
    {
        UNUSED( pointerStructureAddress );
#ifndef __NO_STRING__
        PointerStructure_ThEl *pointerStructure = (PointerStructure_ThEl *)*pointerStructureAddress;
        if ( !pointerStructure )
            return;

        std::ofstream outstream;
        outstream.open( "spicenetwork.net" );
        SpiceExport< myUnit > test( pointerStructure->mElectricalSimulation->mRootTwoPort.get(), &outstream );
        outstream.close();
#endif
    }

#ifdef __cplusplus
}
#endif
#endif
