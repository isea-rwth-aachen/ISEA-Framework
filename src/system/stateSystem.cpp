/*
 * systemlinearequations.cpp
 *
 *  Created on: 03.11.2013
 *      Author: chris
 */

#include "stateSystem.h"

namespace systm
{

template class systm::StateSystem< myMatrixType >;


#ifdef _EIGEN_
template <>
void StateSystem< Eigen::SparseMatrix< double, Eigen::RowMajor > >::AddEquations( size_t equationNumber,
                                                                                  const Eigen::SparseMatrix< double, Eigen::RowMajor > &matrix )
{
    mMatrixA.middleRows( equationNumber, matrix.rows() ) = matrix.leftCols( mMatrixA.cols() );
    mVectorC.middleRows( equationNumber, matrix.rows() ) = matrix.rightCols( 1 );
}

template <>
void StateSystem< Eigen::SparseMatrix< double, Eigen::RowMajor > >::AddEquations(
 size_t equationNumber, const Eigen::SparseMatrix< double, Eigen::RowMajor > &matrix,
 const Eigen::SparseMatrix< double, Eigen::RowMajor > &vectorC )
{
    mMatrixA.middleRows( equationNumber, matrix.rows() ) = matrix;
    mVectorC.middleRows( equationNumber, matrix.rows() ) = vectorC;
}

template <>
void StateSystem< Eigen::SparseMatrix< double, Eigen::RowMajor > >::ResetSystem()
{
    mMatrixA.setZero();
    mVectorC.setZero();
}
#endif /* _EIGEN_ */




} /* namespace systm */
