#include "../LoadObjectValuesFunction.h"
#include "matrix.h"
#include "mex.h"
#include "string.h"

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    if ( nrhs == 0 )
    {
        mexPrintf( "%s\n",
                   "Usage: LoadObjectValues(configstring, modelLocation='BuildMatrixModel')\nLoads the object values "
                   "from the specified xml string to "
                   "the simulink model previously created with CreateSimulinkModel.\nconfigstring: The contents of an "
                   "XML file describing the system\nmodelLocation: System that the model was built in\n" );
        return;
    }

    const size_t stringLength = mxGetN( prhs[0] ) + 1;
    char *xmlContent = (char *)mxMalloc( sizeof( char ) * stringLength );
    mxGetString( prhs[0], xmlContent, stringLength );

    char *modelLocation;
    if ( nrhs > 1 )
    {
        modelLocation = (char *)mxMalloc( sizeof( char ) * ( mxGetN( prhs[1] ) + 1 ) );
        mxGetString( prhs[1], modelLocation, mxGetN( prhs[1] ) + 1 );
    }
    else
    {
        modelLocation = (char *)mxMalloc( sizeof( "BuildMatrixModel" ) );
        strcpy( modelLocation, "BuildMatrixModel" );
    }

    LoadObjectValuesFunction( xmlContent, modelLocation );
    mxFree( modelLocation );
    mxFree( xmlContent );
}
