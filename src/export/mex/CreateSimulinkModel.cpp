#include "../CreateSimulinkModelFunction.h"
#include "matrix.h"
#include "mex.h"
#include "string.h"

// mex -g CreateSimulinkModel.cpp -lmwblas -lmwlapack -v -L"../../buildWin32/Release" -lISEAFrame
// CONFIGSTR=fileread('testconfig.xml');
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    // No input variable
    if ( nrhs == 0 )
    {
        mexPrintf( "%s\n",
                   "Usage: CreateSimulinkModel(configstring, modelType, modelLocation='BuildMatrixModel', "
                   "initialStateStruct='initialState'\nCreates a simulink model from the specified config "
                   "string.\nconfigstring: The contents of an XML file describing the system\nmodelType: type of the "
                   "simulink model, 0 = CircuitModel (currently broken), 1 = MatrixModel\nmodelLocation: System that "
                   "the model will be built in\ninitialStateStruct: workspace variable that contains the initial state "
                   "of the model\n" );
        return;
    }

    const size_t stringLength = mxGetN( prhs[0] ) + 1;
    char *xmlFileText = (char *)mxMalloc( sizeof( char ) * stringLength );
    mxGetString( prhs[0], xmlFileText, stringLength );

    double *number = static_cast< double * >( mxGetData( prhs[1] ) );
    if ( *number == 0 )
        CreateSimulinkModelFunction( xmlFileText, 0 );
    else if ( *number == 1 )
    {
        char *modelLocation, *initialStateStruct;
        if ( nrhs > 2 )
        {
            modelLocation = (char *)mxMalloc( sizeof( char ) * ( mxGetN( prhs[2] ) + 1 ) );
            mxGetString( prhs[2], modelLocation, mxGetN( prhs[2] ) + 1 );
        }
        else
        {
            modelLocation = (char *)mxMalloc( sizeof( "BuildMatrixModel" ) );
            strcpy( modelLocation, "BuildMatrixModel" );
        }

        if ( nrhs > 3 )
        {
            initialStateStruct = (char *)mxMalloc( sizeof( char ) * ( mxGetN( prhs[3] ) + 1 ) );
            mxGetString( prhs[3], initialStateStruct, mxGetN( prhs[3] ) + 1 );
        }
        else
        {
            initialStateStruct = (char *)mxMalloc( sizeof( "MDL.battery.initialState" ) );
            strcpy( initialStateStruct, "MDL.battery.initialState" );
        }

        CreateSimulinkModelFunction( xmlFileText, 1, modelLocation, initialStateStruct );
        mxFree( modelLocation );
        mxFree( initialStateStruct );
    }

    mxFree( xmlFileText );
}
