/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : twoport.cpp
* Creation Date : 13-03-2015
* Last Modified : So 08 Mai 2016 11:52:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "twoport.h"
namespace electrical
{
template class electrical::TwoPort< myMatrixType >;
#ifdef _EIGEN_
template <>
ScalarUnit ReturnFirstElement( Eigen::SparseMatrix< double, Eigen::RowMajor > &mat )
{
    return mat.coeff( 0, 0 );
}
#endif /* _EIGEN_ */
}    // namespace electrical
