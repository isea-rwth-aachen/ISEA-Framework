/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : vectorOperator.cpp
* Creation Date : 11-11-2012
* Last Modified : Sun 11 Nov 2012 01:37:28 AM CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "vectorOperator.h"

template std::vector<double> operator*( std::vector<double> vec,  const double scalar);
template std::vector< std::vector< double > > operator*( std::vector< std::vector< double > > vec,  const double scalar);

