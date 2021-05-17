#ifndef _CINTERFACE_TH_EL_AGING_
#define _CINTERFACE_TH_EL_AGING_

#include "cinterface_common_functions.h"


#ifdef __cplusplus
extern "C"
{
#endif

    /// Initializes a system and returns the adress of the DAE system as a size_t
    void InitializeSystem_ThEl_Aging( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector = 0 );

    /// Resets the system. Meaning the old system will be deleted and a new one initialized
    void ResetSystem_ThEl_Aging( const char *configStr, size_t *pointerStructureAddress, real_T *stateVector = 0,
                                 real_T *voltageOutputVec = 0, real_T *currentOutputVec = 0, real_T *powerOutputVec = 0,
                                 real_T *socOutputVec = 0, real_T *socSurfaceOutputVec = 0 );

    /// Late Binding for the output vectors addresses
    void SetOutputVectors_ThEl_Aging( const size_t *pointerStructureAddress, real_T *voltageOutputVec, real_T *currentOutputVec,
                                      real_T *powerOutputVec, real_T *socOutputVec, real_T *socSurfaceOutputVec );

    void GetRealSizes_ThEl_Aging( const char *configStr, const size_t *pointerStructureAddress, size_t *stateSize,
                                  size_t *numberOfCells, size_t *probeSize, size_t *agingTwoportSize, size_t *realObserverSize );

    /// Apply the aging input to the cells
    void SetAging_ThEl_Aging( const size_t *pointerStructureAddress, const real_T *capFactors, const real_T *resFactors,
                              const real_T *socOffsets, const real_T dt );

    /// Write aging data to the output vectors
    void OutputAgingData_ThEl_Aging( const size_t *pointerStructureAddress, real_T *capVector, real_T *ctpVector,
                                     real_T *socOffsetVector );

    /// set output vectors for temperatures and thermal probes
    void TriggerObserverAndTemperatureOutput_ThEl_Aging( const size_t *pointerStructureAddress, double t,
                                                         real_T *temperatureOutputVector, real_T *thermalProbeOutputVector );

#ifdef __cplusplus
}
#endif

/// DESCRIPTION
#endif /* _CINTERFACE_TH_EL_ */
