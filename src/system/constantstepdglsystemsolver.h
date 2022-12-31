/*
 * constantstepdglsystemsolver.h
 *
 *  Created on: 06.01.2014
 *      Author: chris
 */

#ifndef CONSTANTSTEPDGLSYSTEMSOLVER_H_
#define CONSTANTSTEPDGLSYSTEMSOLVER_H_
#ifndef _DS1006

#include "dae_sys.h"
#include "systemSolver.h"
#include <boost/numeric/odeint/stepper/runge_kutta4.hpp>
#include <boost/scoped_ptr.hpp>

namespace systm
{

/// Solver with constant dt and euler integration
template < typename T >
class ConstantStepDglSystemSolver : public SystemSolver< T >
{
    public:
    ConstantStepDglSystemSolver( StateSystemGroup< T >* stateSystemGroup, double dt );

    virtual ~ConstantStepDglSystemSolver() {}

    virtual double Solve();

    private:
    T mDxDt;
    boost::scoped_ptr< systm::DifferentialAlgebraicSystem< T > > daeSystem;
};

/// Solver with constant dt and euler integration
template < typename T >
ConstantStepDglSystemSolver< T >::ConstantStepDglSystemSolver( StateSystemGroup< T >* stateSystemGroup, double dt )
    : SystemSolver< T >( stateSystemGroup, dt )
    , daeSystem( new systm::DifferentialAlgebraicSystem< T >( stateSystemGroup ) )
{
}

template < typename T >
double ConstantStepDglSystemSolver< T >::Solve()
{
    daeSystem->PrepareEquationSystem();
    daeSystem->operator()( this->mStateVector.submat( 0, 0, this->mStateVector.rows()() - 2, 0 ), mDxDt,
                           0 );    // TODO: FHU mStateVector topRows wrappen
    this->mStateVector.submat( 0, 0, this->mStateVector.rows()() - 2, 0 ) += mDxDt * this->mDt;
    this->mTime += this->mDt;

    this->ResetStateSystemGroup();

    return this->mTime;
}

#ifdef _EIGEN_
template <>
double ConstantStepDglSystemSolver< Eigen::SparseMatrix< double, Eigen::RowMajor > >::Solve();
#endif /* _EIGEN_ */


} /* namespace systm */

#endif /* CONSTANTSTEPDGLSYSTEMSOLVER_H_ */
#endif
