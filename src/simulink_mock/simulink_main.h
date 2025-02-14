/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : simulink_main.cpp
* Creation Date : 01-08-2014
* Last Modified : Mo 24 Nov 2014 17:54:01 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "simulink_mock.h"

extern void mdlStart( SimStruct *S );
extern void mdlInitializeSizes( SimStruct *S );
extern void mdlStart( SimStruct *S );
extern void mdlOutputs( SimStruct *S, int_T tid );
#ifdef MDL_DERIVATIVES
extern void mdlDerivatives( SimStruct *S );
#endif
extern void mdlTerminate( SimStruct *S );


static void SetInputPorts( SimStruct &S )
{
    S.mInputPortConnected.resize( S.mInputVector.size(), false );
    for ( size_t i = 0; i < S.mInputVector.size(); ++i )
    {
        S.mInputPortConnected.at( i ) = true;
    }
    S.mInputVector.at( 0 ).at( 0 ) = 10;
    S.mInputVector.at( 2 ).at( 0 ) = 0;
}

int main( int argc, char *argv[] )
{
    // Pass argv and argc to simulink
    const int simulinkArgumentCount = argc - 1;
    char **sumlinkArgumentVector = 0;
    if ( argc > 1 )
        sumlinkArgumentVector = &argv[1];

    SimStruct S;
    S.mArgumentCount = simulinkArgumentCount;
    S.mArgv = sumlinkArgumentVector;
    S.mDt = 1.000;
    S.mCurrentTime = 0.0;

    mdlInitializeSizes( &S );
    mdlInitializeSampleTimes( &S );

    mdlStart( &S );
    SetInputPorts( S );

    for ( size_t i = 0; i < 1000; ++i )
    {
        mdlOutputs( &S, 0 );
#ifdef MDL_DERIVATIVES
        mdlDerivatives( &S );
        for ( size_t j = 0; j < S.mContinousStates.size(); ++j )
            S.mContinousStates.at( j ) += S.mDx.at( j ) * S.mDt;
#endif
        S.mCurrentTime += S.mDt;
    }
    mdlTerminate( &S );


    return 0;
}
