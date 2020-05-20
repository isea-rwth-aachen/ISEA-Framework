/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.h
* Creation Date : 23-05-2013
* Last Modified : Fr 06 Mär 2015 17:07:00 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CINTERFACE_TH_EL_
#define _CINTERFACE_TH_EL_

#include "cinterface_common_functions.h"


#ifdef __cplusplus
extern "C"
{
#endif

    /// @fn void MakeInternalStateVector_ThEl(myUnit &internalStateVector, const myUnit &electricalStates, const std::vector< myScalar > &temperatureVector)
    /// @brief Fuses the electrical states and the temperature vector into an internal state vector. intern state vector must have the right size (n_th + n_el, 1).

    /// @fn void MakeExternalStateVector_ThEl(real_T *externalStateVector, const myUnit &electricalStates, const std::vector< myScalar > &temperatureVector)
    /// @brief Fuses the electrical states and the temperature vector into an external state vector. external state vector must have the right size (n_th + n_el, 1).

    /// Initializes a system and returns the adress of the DAE system as a size_t
    void InitializeSystem_ThEl( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector = 0 );

    /// Resets the system. Meaning the old system will be deleted and a new one initialized
    void ResetSystem_ThEl( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector = 0,
                           real_T *voltageOutputVec = 0, real_T *currentOutputVec = 0, real_T *powerOutputVec = 0,
                           real_T *socOutputVec = 0, real_T *socSurfaceOutputVec = 0 );

    /// Trigger the observer and the output of cell temperatures
    void TriggerObserverAndCellTemperatureOutput_ThEl( const size_t *pointerStructureAddress, double t,
                                                       real_T *cellTemperatureOutputVector, real_T *thermalProbeOutputVector );

    /// Triggers visualizer to write frame into file if visualizer time difference has been superceeded since the last frame
    void TriggerVisualizer_ThEl( const size_t *pointerStructureAddress, double time = 0.0, bool forceTriggering = false );

    /// Calculate Voltage, Current and Power of the hole System
    void OutputRootElementData_ThEl( const size_t *pointerStructureAddress, real_T *totalVoltageOutput,
                                     real_T *totalCurrentOutput, real_T *totalPowerOutput );

    /// Late Binding for the output vectors addresses
    void SetOutputVectors_ThEl( const size_t *pointerStructureAddress, real_T *voltageOutputVec, real_T *currentOutputVec,
                                real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec );

    /// Gets sizes for simulink vectors that are needed to initialize ths S-Function as needed by the equation system
    void GetRealSizes_ThEl( const char *configStr, const size_t *pointerStructureAddress, size_t *stateSize,
                            size_t *observerSize, size_t *numberOfCells, size_t *probeSize );

    /// Gets size of the state and the dxdt vector only to be called with valid pointer structure
    size_t GetNumberOfStatesForShowInternalStates( const size_t *pointerStructureAddress );

    /// This function sets a new curren which will be applied to th the root twoport
    void SetCurrent_ThEl( const size_t *pointerStructureAddress, const double current );

    void ResetSoc_ThEl( const size_t *pointerStructureAddress );

    void SaveSoc_ThEl( const size_t *pointerStructureAddress );

    /// RunSystem_ThEl returns dx/dt depending on the provided oldState
    /// This function evaluates the Kirchhoff voltage law and creates state space matrices for the differential equations of the network
    void RunSystem_ThEl( const size_t *pointerStructureAddress, real_T *stateVector, real_T *dxDt );

    /// Run the algebraic part of the electric system
    void RunAlgebraicElectricalSystemPart_ThEl( const size_t *pointerStructureAddress, real_T *stateVector, const double dt );

    /// Run the algebraic part of the electric system in the case of discrete simu_dgl
    void RunAlgebraicElectricalSystemPartDiscret_ThEl( const size_t *pointerStructureAddress, const double dt );

    /// Do one discrete Euler step and return dxdt if given
    void IntegrateOneDiscreteStep( const size_t *pointerStructureAddress, const double dt, real_T *stateVector, real_T *dxdt = NULL );

    /// This function sets the Current for balancing purpose
    void SetBalancingCurrent_ThEl( const size_t *pointerStructureAddress, const real_T *const *balancingCurrentVector );

    /// This resets the air temperature
    void SetAirTemperature_ThEl( const size_t *pointerStructureAddress, const real_T *airTemperature );

    /// Free the memory that is used by the DAE
    void DeleteSystem_ThEl( size_t *pointerStructureAddress );

    /// Output a dot readable file
    void PrintDotOutput_ThEl( size_t *pointerStructureAddress );

    /// Output a spicenetwork
    void PrintSpiceOutput_ThEl( size_t *pointerStructureAddress );

#ifdef __cplusplus
}
#endif

/// DESCRIPTION
#endif /* _CINTERFACE_TH_EL_ */
