/*
 * eigen_wrapper.h
 *
 *  Created on: 16.02.2014
 *      Author: chris
 */

#ifndef EIGEN_WRAPPER_H_
#define EIGEN_WRAPPER_H_

#include "../exceptions/error_proto.h"
#include "../misc/matrixInclude.h"
namespace Eigen
{
#ifdef _EIGEN_

double ScalarMul( const myMatrixType &rowMatrix, const myMatrixType &colMatrix );
#endif /* _EIGEN_ */

#ifdef _SYMBOLIC_
#include "../misc/symbolic.h"
#include <eigen3/Eigen/Core>


template <>
struct NumTraits< symbolic::Symbolic< double > >
    : GenericNumTraits< symbolic::Symbolic< double > >    // permits to get the epsilon, dummy_precision, lowest,
                                                          // highest
                                                          // functions
{
    typedef symbolic::Symbolic< double > Real;
    typedef symbolic::Symbolic< double > NonInteger;
    typedef symbolic::Symbolic< double > Nested;
    enum
    {
        IsComplex = 0,
        IsInteger = 0,
        IsSigned = 1,
        RequireInitialization = 1,
        ReadCost = 1,
        AddCost = 3,
        MulCost = 3
    };
};

namespace internal
{
template <>
struct default_digits10_impl< symbolic::Symbolic< double >, false, false >
{
    static int run() { return 0; }
};
}    // namespace internal


#endif /* _SYMBOLIC_ */
}    // namespace Eigen

template < typename _Scalar, int _Options, typename _StorageIndex >
Eigen::SparseMatrix< _Scalar, _Options, _StorageIndex > solve( Eigen::SparseMatrix< _Scalar, _Options, _StorageIndex > matrixA,
                                                               Eigen::SparseMatrix< _Scalar, _Options, _StorageIndex > vectorC )
{
    Eigen::SparseMatrix< _Scalar, ColMajor, _StorageIndex > vecC = vectorC;
    Eigen::SparseLU< Eigen::SparseMatrix< _Scalar, Eigen::ColMajor, _StorageIndex > > solver;
    solver.analyzePattern( matrixA );
    solver.factorize( matrixA );

    ComputationInfo o;
    if ( Success != ( o = solver.info() ) )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             solver.lastErrorMessage().c_str() );


    SparseMatrix< _Scalar, ColMajor > ret = solver.solve( vecC );
    return ret;
};

#endif /* EIGEN_WRAPPER_H_ */
