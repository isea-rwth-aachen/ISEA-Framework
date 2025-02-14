/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : simulink_mock.cpp
* Creation Date : 31-07-2014
* Last Modified : Fr 28 Nov 2014 18:13:35 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

// STD
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ETC
#include "../cstring/strlcpy.h"
#include "../exceptions/error_function.h"
#include "simulink_exception.h"
#include "simulink_mock.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /// Return default mxCHAR_CLASS
    mxClassID mxGetClassID( const char * /* x */ ) { return mxCHAR_CLASS; }

    int ssSetNumSFcnParams( SimStruct *S, size_t numberOfExpectedParameters )
    {
        S->mNumberOfExpectedParameters = numberOfExpectedParameters;
        return numberOfExpectedParameters;
    }

    size_t ssGetNumSFcnParams( SimStruct *S ) { return S->mNumberOfExpectedParameters; }

    size_t ssGetSFcnParamsCount( SimStruct *S ) { return S->mArgumentCount; }

    bool ssSetNumInputPorts( SimStruct *S, int numberOfInput )
    {
        //    S->mNumberOfInput = numberOfInput;
        S->mInputVector.resize( numberOfInput );
        S->mInputPortRealSignalsPtrs.resize( numberOfInput );
        S->mDirectFeedBackOfInput.resize( numberOfInput );
        return true;
    }

    void ssSetInputPortWidth( SimStruct *S, size_t inputPort, size_t vectorSize )
    {
        if ( S->mInputVector.size() <= inputPort )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );

        S->mInputVector.at( inputPort ).resize( vectorSize );
    }

    void ssSetInputPortDirectFeedThrough( SimStruct *S, size_t inputPort, bool active )
    {
        if ( S->mDirectFeedBackOfInput.size() <= inputPort )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );

        S->mDirectFeedBackOfInput.at( inputPort ) = active;
    }

    bool ssSetNumOutputPorts( SimStruct *S, int numberOfOutputs )
    {
        S->mOutputVector.resize( numberOfOutputs );
        return true;
    }

    void ssSetNumSampleTimes( SimStruct * /* S */, int /* blobb */ )
    {
        // NOTHING
    }

    void ssSetNumRWork( SimStruct *S, size_t i ) { S->mRWork.resize( i, 0 ); }

    void ssSetNumIWork( SimStruct *S, size_t i ) { S->mIWork.resize( i, 0 ); }

    void ssSetNumPWork( SimStruct *S, size_t i ) { S->mPWork.resize( i, 0 ); }

    void ssSetNumMWork( SimStruct * /* S */, size_t /* i */ )
    {
        // NOTHING
    }

    void ssSetNumNonsampledZCs( SimStruct * /* S */, size_t /* i */ )
    {
        // NOTHING
    }

    void ssSetSimStateCompliance( SimStruct * /* S */, int )
    {
        // NOTHING
    }

    void ssSetOptions( SimStruct * /* S */, int )
    {
        // NOTHING
    }

    void ssSetOutputPortVectorDimension( SimStruct *S, size_t port, size_t size )
    {
        if ( S->mOutputVector.size() <= port )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );

        S->mOutputVector.at( port ).resize( size );
    }

    void ssSetOutputPortWidth( SimStruct *S, size_t port, size_t size )
    {
        if ( S->mOutputVector.size() <= port )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );

        S->mOutputVector.at( port ).resize( size );
    }

    void ssSetNumContStates( SimStruct *S, size_t size )
    {
        S->mContinousStates.resize( size );
        S->mDx.resize( size );
    }


    double ssSetSampleTime( SimStruct *S, size_t /* index */, double period )
    {
        S->mDt = period;
        return period;
    }

    // time_T ssSetOffsetTime(SimStruct *S, int_T st_index, time_T offset)
    /**
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] st_index Index of the sample time whose offset is to be set.
     * @param[in] Offset of the sample time specified by st_index.
    ///Set the offset time of a block
    //\return The time_T offset value specified by offset.
    */
    void ssSetOffsetTime( SimStruct * /* S */, size_t /* index */, double /* offset */ )
    {
        // NOTHING
    }

    /**
     * Specify that a referenced model containing this S-function can inherit its sample time from its parent model
     * @param[in] S SimStruct representing an S-Function block.
     */
    void ssSetModelReferenceSampleTimeDefaultInheritance( SimStruct * /* S */ )
    {
        // NOTHING
    }

    /**
     * Get a block's int_T work vector
     * @param[in] S SimStruct representing an S-Function block.
     */
    int_T *ssGetIWork( SimStruct *S ) { return &S->mIWork.at( 0 ); }

    /**
     * Get a block's pointer work vector
     * @param[in] S SimStruct representing an S-Function block.
     */
    void **ssGetPWork( SimStruct *S ) { return &S->mPWork.at( 0 ); }

    /**
     * Get a block's floating-point work vector
     * @param[in] S SimStruct representing an S-Function block.
     */
    double *ssGetRWork( SimStruct *S ) { return &S->mRWork.at( 0 ); }

    // time_T ssGetTaskTime(SimStruct *S, st_index)
    /**
     * Get the current time for the current task
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] i Index of the parameter to be returned.
     */
    double ssGetTaskTime( SimStruct *S, size_t /* i */ ) { return S->mCurrentTime; }

    /**
     * Get a parameter of an S-Function block
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] index Index of the parameter to be returned.
     */
    char *ssGetSFcnParam( SimStruct *S, int_T index )
    {
        if ( S->mArgumentCount < index )
        {
            return 0;
        }
        // simulink::ErrorImpl(__FUNCTION__, __LINE__, __FILE__, ("Argument is not defined" ) );
        return S->mArgv[index];
    }

    /**
     * Get a pointer to an output signal of type double (real_T)
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] port Index of an output port.
     */
    real_T *ssGetOutputPortRealSignal( SimStruct *S, size_t port )
    {
        if ( S->mOutputVector.size() <= port )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );
        return &S->mOutputVector.at( port ).at( 0 );
    }

    /**
     * Get the dimensions of the signal leaving an output port
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] port Index of an output port.
     */
    int_T *ssGetOutputPortDimensions( SimStruct *S, int_T port )
    {
        if ( S->mOutputVector.size() <= port )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );

        S->mOutputPortDimensions.reset( new size_t[2] );
        S->mOutputPortDimensions[0] = S->mOutputVector.at( port ).size();
        S->mOutputPortDimensions[1] = 1;

        return S->mOutputPortDimensions.get();
    }

    /**
     * Get pointers to signals of type double connected to an input port
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] port Index of the port whose signal is required.
     */
    InputRealPtrsType ssGetInputPortRealSignalPtrs( SimStruct *S, int_T port )
    {
        if ( S->mInputPortRealSignalsPtrs.size() <= port || S->mInputVector.size() <= port || S->mInputVector.at( port ).empty() )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );
        S->mInputPortRealSignalsPtrs.at( port ) = &S->mInputVector.at( port ).at( 0 );
        InputRealPtrsType pointer = &S->mInputPortRealSignalsPtrs.at( port );
        return pointer;
    }

    /**
     * Returns the number of continous states
     * @param[in] S SimStruct representing an S-Function block.
     * \return Number of continous states
     */
    int_T ssGetNumContStates( SimStruct *S ) { return S->mContinousStates.size(); }

    /**
     * Returns a pointer to the first continous states
     * @param[in] S SimStruct representing an S-Function block.
     * \return Pointer to the first element of continous.
     */
    real_T *ssGetContStates( SimStruct *S ) { return &S->mContinousStates.at( 0 ); }

    /**
     * Sets the number of the discrete states
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] size number of discrete states.
     */
    void ssSetNumDiscStates( SimStruct *S, size_t size ) { S->mDiscreteStates.resize( size ); }

    /**
     * Gets the number of the discrete states
     * @param[in] S SimStruct representing an S-Function block.
     * \return integer with the number of discrete states
     */
    int_T ssGetNumDiscStates( SimStruct *S ) { return S->mDiscreteStates.size(); }

    /**
     * Returns a pointer to the first discrete state
     * @param[in] S SimStruct representing an S-Function block.
     * \return Pointer to the first element of discrete states.
     */
    real_T *ssGetDiscStates( SimStruct *S ) { return &S->mDiscreteStates.at( 0 ); }

    /**
     * Returns a pointer to the first element in the array dx/dt
     * @param[in] S SimStruct representing an S-Function block.
     * \return Pointer to the first element in the array dx/dt.
     */
    real_T *ssGetdX( SimStruct *S ) { return &S->mDx.at( 0 ); }

    /**
     * Returns true if input at port is connected else false.
     * @param[in] S SimStruct representing an S-Function block.
     * @param[in] port Number of input port.
     * \return Status of input at position port is connected .
     */
    bool ssGetInputPortConnected( SimStruct *S, int_T port )
    {
        if ( S->mInputPortConnected.size() <= port )
            simulink::ErrorImpl( __FUNCTION__, __LINE__, __FILE__, ( "Port is not defined" ) );
        return S->mInputPortConnected.at( port );
    }

    /**
     * Specify the size of the block's mode vector
     * \param[in] SimStruct representing an S-Function block.
     * \param[in] nModes Size of the mode vector for the block represented by S. Valid values are 0, a positive integer, or DYNAMICALLY_SIZED.
     */
    int_T ssSetNumModes( SimStruct * /* S */, int_T /* nModes */ )
    {
        // NOTHING
        return int_T();
    }

    TimeStepEnum ssGetSimTimeStep( SimStruct * /* S */ ) { return MAJOR_TIME_STEP; }

    // MATLAB
    /**
     * This function wraps standard malloc
     * \param[in] n bytes to allocate
     * \return Returns a pointer address to the newly allocated array
     */
    void *mxMalloc( size_t n ) { return malloc( n ); }

    /*
     * Display error message with identifier and return to MATLAB prompt
     * In C, any arguments used in the message. Each argument must have a corresponding conversion specification.
     * \param[in] errorid String containing a MATLAB� message identifier. For information on creating identifiers, see Message Identifiers.
     * \param[in] errormsg String to display. In C, the string can include conversion specifications, used by the ANSI� C printf function.
     */
    void mexErrMsgIdAndTxt( const char *errorid, const char *errormsg, ... )
    {
        const size_t MAXSIZE = 500;
        size_t leftovers = MAXSIZE;
        char msg[MAXSIZE];
        memset( msg, '0', MAXSIZE );
        leftovers = MAXSIZE - strlcpy( msg, errorid, MAXSIZE );

        if ( leftovers > 1 )
        {
            msg[MAXSIZE - leftovers] = ';';
            leftovers -= 1;
        }

        if ( leftovers )
        {
            va_list args;
            va_start( args, errormsg );
            vsnprintf( &msg[MAXSIZE - leftovers], leftovers, errormsg, args );
            va_end( args );
        }
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SimulinkMockError", msg );
    }

    // THIS IS IN NO WAY THE BEHAVIOR OF mxGetN
    size_t mxGetN( const char *argv ) { return strlen( argv ); }

    // THIS IS IN NO WAY THE BEHAVIOR OF mxGetM
    size_t mxGetM( const char * /* argv */ ) { return 1; }

    char *mxGetData( char *data ) { return reinterpret_cast< char * >( data ); }

    /**
     * String array to C-style string
     * The current implementation only calls strlcat
     */
    int mxGetString( const char *pm, char *str, size_t strlen ) { return strlcpy( str, pm, strlen + 1 ); }

    /**
     * String array to C-style string
     * The returned string must be free'd after use
     */
    char *mxArrayToString( const char *array_ptr )
    {
        size_t length = strlen( array_ptr );
        char *newString = (char *)malloc( ( length + 1 ) * sizeof( char ) );
        strlcpy( newString, array_ptr, length + 1 );
        return newString;
    }

    /**
     * This function wraps standard free
     * \param[in] ptr to be freed
     */
    void mxFree( void *ptr ) { free( ptr ); }

#ifdef __cplusplus
}
#endif /* __cplusplus */
