/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : generalizedSystem.h
* Creation Date : 03-09-2014
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _GENERALIZEDSYSTEM_
#define _GENERALIZEDSYSTEM_
#include "../misc/matrixInclude.h"
#include <vector>

namespace systm
{

template < typename MatrixT >
class GeneralizedSystem
{
    public:
    GeneralizedSystem(){};
    virtual ~GeneralizedSystem(){};

    virtual void PrepareEquationSystem(){};
    virtual void operator()( const MatrixT& /* x */, MatrixT& /* dxdt */,
                             const double /* t */ ){};    // kann vermutlich nicht vererbt werden, muss also jede Klasse
                                                          // selber implementieren!
    virtual void operator()( const std::vector< double >& /* x */, std::vector< double >& /* dxdt */, const double /* t */ ){};    // Um boost::odeint-solver mit vector<double> benutzen zu können
    virtual const char* GetName() const { return "GeneralizedSystem"; };
    virtual void SolveForStateVector( MatrixT& /* x */ ){};    // NICHT SCHOEN DA NUR IN LINALGSYSTEM GEBRAUCHT!!
    virtual const MatrixT GetA() const = 0;
    virtual const MatrixT GetC() const = 0;
    virtual const MatrixT GetAODE() const = 0;
    virtual const MatrixT GetCODE() const = 0;
    //    private:
};
}

#endif
