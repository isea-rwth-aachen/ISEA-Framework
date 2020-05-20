/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : mex2lookup.cpp
* Creation Date : 09-06-2013
* Last Modified : Sa 06 Jul 2013 10:27:48 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "mex2lookup.h"
#ifdef _MATLAB_
template ::std::vector<double> convert::matlab::Make1DLookupTable< double >(const mxArray *array);
template ::std::vector< ::std::vector<double> > convert::matlab::Make2DLookupTable<double>(const mxArray *array);
#endif
