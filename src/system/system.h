/*
 * system.h
 *
 *  Created on: 07.12.2013
 *      Author: chris
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#ifndef _DS1006

#include "linearsystemsolver.h"
#include "constantstepdglsystemsolver.h"
#include "variablestepdglsystemsolver.h"

namespace systm
{

///Class for solving any System with constant or variable stepsize
template<typename T >
class System
{
public:
    ///Constructor: dt is the initial timestep. If dt==0 then a variable step solver is used.
    System(StateSystemGroup< T >* stateSystemGroup, double dt = 0);

    virtual ~System() { }

    ///Solve the system and return the current simulated time
    virtual double Solve();

    ///Returns the current simulation time
    virtual double Time();

private:
    StateSystemGroup< T >* mStateSystemGroup;
    boost::scoped_ptr< SystemSolver<T> > mSystemSolver;
    double mTime;
};

template<typename T >
System<T>::System(StateSystemGroup< T >* stateSystemGroup, double dt)
    : mStateSystemGroup( stateSystemGroup ),
      mTime( 0 )
{
    if(stateSystemGroup->mDglStateSystem.GetEquationCount() > 0) //IsDGLSystem
    {
        if(dt == 0) //UseSolver
        {
#ifndef _DS1006
            mSystemSolver.reset( new VariableStepDglSystemSolver<T>(stateSystemGroup) );
#endif
        }
        else
        {
            mSystemSolver.reset( new ConstantStepDglSystemSolver<T>(stateSystemGroup, dt) );
        }
    }
    else
    {
        mSystemSolver.reset( new LinearSystemSolver<T>(stateSystemGroup, dt) );
    }
}

template<typename T >
double System<T>::Solve()
{
    mTime = mSystemSolver->Solve();
    return mTime;
}

template<typename T >
double System<T>::Time()
{
    return mTime;
}

} /* namespace systm */

#endif /* SYSTEM_H_ */
#endif
