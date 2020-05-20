#include "../LoadInitialStateFunction.h"
#include "matrix.h"
#include "mex.h"
#include "string.h"

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    if ( nrhs == 0 )
    {
        mexPrintf( "%s\n",
                   "Usage: LoadInitialState(configstring, initialStateStruct='MDL.battery.initialState')\nLoads the "
                   "initial state "
                   "from the specified xml string to "
                   "the workspace for a simulink model previously created with CreateSimulinkModel.\nconfigstring: The "
                   "contents of an XML file describing the system\ninitialStateStruct: workspace variable that "
                   "contains the initial state of the model\n" );
        return;
    }

    const size_t stringLength = mxGetN( prhs[0] ) + 1;
    char *xmlContent = (char *)mxMalloc( sizeof( char ) * stringLength );
    mxGetString( prhs[0], xmlContent, stringLength );

    char *initialStateStruct;
    if ( nrhs > 1 )
    {
        initialStateStruct = (char *)mxMalloc( sizeof( char ) * ( mxGetN( prhs[1] ) + 1 ) );
        mxGetString( prhs[1], initialStateStruct, mxGetN( prhs[1] ) + 1 );
    }
    else
    {
        initialStateStruct = (char *)mxMalloc( sizeof( "MDL.battery.initialState" ) );
        strcpy( initialStateStruct, "MDL.battery.initialState" );
    }

    LoadInitialStateFunction( xmlContent, initialStateStruct );
    mxFree( initialStateStruct );
    mxFree( xmlContent );
}
