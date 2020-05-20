/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.h
* Creation Date : 23-05-2013
* Last Modified : Do 03 Mär 2016 16:00:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CINTERFACE_
#define _CINTERFACE_

#include "cinterface_common_functions.h"


#ifdef __cplusplus
extern "C" {
#endif

/// Initializes a system and returns the adress of the DAE system as a size_t
void InitializeSystem( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector = 0,
                       size_t *definedStateSize = 0, size_t *definedOutputSize = 0 );

/// Get the real state size from the xml file
size_t GetRealStateSize( const size_t *pointerStructureAddress );


size_t GetDifferentialStateSize( const size_t *pointerStructureAddress );

/// Get the real output size from the xml file
size_t GetRealOutputSize( const size_t *pointerStructureAddress );

/// Resets the system. Meaning the old system will be deleted and a new one initialized
void ResetSystem( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector = 0,
                  real_T *voltageOutputVec = 0, real_T *currentOutputVec = 0, real_T *powerOutputVec = 0,
                  real_T *socOutputVec = 0, real_T *socSurfaceOutputVec = 0 );

/// Trigger the observer
void TriggerObserver( const size_t *pointerStructureAddress, double t );

/// Calculate Voltage, Current and Power of the hole System
void OutputRootElementData( const size_t *pointerStructureAddress, real_T *totalVoltageOutput,
                            real_T *totalCurrentOutput, real_T *totalPowerOutput );

/// Late Binding for the output vectors addresses
void SetOutputVectors( const size_t *pointerStructureAddress, real_T *voltageOutputVec, real_T *currentOutputVec,
                       real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec );

/// This function sets a new curren which will be applied to th the root twoport
void SetCurrent( const size_t *pointerStructureAddress, const double current );

/// This function sets a new cell element temperatures
void SetCellElementTemperatures( const size_t *pointerStructureAddress, const real_T *const *cellElementTemperaturesVector );

// Gets the  number of cell elements
size_t GetNumberOfCellElements( const size_t *pointerStructureAddress );

void ResetSoc( const size_t *pointerStructureAddress );

void SaveSoc( const size_t *pointerStructureAddress );

/// RunSystem returns dx/dt depending on the provided oldState
/// This function evaluates the Kirchhoff voltage law and creates state space matrices for the differential equations of
/// the network
void RunSystem( const size_t *pointerStructureAddress, real_T *stateVector, real_T *returnDxDt );

void IntegrateStep( const size_t *pointerStructureAddress, const real_T *const dxdt, const double dt );

void RunAlgebraicElectricalSystemPart( const size_t *pointerStructureAddress, real_T *stateVector, const double dt );

void InitOutputports( const size_t *pointerStructureAddress, const double dt );

/// This function sets the Current for balancing purpose
void SetBalancingCurrent( const size_t *pointerStructureAddress, const real_T *const *balancingCurrentVector );

/// Free the memory that is used by the DAE
void DeleteSystem( size_t *pointerStructureAddress );

/// Output a dot readable file
void PrintDotOutput( size_t *pointerStructureAddress );

/// Output a spicenetwork
void PrintSpiceOutput( size_t *pointerStructureAddress );

#ifdef __cplusplus
}
#endif

/// DESCRIPTION
#endif /* _CINTERFACE_ */
