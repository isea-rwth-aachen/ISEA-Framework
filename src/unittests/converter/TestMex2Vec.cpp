/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMex2Vec.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 10:26:36 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestMex2Vec.h"

#ifdef _MATLAB_
#ifndef _DS1006
#include "mex.h"
#else
#include "rt_matrx.h"

#endif /* _DS1006 */
#include "../../matlab/mmatrix.h"
#include "../../convert/mex2lookup.h"
#endif /*_MATLAB_ */

void TestConvertMex2Vec::test2DMex2Vec()
{
#ifdef  _MATLAB_
    size_t n_rows, n_cols;
    n_rows = 3;
    n_cols = 3;
    matlab::MMatrix< > array( n_rows, n_cols);
    for( size_t i = 0 ; i < 9 ; ++i )
    {
        mxGetPr(array.GetMxArray())[i] = static_cast<double> (i+1.0);
    }
    std::vector< std::vector<double> > vec = ::convert::matlab::Make2DLookupTable<double>(array.GetMxArray());

    for( size_t i = 0 ; i < vec.size() ; ++i )
    {
        for( size_t j = 0 ; j < vec.at(i).size() ; ++j )
        {
            TS_ASSERT_DELTA( vec.at(j).at(i), 3*i + j + 1 , 0.00001 )
        }
    }
#endif /*_MATLAB_ */
}

void TestConvertMex2Vec::test2DMex1Vec()
{
#ifdef  _MATLAB_ 
    size_t n_rows, n_cols;
    n_rows = 9;
    n_cols = 1;
    matlab::MMatrix< > array( n_rows, n_cols);
    for( size_t i = 0 ; i < 9 ; ++i )
    {
        mxGetPr(array.GetMxArray())[i] = static_cast<double> (i+1.0);
    }

    ::std::vector< double > vec = ::convert::matlab::Make1DLookupTable< double >(array.GetMxArray());

    for( size_t i = 0 ; i < vec.size() ; ++i )
    {
        TS_ASSERT_DELTA( vec.at(i), i +  1 , 0.00001 )
    }
#endif /*_MATLAB_ */
}

