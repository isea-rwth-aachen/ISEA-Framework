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


#ifdef _ARMADILLO_

ConstantStepDglSystemSolver< arma::Mat< double > >::ConstantStepDglSystemSolver( StateSystemGroup< arma::Mat< double > >* stateSystemGroup,
                                                                                 double dt )
    : SystemSolver< arma::Mat< double > >( stateSystemGroup, dt )
    , daeSystem( new systm::DifferentialAlgebraicSystem< arma::Mat< double > >( stateSystemGroup ) )
{
}

double ConstantStepDglSystemSolver< arma::Mat< double > >::Solve()
{
    daeSystem->PrepareEquationSystem();
    arma::Mat< double > states = this->mStateVector.submat( 0, 0, this->mStateVector.rows() - 2, 0 );
    mStepper.do_step( *daeSystem, states, 0, this->mDt );
    this->mStateVector.submat( 0, 0, this->mStateVector.rows() - 2, 0 ) = states;

    this->mTime += this->mDt;

    this->ResetStateSystemGroup();

    return this->mTime;
}

#endif /* _ARMADILLO_ */

} /* namespace systm */

#endif
