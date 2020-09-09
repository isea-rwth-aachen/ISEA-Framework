/*
 * eigen_wrapper.cpp
 *
 *  Created on: 16.02.2014
 *      Author: chris
 */


#include "eigen_wrapper.h"

namespace Eigen
{

#ifdef _EIGEN_
double ScalarMul( const myMatrixType &rowMatrix, const myMatrixType &colMatrix )
{
    myMatrixType m = ( rowMatrix * colMatrix );
    return m.coeff( 0, 0 );
}
#endif /* _EIGEN_ */

#ifdef _SYMBOLIC_

namespace internal
{
template <>
struct default_digits10_impl< ::symbolic::Symbolic< OutType >, false, false >;
}

#endif /* _SYMBOLIC_ */

} /* namespace Eigen */
