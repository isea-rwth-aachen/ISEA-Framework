/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.h
* Creation Date : 23-05-2013
* Last Modified : Fr 06 Mär 2015 11:35:54 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CINTERFACE_COMMON_FUNCTIONS_
#define _CINTERFACE_COMMON_FUNCTIONS_

#include <cstddef>
typedef double real_T;


#ifdef __cplusplus
extern "C"
{
#endif

    /// Compare two integers and throw an exception if argument1 is greater than argument2. Function is needed to control vector sizes
    void CheckVectorSizes( const size_t& smallInt, const size_t& bigInt, const char* myString );

    /// Reads from array element by element and writes into target
    void CreateCharArrayFromDouble( double* arry, size_t buflen, char* target );

    /// Compares size1 with size2 and returns the larger one
    size_t PickLargerSize( const size_t& size1, const size_t& size2 );

    /// Compares size1 with size2 and returns the larger one
    size_t PickSmallerSize( const size_t& size1, const size_t& size2 );

    /// Gets sizes for simulink vectors that are needed to initialize ths S-Function as given in the options-node of the xml file
    void GetDefinedSizes( const char* configStr, size_t* stateSize, size_t* observerSize = 0, size_t* numberOfCells = 0,
                          size_t* numberOfProbes = 0 );

    /// This function displays the version string
    void VersionInfo();


#ifdef __cplusplus
}
#endif

/// DESCRIPTION
#endif /* _CINTERFACE_ */
