/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.h
* Creation Date : 23-05-2013
* Last Modified : Fr 06 Mär 2015 11:35:21 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CINTERFACE_TH_
#define _CINTERFACE_TH_

#include "cinterface_common_functions.h"



#ifdef __cplusplus
extern "C"
{
#endif
    /// Gets sizes for simulink vectors that are needed to initialize ths S-Function as needed by the equation system
    void GetRealSizes_Th(const char *configStr, size_t *numberOfCells, size_t *stateSize);

    void SetEmplacementPoints_Th(const char *configStr, real_T *emplacementPointsOutputVector);

    /// Initializes a system and returns the adress of the DAE system as a size_t
    void InitializeSystem_Th(const char* configStr, size_t *pointerStructureAddress, real_T * stateVector = 0);

    /// Resets the system. Meaning the old system will be deleted and a new one initialized
    void ResetSystem_Th(const char* configStr, size_t *pointerStructureAddress, real_T * stateVector = 0, real_T *cellTemperatureOutputVector = 0);

    /// Output of cell temperatures
    void CellTemperatureOutput_Th( const size_t *pointerStructureAddress, real_T *cellTemperatureOutputVector, double time = 0.0 );

    /// Triggers visualizer to write frame into file if visualizer time difference has been superceeded since the last frame
    void TriggerVisualizer_Th( const size_t *pointerStructureAddress, double time = 0.0, bool forceTriggering = false );

     /// This function sets the power dissipation from the S_function input
    void SetPowerDissipation_Th(const size_t * pointerStructureAddress, const real_T * const * powerDissipationInputVector);

    /// This resets the air temperature
    void SetAirTemperature_Th(const size_t * pointerStructureAddress, const real_T* airTemperature );

    /// RunSystem_ThEl returns dx/dt depending on the provided oldState
    /// This function evaluates the Kirchhoff voltage law and creates state space matrices for the differential equations of the network
    void RunSystem_Th(const size_t * pointerStructureAddress, real_T * stateVector, real_T* dxDt );

    void DeleteSystem_Th(size_t *pointerStructureAddress); //< Free the memory that is used by the DAE
#ifdef __cplusplus
}
#endif

/// DESCRIPTION
#endif // _CINTERFACE_TH_
