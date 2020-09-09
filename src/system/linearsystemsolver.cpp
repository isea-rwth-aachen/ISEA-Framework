/*
 * linearsystemsolver.cpp
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#include "linearsystemsolver.h"

template class systm::LinearSystemSolver< myMatrixType >;

namespace systm
{

#ifdef _EIGEN_
template <>
double LinearSystemSolver< Eigen::SparseMatrix< double, Eigen::RowMajor > >::Solve()
{
    if ( linAlgSystem->GetEquationCount() )
    {
        Eigen::SparseMatrix< double, Eigen::RowMajor > realStat = this->mStateVector.topRows( this->mStateVector.rows() - 1 );
        linAlgSystem->SolveForStateVector( realStat );
        this->mStateVector.topRows( this->mStateVector.rows() - 1 ) = realStat;
    }
    this->mTime += this->mDt;

    this->ResetStateSystemGroup();

    return this->mTime;
}
#endif /* _EIGEN_ */
}    // namespace systm
