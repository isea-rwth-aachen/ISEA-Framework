/*
 * matrixInclude.h
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#ifndef MATRIXINCLUDE_H_
#define MATRIXINCLUDE_H_

#ifdef _ARMADILLO_
#include <armadillo>
using namespace arma;
#include "../container/armadillo_wrapper.h"

#ifdef SPARSE_MATRIX_FORMAT
typedef arma::SpMat< double > myMatrixType;
#else
typedef arma::Mat< double > myMatrixType;
#endif
#elif _EIGEN_
#include "../container/armadillo_wrapper.h"
#include <armadillo>

#include <eigen3/Eigen/Cholesky>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Eigenvalues>
#include <eigen3/Eigen/LU>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/SparseLU>

using namespace Eigen;
typedef Eigen::SparseMatrix< double, RowMajor > myMatrixType;

#include "../container/eigen_wrapper.h"
#elif _SYMBOLIC_
#include "../stub/armadillo.h"
#include "symbolic.h"

#include <eigen3/Eigen/Cholesky>
#include <eigen3/Eigen/Eigenvalues>
#include <eigen3/Eigen/LU>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/SparseLU>
using namespace Eigen;

typedef Eigen::SparseMatrix< symbolic::Symbolic< double >, Eigen::RowMajor > myMatrixType;
#include "../container/eigen_wrapper.h"
namespace symbolic
{

template < typename Output >
using SymbolicMatrix = Eigen::SparseMatrix< symbolic::Symbolic< Output >, Eigen::RowMajor >;
}
using namespace symbolic;
#else
#Error Either _ARMADILLO_ or _EIGEN_ must be defined
#endif

template < typename MatrixType >
MatrixType &CopyTopToMatrixLeft( MatrixType &target, const MatrixType &source, size_t length )
{
    for ( size_t i = 0; i < length; ++i )
    {
        target( i, 0 ) = source( i, 0 );
    }
    return target;
}

#ifdef _EIGEN_
template <>
Eigen::SparseMatrix< double, RowMajor > &CopyTopToMatrixLeft( Eigen::SparseMatrix< double, RowMajor > &target,
                                                              const Eigen::SparseMatrix< double, RowMajor > &source, size_t length );
#endif

#ifndef _SYMBOLIC_
typedef double ScalarUnit;
#else
typedef ::symbolic::Symbolic< OutType > ScalarUnit;
#endif


#endif /* MATRIXINCLUDE_H_ */
