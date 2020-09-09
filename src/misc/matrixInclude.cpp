/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : srcmatrixInclude.cpp
* Creation Date : 11-02-2015
* Last Modified : Mi 11 Feb 2015 18:54:24 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "matrixInclude.h"

#ifdef _EIGEN_
template <>
Eigen::SparseMatrix< double, Eigen::RowMajor > &
CopyTopToMatrixLeft( Eigen::SparseMatrix< double, Eigen::RowMajor > &target,
                     const Eigen::SparseMatrix< double, Eigen::RowMajor > &source, size_t length )
{
    target.topRows( length ) = source;
    return target;
}
#endif
