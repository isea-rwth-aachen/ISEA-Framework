/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.cpp
* Creation Date : 23-05-2013
* Last Modified : Do 03 Mär 2016 15:58:09 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "cinterface.h"
#include "pointer_structure.h"

#include <boost/scoped_ptr.hpp>

#include "../export/esbVisualizer.h"
#include "../export/spiceExport.h"
#include "../misc/fast_copy_matrix.h"
#include "../observer/filter/filter.h"
#include "../electrical/electrical_simulation.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"

#ifndef _SYMBOLIC_

#ifndef __NO_STRING__
#include <fstream>
#endif

typedef myMatrixType myUnit;
typedef double myScalar;

#ifdef __cplusplus
extern "C"
{
#endif

    void InitializeSystem( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector,
                           size_t *definedStateSize, size_t *definedOutputSize )
    {
        xmlparser::tinyxml2::XmlParserImpl parser;
        parser.ReadFromMem( configStr );
        boost::shared_ptr< xmlparser::XmlParameter > rootXmlNode = parser.GetRoot();

        PointerStructure *pointerStructure = new PointerStructure;
        *pointerStructureAddress = (size_t)pointerStructure;
        std::vector< boost::shared_ptr< ::electrical::TwoPort< myUnit > > > cells;
        pointerStructure->mElectricalSimulation.reset(
         new simulation::ElectricalSimulation< myUnit, myScalar, false >( rootXmlNode, 0.0, 0.0, 0.0, &cells, nullptr ) );

        GetDefinedSizes( configStr, definedStateSize, definedOutputSize, 0, 0 );

        if ( stateVector )
        {
            const myUnit &initstate = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector;
            misc::FastCopyMatrix( stateVector, initstate, initstate.rows() - 1 );    // memcpy((void*) stateVector,
                                                                                     // mxGetPr(initstate.GetMxArray()),
                                                                                     // initstate.rows() * sizeof(real_T));
        }
    }

    size_t GetDifferentialStateSize( const size_t *pointerStructureAddress )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;    // cast

        return pointerStructure->mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount();
    }

    size_t GetRealStateSize( const size_t *pointerStructureAddress )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;    // cast

        return pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();
    }

    size_t GetRealOutputSize( const size_t *pointerStructureAddress )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;    // cast

        return pointerStructure->mElectricalSimulation->mObserver->GetObservedTwoPortsPtr().size();
    }

    void ResetSystem( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector, real_T *voltageOutputVec,
                      real_T *currentOutputVec, real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec )
    {
        DeleteSystem( pointerStructureAddress );
        InitializeSystem( configStr, pointerStructureAddress, stateVector );
        SetOutputVectors( pointerStructureAddress, voltageOutputVec, currentOutputVec, powerOutputVec, socOutputVec, socSurfaceOutputVec );
    }

    void TriggerObserver( const size_t *pointerStructureAddress, double t )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->mObserver->operator()( t );
    }

    void OutputRootElementData( const size_t *pointerStructureAddress, real_T *totalVoltageOutput,
                                real_T *totalCurrentOutput, real_T *totalPowerOutput )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;

        *totalVoltageOutput = pointerStructure->mElectricalSimulation->mRootTwoPort->GetVoltageValue();
        *totalCurrentOutput = pointerStructure->mElectricalSimulation->mRootTwoPort->GetCurrentValue();
        *totalPowerOutput = pointerStructure->mElectricalSimulation->mRootTwoPort->GetPowerValue();
    }

    void SetOutputVectors( const size_t *pointerStructureAddress, real_T *voltageOutputVec, real_T *currentOutputVec,
                           real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;


        pointerStructure->mElectricalSimulation->mObserver =
         CreateTwoPortObserver< std::vector< boost::shared_ptr< electrical::TwoPort< myUnit > > >, myUnit, false >(
          &pointerStructure->mElectricalSimulation->mObserver->GetObservedTwoPorts(), 0, voltageOutputVec,
          currentOutputVec, powerOutputVec, socOutputVec, socSurfaceOutputVec );
    }

    void SetCurrent( const size_t *pointerStructureAddress, const double current )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->mRootTwoPort->SetCurrent( current );
    }

    void SetCellElementTemperatures( const size_t *pointerStructureAddress, const real_T *const *cellElementTemperaturesVector )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        const size_t numberOfCellElements = pointerStructure->mElectricalSimulation->mThermalStates.size();
        for ( size_t i = 0; i < numberOfCellElements; ++i )
        {
            pointerStructure->mElectricalSimulation->mThermalStates[i]->ResetTemperature();
            pointerStructure->mElectricalSimulation->mThermalStates[i]->AddTemperature( *cellElementTemperaturesVector[i], 1.0 );
        }
    }

    size_t GetNumberOfCellElements( const size_t *pointerStructureAddress )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        return pointerStructure->mElectricalSimulation->mThermalStates.size();
    }

    void RunSystem( const size_t *pointerStructureAddress, real_T *stateVector, real_T *returnDxDt )
    {
        if ( stateVector == 0 )
        {
            abort();    // TODO implement later: Algebraic System is implemented, if the state vector is zero, we should
                        // never reach this point, therefore the abort is okay!
        }

        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        const size_t stateSize = pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();


        // Initialize variables and write simulink states into software states
        myUnit &internalStateVector = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector;
        myUnit internalDxdt( stateSize, 1 );


// Do simulation
#ifdef _EIGEN_
        pointerStructure->mElectricalSimulation->mEqSystem->operator()(
         pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.topRows( internalDxdt.rows() ), internalDxdt, 0.0 );
#else
    pointerStructure->mElectricalSimulation->mEqSystem->operator()(
     pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector.submat( 0, 0, internalDxdt.rows() - 1, 0 ),
     internalDxdt, 0.0 );
#endif


        // Write calculated internal states and dxdt into simulink states and dxdt
        misc::FastCopyMatrix( returnDxDt, internalDxdt, stateSize );

        misc::FastCopyMatrix( stateVector, internalStateVector, stateSize );
    }

    // Seems to be only needed in unittest TestCinterface, check if obsolete!
    void IntegrateStep( const size_t *pointerStructureAddress, const real_T *const dxdt, const double dt )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        myUnit &stateVector = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector;
        for ( size_t i = 0; i < stateVector.rows() - 1; ++i )
            stateVector.coeffRef( i, 0 ) += dxdt[i] * dt;
        stateVector.coeffRef( stateVector.rows() - 1, 0 ) = 1;

        // Calculate all the Voltages, Currents, Powers and also integrate the Socs
        // pointerStructure->mElectricalSimulation->UpdateSystemValues();
    }

    void RunAlgebraicElectricalSystemPart( const size_t *pointerStructureAddress, real_T *stateVector, const double dt )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->mStateSystemGroup.mDt = dt;

        if ( stateVector )
        {
            const size_t stateSize = pointerStructure->mElectricalSimulation->mStateSystemGroup.GetStateCount();
            myUnit &internalStateVector = pointerStructure->mElectricalSimulation->mStateSystemGroup.mStateVector;
            misc::FastCopyMatrix( internalStateVector, stateVector, stateSize );

            internalStateVector.coeffRef( stateSize, 0 ) = 1;
        }

        pointerStructure->mElectricalSimulation->UpdateSystem();
        pointerStructure->mElectricalSimulation->UpdateSystemValues();
    }
    void SetBalancingCurrent( const size_t *pointerStructureAddress, const real_T *const *balancingCurrentVector )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
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

    void ResetSoc( const size_t *pointerStructureAddress )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->LoadCapacityForLaterReset();
    }

    void SaveSoc( const size_t *pointerStructureAddress )
    {
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        pointerStructure->mElectricalSimulation->SaveCapacityForLaterReset();
    }

    void DeleteSystem( size_t *pointerStructureAddress )
    {
        if ( !pointerStructureAddress )
            return;

        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        delete pointerStructure;
        *pointerStructureAddress = 0;
    }

    void PrintDotOutput( size_t *
#ifndef __NO_STRING__
                          pointerStructureAddress
#endif
    )
    {
#ifndef __NO_STRING__
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
        if ( !pointerStructure )
            return;

        std::ofstream outstream;
        outstream.open( "esb.dot" );
        visualizer::EsbVisualizer< myUnit > testVisualizer( pointerStructure->mElectricalSimulation->mRootTwoPort.get(), &outstream );
#endif
    }

    void PrintSpiceOutput( size_t *
#ifndef __NO_STRING__
                            pointerStructureAddress
#endif
    )
    {
#ifndef __NO_STRING__
        PointerStructure *pointerStructure = (PointerStructure *)*pointerStructureAddress;
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
