/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : simulink_mock.h
* Creation Date : 30-07-2014
* Last Modified : Fr 28 Nov 2014 18:07:24 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SIMULINK_MOCK_
#define _SIMULINK_MOCK_

#include <cstdio>
#include <cstring>
#include <vector>
////BOOST
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>

typedef double **InputRealPtrsType;

#ifdef __cplusplus
extern "C"
{
#endif

    /// This tries to mocks the functions of simulink
    static const int PORT_BASED_SAMPLE_TIMES = 0;
    static const int USE_DEFAULT_SIM_STATE = 0;
    static const int DYNAMICALLY_SIZED = -1;
    static const int INHERITED_SAMPLE_TIME = 0;
    static const int FIXED_IN_MINOR_STEP_OFFSET = 0;

    /// Typedefs
    typedef double real_T;
    typedef size_t int_T;
    typedef char mxChar;
    // typedef double** InputRealPtrsType;

    /// Structs
    struct SimStruct
    {
        size_t mNumberOfExpectedParameters;

        size_t mArgumentCount;
        char **mArgv;

        //    int mNumberOfInput;
        int mNumberOfOutput;

        double mDt;
        double mCurrentTime;

        std::vector< int > mNumberOfInputs;
        std::vector< bool > mDirectFeedBackOfInput;
        std::vector< bool > mInputPortConnected;
        std::vector< double > mRWork;
        std::vector< int_T > mIWork;
        std::vector< void * > mPWork;

        std::vector< std::vector< double > > mOutputVector;
        std::vector< std::vector< double > > mInputVector;

        std::vector< double > mContinousStates;
        std::vector< double > mDiscreteStates;

        std::vector< double > mDx;
        boost::scoped_array< size_t > mOutputPortDimensions;
        std::vector< double * > mInputPortRealSignalsPtrs;
    };

    enum TimeStepEnum
    {
        MAJOR_TIME_STEP = 0,
        MINOR_TIME_STEP = 1
    };

    /*
    struct mxArray
    {
        int N,M;
    };
    */

    /** Number of expected parameters */
    int ssSetNumSFcnParams( SimStruct *S, size_t numberOfExpectedParameters );
    size_t ssGetNumSFcnParams( SimStruct *S );
    size_t ssGetSFcnParamsCount( SimStruct *S );
    bool ssSetNumInputPorts( SimStruct *S, int numberOfInput );
    bool ssSetNumOutputPorts( SimStruct *S, int numberOfOutputs );
    void ssSetInputPortWidth( SimStruct *S, size_t inputPort, size_t vectorSize );
    void ssSetInputPortDirectFeedThrough( SimStruct *S, size_t inputPort, bool active );


    void ssSetNumRWork( SimStruct *S, size_t i );
    void ssSetNumIWork( SimStruct *S, size_t i );
    void ssSetNumPWork( SimStruct *S, size_t i );
    void ssSetNumMWork( SimStruct *S, size_t i );
    void ssSetNumNonsampledZCs( SimStruct *S, size_t i );

    void ssSetSimStateCompliance( SimStruct *S, int );
    void ssSetOptions( SimStruct *S, int );
    void ssSetOutputPortVectorDimension( SimStruct *S, size_t port, size_t size );
    void ssSetOutputPortWidth( SimStruct *S, size_t port, size_t size );

    double ssSetSampleTime( SimStruct *S, size_t index, double period );
    void ssSetModelReferenceSampleTimeDefaultInheritance( SimStruct *S );
    void ssSetOffsetTime( SimStruct *S, size_t index, double period );
    int_T *ssGetIWork( SimStruct *S );
    void **ssGetPWork( SimStruct *S );
    double *ssGetRWork( SimStruct *S );
    double ssGetTaskTime( SimStruct *S, size_t i );
    char *ssGetSFcnParam( SimStruct *S, int_T index );

    real_T *ssGetOutputPortRealSignal( SimStruct *S, size_t port );
    int_T *ssGetOutputPortDimensions( SimStruct *S, int_T port );

    void ssSetNumSampleTimes( SimStruct *S, int blobb );

    void ssSetNumContStates( SimStruct *S, size_t size );
    int_T ssGetNumContStates( SimStruct *S );
    real_T *ssGetContStates( SimStruct *S );

    void ssSetNumDiscStates( SimStruct *S, size_t size );
    int_T ssGetNumDiscStates( SimStruct *S );
    real_T *ssGetDiscStates( SimStruct *S );

    int_T ssSetNumModes( SimStruct *S, int_T nModes );

    real_T *ssGetdX( SimStruct *S );
    InputRealPtrsType ssGetInputPortRealSignalPtrs( SimStruct *S, int_T port );
    bool ssGetInputPortConnected( SimStruct *S, int_T port );

    TimeStepEnum ssGetSimTimeStep( SimStruct *S );


// MATLAB
#define mexPrintf printf
    void mexErrMsgIdAndTxt( const char *errorid, const char *errormsg, ... );

    /// mxClassID
    typedef enum
    {
        mxUNKNOWN_CLASS,
        mxCELL_CLASS,
        mxSTRUCT_CLASS,
        mxLOGICAL_CLASS,
        mxCHAR_CLASS,
        mxVOID_CLASS,
        mxDOUBLE_CLASS,
        mxSINGLE_CLASS,
        mxINT8_CLASS,
        mxUINT8_CLASS,
        mxINT16_CLASS,
        mxUINT16_CLASS,
        mxINT32_CLASS,
        mxUINT32_CLASS,
        mxINT64_CLASS,
        mxUINT64_CLASS,
        mxFUNCTION_CLASS
    } mxClassID;


    /// Return default mxCHAR_CLASS
    mxClassID mxGetClassID( const char *x );

    void *mxMalloc( size_t n );

    size_t mxGetN( const char *pm );
    size_t mxGetM( const char *pm );
    char *mxGetData( char *data );

    int mxGetString( const char *pm, char *str, size_t strlen );
    char *mxArrayToString( const char *array_ptr );
    void mxFree( void *ptr );
#ifdef __cplusplus
}

#endif /*  __cplusplus */


#endif /* _SIMULINK_MOCK_ */
