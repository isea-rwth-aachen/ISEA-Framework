/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : mex2lookup.h
* Creation Date : 09-06-2013
* Last Modified : So 16 Jun 2013 00:43:45 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MEX2LOOKUP_
#define _MEX2LOOKUP_

#ifdef _MATLAB_
#ifndef _DS1006
#include "mex.h"
#else
#include "rt_matrx.h"
#endif /* _DS1006 */

#include <vector>

namespace convert
{
namespace matlab
{
/// This function transforms an mxArray to a lookup
template<template<class> class T, class C>
T< C > Make1DLookupTable( const mxArray *array)
{
    T< C > mat;
    const size_t rows( mxGetM(array) );
    const size_t cols( mxGetN(array) );

    for( size_t i = 0 ; i < rows * cols; ++i )
    {
        mat.push_back( mxGetPr(array)[i] );
    }
    return mat;
}

/// This function transforms an mxArray to a vector which can be used for a lookup
template< typename C>
::std::vector< C > Make1DLookupTable( const mxArray *array)
{
    ::std::vector< C > mat;
    const size_t rows( mxGetM(array) );
    const size_t cols( mxGetN(array) );

    for( size_t i = 0 ; i < rows * cols; ++i )
    {
        mat.push_back( mxGetPr(array)[i] );
    }
    return mat;
}

/// This function transforms an mxArray to a vector vector which can be used for a lookup
template<typename T>
::std::vector< ::std::vector<T> > Make2DLookupTable( const mxArray *array)
{
    ::std::vector< ::std::vector<T> > result;
    const size_t rows( mxGetM(array) );
    const size_t cols( mxGetN(array) );
    for( size_t i = 0 ; i < rows ; ++i )
    {
        result.push_back(::std::vector<T>());
        for( size_t j = 0 ; j < cols ; ++j )
        {
            result.at(i).push_back( (static_cast<const T*>(mxGetPr(array)))[i + cols * j ] );
        }
    }
    return result;
};

} /* matlab */

} /* convert */
#endif /* _MATLAB_ */
#endif /* _MEX2LOOKUP_ */
