/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : simulink_common.h
* Creation Date : 24-02-2015
* Last Modified : Do 03 Mär 2016 16:52:42 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SIMULINK_COMMON_
#define _SIMULINK_COMMON_

// for windows versions all simulink callbacks must be exported from the DLL by using __declspec(dllexport)
// __declspec is windows specific, so it is not added for linux versions
/*#ifdef _WIN32
#define simulink_callback __declspec( dllexport )
#else*/
#define simulink_callback
//#endif

#include <stdexcept>

extern const char *VERSION_NUMBER;


enum FcnParameters
{
    CONFIGSTR_A = 0,
    CONFIGSTR_B = 1,
    CONFIGSTR_C = 2,
    DISPLAY_VERSION = 3,
    LAST_PARAMETER
};

#include "cinterface.h"
#include "cinterface_Th.h"
#include "cinterface_ThEl.h"
#include "cinterface_common_functions.h"

#ifdef _MOCK_    // This has to be included out of the __cplusplus scope
#include "../../src/simulink_mock/simulink_mock.h"
#endif

#ifndef _MOCK_
#include "simstruc.h"
#endif
#include "macros.h"
#include <vector>


#ifdef __cplusplus
// this opens an extern "C" block, but only if being included for a C++ compile
//  if this is being included in a C compile, the extern "C" bits won't be seen
extern "C"
{
#endif

#ifdef _DS1006
#include "RTKernel.h"
    /// __cxa_pure_virtual is needed for pure virtual functions
    void __cxa_pure_virtual() {}

    mxArray *mxCreateNumericMatrix( mwSize m, mwSize n, mxClassID classid, mxComplexity flag )
    {
        int_T dim[2];
        dim[0] = m;
        dim[1] = n;
        return mxCreateNumericArray( 2, dim, classid, flag );
    }

#include <brtenv.h>
#include <malloc_info.h>
    extern heap_info_t *vcm_heap_info;
#endif


#include <cstdio>

#include <cstring>

    /// Common functions that are used by the ISEAFramework simulink functions

    void VersionInfo()
    {
#ifndef _DS1006
        printf( "%s\n", VERSION_NUMBER );
#endif
    }

    static char *GetConfigurationStringFromInput( SimStruct *S )
    {
        char *configStr = 0;
        try
        {
            if ( mxGetClassID( ssGetSFcnParam( S, CONFIGSTR_A ) ) == mxCHAR_CLASS )
            {
                // use mxArrayToString because mxGetString cannot handle multibyte characters
                std::vector< char * > configParts;
                std::vector< size_t > partLength;
                size_t totalConfigLength = 0;
                for ( size_t CONFIG_PART = CONFIGSTR_A; CONFIG_PART < CONFIGSTR_C + 1; ++CONFIG_PART )
                {
                    configParts.push_back( mxArrayToString( ssGetSFcnParam( S, CONFIG_PART ) ) );
                    partLength.push_back( strlen( configParts.back() ) );
                    totalConfigLength += partLength.back();
                }
                configStr = (char *)mxMalloc( totalConfigLength * sizeof( char ) + 1 );
                size_t offset = 0;
                for ( size_t i = 0; i < configParts.size(); ++i )
                {
                    strncpy( configStr + offset, configParts[i], partLength[i] + 1 );
                    offset += partLength[i];
                    mxFree( configParts[i] );
                    configParts[i] = nullptr;
                }
            }
            else if ( mxGetClassID( ssGetSFcnParam( S, CONFIGSTR_A ) ) == mxDOUBLE_CLASS )
            {

                int_T totalBufLen = 0;
                std::vector< int_T > config_length;
                size_t tmp_counter = 0;
                // Iterate over all 3 Configvariables
                for ( size_t CONFIG_PART = CONFIGSTR_A; CONFIG_PART < CONFIGSTR_C + 1; ++CONFIG_PART )
                {
                    config_length.push_back( mxGetM( ssGetSFcnParam( S, CONFIG_PART ) ) * mxGetN( ssGetSFcnParam( S, CONFIG_PART ) ) );
                    totalBufLen += config_length.at( tmp_counter );
                    ++tmp_counter;
                }

                tmp_counter = 0;
                // Allocate memory
                configStr = (char *)mxMalloc( totalBufLen * sizeof( char ) + 1 );
                memset( static_cast< void * >( configStr ), 0, totalBufLen + 1 );

                int offset = 0;
                // Iterate and append Configstrings
                for ( size_t CONFIG_PART = CONFIGSTR_A; CONFIG_PART < CONFIGSTR_C + 1; ++CONFIG_PART )
                {
                    CreateCharArrayFromDouble( (double *)mxGetData( ssGetSFcnParam( S, CONFIG_PART ) ),
                                               config_length.at( tmp_counter ), &configStr[offset] );
                    if ( offset + int( config_length.at( tmp_counter ) ) - 1 < 0 )
                    {
                        ErrorFunction( "Error in Parsing config strings. Calculated a negative offset" );
                    }

                    if ( configStr[offset + config_length.at( tmp_counter ) - 1] == '\0' )
                    {
                        --offset;
                    }
                    offset += config_length.at( tmp_counter );
                    ++tmp_counter;
                }
            }
            else
            {
                ErrorFunction(
                 "Couldn't understand config string format\n This needs to be mxCHAR_CLASS or mxDOUBLE_CLASS" );
            }
        }
        catch ( std::exception &e )
        {
            ErrorFunction( "%s", e.what() );
        }
        catch ( ... )
        {
            ErrorFunction( "%s", "Unknown Error" );
        }
        return configStr;
    }

#ifdef _DS1006

    void ToggleToRTQueSettings( int &old_value, rtk_ovc_check_type &old_check_type )
    {
        extern volatile rtk_p_task_control_block rtk_current_task;
        old_value = 0;
        old_check_type = ovc_queue;
        if ( rtk_current_task )
        {
            old_value = rtk_current_task->ovc_max;
            old_check_type = rtk_current_task->ovc_type;
            rtk_current_task->ovc_max = 100000;
            rtk_current_task->ovc_type = ovc_queue;
        }
    }

    void ResetToOldRTSettings( int old_value, rtk_ovc_check_type old_check_type )
    {
        extern volatile rtk_p_task_control_block rtk_current_task;
        if ( rtk_current_task )
        {
            rtk_current_task->ovc_counter = 0;
            rtk_current_task->ovc_max = old_value;
            rtk_current_task->ovc_type = old_check_type;
        }
    }
#endif

    void PrepareReset( SimStruct *S )
    {
#ifdef _DS1006
        int old_value = 0;
        rtk_ovc_check_type old_type;
        ToggleToRTQueSettings( old_value, old_type );
        ssGetRWork( S )[OVC_OLD_VALUE] = old_value;
        // ovc_count, ovc_fcn, ovc_queue
        if ( old_type == ovc_count )
        {
            ssGetRWork( S )[OVC_OLD_TYPE] = 0;
        }
        else if ( old_type == ovc_fcn )
        {
            ssGetRWork( S )[OVC_OLD_TYPE] = 1;
        }
        else if ( old_type == ovc_queue )
        {
            ssGetRWork( S )[OVC_OLD_TYPE] = 2;
        }
        else
        {
            ErrorFunction( "%s", "Unknown Overrun type" );
        }
#else
    UNUSED( S );
#endif
    }

    void FinalizeReset( SimStruct *S )
    {
#ifdef _DS1006
        rtk_ovc_check_type old_type;
        // ovc_count, ovc_fcn, ovc_queue
        if ( ssGetRWork( S )[OVC_OLD_TYPE] == 0 )
        {
            old_type = ovc_count;
        }
        else if ( ssGetRWork( S )[OVC_OLD_TYPE] == 1 )
        {
            old_type = ovc_fcn;
        }
        else if ( ssGetRWork( S )[OVC_OLD_TYPE] == 2 )
        {
            old_type = ovc_queue;
        }
        else
        {
            ErrorFunction( "%s", "Unknown Overrun type" );
        }
        ResetToOldRTSettings( static_cast< int >( ssGetRWork( S )[OVC_OLD_VALUE] ), old_type );
#else
    UNUSED( S );
#endif
    }


#ifdef __cplusplus
    // close the extern "C" block, but only if we started it in the first place
}
#endif /* __cplusplus */

#endif /* _SIMULINK_COMMON_ */
