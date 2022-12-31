/*
 * constantstepdglsystemsolver.cpp
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#ifndef _DS1006
#include "constantstepdglsystemsolver.h"

namespace systm
{

template class systm::ConstantStepDglSystemSolver< myMatrixType >;


#ifdef _EIGEN_
template <>
double ConstantStepDglSystemSolver< Eigen::SparseMatrix< double, Eigen::RowMajor > >::Solve()
{
    daeSystem->PrepareEquationSystem();

    size_t realStates = this->mStateVector.rows() - 1;

    Eigen::SparseMatrix< double, Eigen::RowMajor > dxdt( realStates, 1 );
    daeSystem->operator()( this->mStateVector.topRows( realStates ), dxdt, 0 );

    this->mStateVector.topRows( realStates ) += dxdt * this->mDt;
    this->mStateVector.coeffRef( realStates, 0 ) = 1;

    this->mTime += this->mDt;
    this->ResetStateSystemGroup();

    return this->mTime;
}
#endif /* _EIGEN_ */




} /* namespace systm */

#endif
