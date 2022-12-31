/*
 * matrixInclude.h
 *
 *  Created on: 18.11.2013
 *      Author: chris
 */

#ifndef MATRIXINCLUDE_H_
#define MATRIXINCLUDE_H_

#ifdef _EIGEN_

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/LU>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

// using namespace Eigen;
typedef Eigen::SparseMatrix< double, Eigen::RowMajor > myMatrixType;

#include "../container/eigen_wrapper.h"
#elif _SYMBOLIC_
#include "symbolic.h"

#include <Eigen/Cholesky>
#include <Eigen/Eigenvalues>
#include <Eigen/LU>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
// using namespace Eigen;

typedef Eigen::SparseMatrix< symbolic::Symbolic< double >, Eigen::RowMajor > myMatrixType;
#include "../container/eigen_wrapper.h"
namespace symbolic
{

template < typename Output >
using SymbolicMatrix = Eigen::SparseMatrix< symbolic::Symbolic< Output >, Eigen::RowMajor >;
}
using namespace symbolic;
#else
#Error _EIGEN_ must be defined
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
Eigen::SparseMatrix< double, Eigen::RowMajor > &
CopyTopToMatrixLeft( Eigen::SparseMatrix< double, Eigen::RowMajor > &target,
                     const Eigen::SparseMatrix< double, Eigen::RowMajor > &source, size_t length );
#endif

#ifndef _SYMBOLIC_
typedef double ScalarUnit;
#else
typedef ::symbolic::Symbolic< OutType > ScalarUnit;
#endif


#endif /* MATRIXINCLUDE_H_ */
