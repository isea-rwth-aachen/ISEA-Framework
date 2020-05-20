#ifdef _SYMBOLIC_
#ifndef _CREATE_SIMULINK_MODEL_FUNCTION_
#define _CREATE_SIMULINK_MODEL_FUNCTION_

#include "stddef.h"

void CreateSimulinkModelFunction( const char* xmlFileText, size_t modelType,
                                  const char* modelLocation = "BuildMatrixModel",
                                  const char* initialStateStruct = "initialState" );

#endif /* _CREATE_SIMULINK_MODEL_FUNCTION_ */
#endif /* _SYMBOLIC_ */
