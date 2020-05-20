/*
 * differentialsystem.h
 *
 *  Created on: 07.12.2013
 *      Author: chris
 */

#ifndef DIFFERENTIALSYSTEM_H_
#define DIFFERENTIALSYSTEM_H_

#include "../misc/matrixInclude.h"
#include "stateSystemGroup.h"

namespace systm
{

///Base class for all solvers
template<typename T >
class SystemSolver
{
public:
    ///Constructor: dt is the initial timestep.
    SystemSolver(StateSystemGroup<T>* stateSystemGroup, double dt);

    virtual ~SystemSolver() { }

    ///Solve the equationsystem
    virtual double Solve() = 0;

protected:
    void ResetStateSystemGroup();

    T &mStateVector;
    StateSystemGroup<T>* mStateSystemGroup;
//    StateSystem<T> &mAlgStateSystem; //verschoben nach linearSystem da nur dort gebraucht
    double mTime;
    double &mDt;

};

template<typename T >
SystemSolver<T>::SystemSolver(StateSystemGroup<T>* stateSystemGroup, double dt)
    : mStateVector(stateSystemGroup->mStateVector),
      mStateSystemGroup(stateSystemGroup),
  //    mAlgStateSystem(stateSystemGroup->mAlgStateSystem), //verschoben nach linearSstem da nur dort gebraucht
      mTime(0),
      mDt(stateSystemGroup->mDt)
{
    mDt = dt;
}

template<typename T >
void SystemSolver<T>::ResetStateSystemGroup()
{
    mStateSystemGroup->ResetSystem(); //Because we use iterators for sparse, we need to clear the matrices! Dummy method in case of dense matrix.
}

} /* namespace systm */

#endif /* DIFFERENTIALSYSTEM_H_ */
