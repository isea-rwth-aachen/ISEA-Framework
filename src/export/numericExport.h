/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : numericExport.h
* Creation Date : 06-05-2016
* Last Modified : Mo 19 Jun 2017 15:57:04 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _NUMERICEXPORT_
#define _NUMERICEXPORT_
#include "../electrical/twoport_withchild.h"
#include "../misc/matrixInclude.h"
#include "../thermal/electrical_simulation.h"
#include "baseExport.h"

/// DESCRIPTION
template < typename matType = myMatrixType >
class NumericExport : public BaseExport< matType >
{
    public:
    NumericExport( simulation::ElectricalSimulation< myMatrixType, ScalarUnit, false > *sim, std::ostream *stream );

    private:
    simulation::ElectricalSimulation< myMatrixType, ScalarUnit, false > *mSimulation = 0;
    std::ostream *mStream = 0;

    virtual void WritePreamble(){};
    virtual void WriteEnding(){};
    virtual void LoopOverAllTwoPorts( const electrical::TwoPort< matType > * ){};

    void Output( const matType *mat );
    void Output( const matType &mat );
    void Output( Eigen::Matrix< std::complex< double >, -1, 1 > &mat );

    template < typename ReturnType >
    void LoopOverAllTwoPorts( const electrical::TwoPort< matType > *tp, ReturnType ( electrical::TwoPort< matType >::*mf )() const );
    void PrintVoltageMatrix();
};

template < typename matType >
NumericExport< matType >::NumericExport( simulation::ElectricalSimulation< myMatrixType, ScalarUnit, false > *sim, std::ostream *stream )
    : BaseExport< matType >( sim ? ( sim->mRootTwoPort ? sim->mRootTwoPort.get() : 0 ) : 0 )
    , mSimulation( sim )
    , mStream( stream )
{
    PrintVoltageMatrix();
}

template < typename matType >
void NumericExport< matType >::PrintVoltageMatrix()
{
    if ( !mStream )
        return;
    ( *mStream ) << "U = [ ";
    LoopOverAllTwoPorts( this->mRootPort, &electrical::TwoPort< matType >::GetConstVoltage );
    ( *mStream ) << "];\n";

    ( *mStream ) << "I = [ ";
    LoopOverAllTwoPorts< const matType >( this->mRootPort, &electrical::TwoPort< matType >::GetCurrent );
    ( *mStream ) << "];\n";

    ( *mStream ) << "A_DAE = [ ";
    Output( mSimulation->mEqSystem->GetA() );
    ( *mStream ) << "];\n";

    ( *mStream ) << "C_DAE = [ ";
    Output( mSimulation->mEqSystem->GetC() );
    ( *mStream ) << "];\n";

    matType A_ODE = mSimulation->mEqSystem->GetAODE();
    ( *mStream ) << "A_ODE = [ ";
    Output( A_ODE );
    ( *mStream ) << "];\n";

    ( *mStream ) << "C_ODE = [ ";
    Output( mSimulation->mEqSystem->GetCODE() );
    ( *mStream ) << "];\n";

    ( *mStream ) << "Eigen_Value = [ ";

    /*
    Eigen::Matrix< ScalarUnit, Dynamic, Dynamic > a_ode = A_ODE;
    ComplexEigenSolver< Eigen::Matrix< ScalarUnit, Dynamic, Dynamic > > solver;
    solver.compute( a_ode );
    */
    /*
    MatrixXd a_ode = A_ODE;
    ComplexEigenSolver< MatrixXd > solver;
    solver.compute( a_ode );
    auto solution = solver.eigenvalues();
    Output( solution );
    */
    ( *mStream ) << "];\n";
}

template < typename matType >
template < typename ReturnType >
void NumericExport< matType >::LoopOverAllTwoPorts( const electrical::TwoPort< matType > *tp,
                                                    ReturnType ( electrical::TwoPort< matType >::*mf )() const )
{
    Output( ( ( *tp ).*mf )() );
    if ( tp->CanHaveChildren() )
    {
        const electrical::TwoPortWithChild< matType > *twoPortWithchild =
         static_cast< const electrical::TwoPortWithChild< matType > * >( tp );
        for ( size_t i = 0; i < twoPortWithchild->size(); ++i )
        {
            LoopOverAllTwoPorts( twoPortWithchild->at( i ), mf );
        }
    }
}

template < typename matType >
void NumericExport< matType >::Output( const matType *mat )
{
    if ( !mat )
    {
        ( *mStream ) << " ";
        return;
    }
    for ( size_t i = 0; i < mat->rows(); ++i )
    {
        for ( size_t j = 0; j < mat->cols(); ++j )
        {
            if ( j != 0 )
                ( *mStream ) << ", ";
            ( *mStream ) << ( *mat ).coeff( i, j );
        }
        ( *mStream ) << ";";
    }
}

template < typename matType >
void NumericExport< matType >::Output( const matType &mat )
{
    for ( size_t i = 0; i < mat.rows(); ++i )
    {
        for ( size_t j = 0; j < mat.cols(); ++j )
        {
            if ( j != 0 )
                ( *mStream ) << ", ";
            ( *mStream ) << mat.coeff( i, j );
        }
        ( *mStream ) << ";";
    }
}

template < typename matType >
void NumericExport< matType >::Output( Eigen::Matrix< std::complex< double >, -1, 1 > &mat )
{
    for ( size_t i = 0; i < size_t( mat.rows() ); ++i )
    {
        for ( size_t j = 0; j < size_t( mat.cols() ); ++j )
        {
            if ( j != 0 )
                ( *mStream ) << ", ";
            ( *mStream ) << " ( ";
            ( *mStream ) << std::real( mat( i, j ) );
            ( *mStream ) << " + ";
            ( *mStream ) << std::imag( mat( i, j ) );
            ( *mStream ) << "j ) ";
        }
        ( *mStream ) << ";";
    }
}

#endif /* _NUMERICEXPORT_ */
