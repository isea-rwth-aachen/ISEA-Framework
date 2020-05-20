#include "armadillo_wrapper.h"
#ifdef _ARMADILLO_
#ifdef __EXCEPTIONS__
#include "../exceptions/error_proto.h"
#else
#include <cstdlib>
#endif
namespace boost
{
namespace numeric
{
namespace odeint
{


// In case of SpMat

// template<> template<>
// void default_operations::rel_error<double>::operator()<arma::SpMat<double>,arma::SpMat<double>,arma::SpMat<double> >(
// arma::SpMat<double> &t3 , const arma::SpMat<double> &t1 , const arma::SpMat<double> &t2 ) const
//{
//    for( size_t i = 0 ; i < t3.n_cols ; ++i )
//    {
//        t3(i,i) = t3(i,i) / (m_eps_abs + m_eps_rel * ( m_a_x * arma::abs(t1)(i,i) + m_a_dxdt * arma::abs(  t2  )(i,i)
//        ) );
//    }
//
//}
//
//}}}
}
}
}

namespace arma
{
const SpMat< double > solve( const SpMat< double > &A, const SpMat< double > &B )
{
    return arma::SpMat< double >( arma::solve( arma::Mat< double >( A ), arma::Mat< double >( B ) ) );
}

const SpMat< double > inv( const SpMat< double > &A )
{
    return arma::SpMat< double >( arma::inv( arma::Mat< double >( A ) ) );
}

SpMat< double > operator/( const SpMat< double > &A, const SpMat< double > &B )
{
    if ( ( A.n_cols != B.n_cols || A.n_rows != B.n_rows ) && A.n_cols != A.n_rows )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnequalSize", A.n_cols, B.n_cols );
    }

    SpMat< double > C( A.n_cols, A.n_rows );

    for ( size_t i = 0; i < A.n_cols; ++i )
    {
        for ( size_t j = 0; j < A.n_rows; ++j )
        {
            C( j, i ) = A( j, i ) / B( j, i );
        }
    }
    return C;
}

double ScalarMul( const SpMat< double > &rowMatrix, const SpMat< double > &colMatrix )
{
    double sum = 0;
    if ( colMatrix.n_rows > 0 )
    {
        SpMat< double > subMat = rowMatrix.cols( 0, colMatrix.n_rows - 1 );
        for ( arma::SpMat< double >::const_iterator it = subMat.begin(); it != subMat.end(); it++ )
            sum += *it * colMatrix( it.col(), 0 );
    }
    return sum;
}

double ScalarMul( const Mat< double > &rowMatrix, const Mat< double > &colMatrix )
{
    double sum = 0;
    for ( size_t i = 0; i < colMatrix.n_rows; ++i )
        sum += rowMatrix( 0, i ) * colMatrix( i, 0 );
    return sum;
}

double SumOfElements( const SpMat< double > &matrix ) { return sum( sum( matrix ) ); }

double SumOfElements( const Mat< double > &matrix ) { return sum( sum( matrix ) ); }
}

#endif
