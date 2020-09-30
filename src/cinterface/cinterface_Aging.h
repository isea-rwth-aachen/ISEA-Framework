#ifndef _CINTERFACE_AGING_
#define _CINTERFACE_AGING_

#include "cinterface_common_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /// Initializes a system and returns the adress of the DAE system as a size_t
    void InitializeSystem_Aging( const char *configStr, size_t *pointerStructureAddress, real_T *agingStepTime );

    /// Resets the system. Meaning the old system will be deleted and a new one initialized
    void ResetSystem_Aging( const char *configStr, size_t *pointerStructureAddress );

    void GetRealSizes_Aging( const char *configStr, const size_t *pointerStructureAddress, size_t *agingTwoportSize );

    void CollectData_Aging( const size_t *pointerStructureAddresst, const real_T *voltageVector, const real_T *currentVector,
                            const real_T *socVector, const real_T *temperatureVector, const real_T d );

    void ResetToPointInTime_Aging( const size_t *pointerStructureAddress, const real_T time );

    void Calculate_Aging( const size_t *pointerStructureAddress, real_T *relCapVector, real_T *relResVector,
                          real_T *socOffsetVector, real_T timestep );

    void Calculate_Aging_Variable_Steptime( const size_t *pointerStructureAddress, real_T *relCapVector, real_T *relResVector,
                                            real_T *socOffsetVector, real_T timestep, real_T scaleFactor );

    void DeleteSystem_Aging( size_t *pointerStructureAddress );

#ifdef __cplusplus
}
#endif

/// DESCRIPTION
#endif /* _CINTERFACE_AGING_ */
