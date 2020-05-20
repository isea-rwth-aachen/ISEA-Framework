/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : exportSystem.h
* Creation Date : 19-06-2017
* Last Modified : Di 20 Jun 2017 23:04:07 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _EXPORTSYSTEM_
#define _EXPORTSYSTEM_
#ifdef _SYMBOLIC_

#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/thread/thread.hpp>

#include "../electrical/twoport_withchild.h"
#include "../exceptions/error_proto.h"
#include "../misc/matrixInclude.h"
#include "../thermal/electrical_simulation.h"
#include "baseExport.h"

namespace symbolic
{
/// DESCRIPTION
template < typename matType, typename ExportClass, typename Parser, size_t DEPTH = 1 >
class ExportSystem : public BaseExport< matType >

{
    public:
    ExportSystem( simulation::ElectricalSimulation< myMatrixType, double, false > *sim, std::ostream *stream,
                  const long defaultTimeout = 5 );
    virtual ~ExportSystem(){};
    virtual void PrintVoltageMatrix();
    void PreParse( const ScalarUnit &mat, std::string &returnString ) const;

    protected:
    simulation::ElectricalSimulation< myMatrixType, double, false > *mSimulation = nullptr;
    std::ostream *mStream = nullptr;
    const long mDefaultTimeout = 5;

    virtual void WritePreamble(){};
    virtual void WriteEnding(){};
    virtual void LoopOverAllTwoPorts( const electrical::TwoPort< matType > * );

    std::vector< const electrical::TwoPort< matType > * > mElementList;

    void Parse( const ScalarUnit &mat, std::string &returnvalue ) const;
    std::string Export( std::string &result ) const;

    virtual void Output( const matType *mat ) const;
    virtual void Output( const matType &mat ) const;

    typename ExportClass::Generator_T mExportStruct;
};

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
ExportSystem< matType, ExportClass, Parser, DEPTH >::ExportSystem( simulation::ElectricalSimulation< myMatrixType, double, false > *sim,
                                                                   std::ostream *stream, const long defaultTimeout )
    : BaseExport< matType >( sim ? ( sim->mRootTwoPort ? sim->mRootTwoPort.get() : nullptr ) : nullptr )
    , mSimulation( sim )
    , mStream( stream )
    , mDefaultTimeout( defaultTimeout )
{
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void ExportSystem< matType, ExportClass, Parser, DEPTH >::PrintVoltageMatrix()
{
    if ( !mStream )
        return;

    LoopOverAllTwoPorts( this->mRootPort );


    ( *mStream ) << mExportStruct.PreTextOutput( "U" );
    for ( size_t i = 0; i < mElementList.size(); ++i )
    {
        Output( mElementList[i]->GetConstVoltage() );
        if ( i != mElementList.size() - 1 )
            ( *mStream ) << mExportStruct.newline << std::endl;
    }
    ( *mStream ) << mExportStruct.PostTextOutput( "U" );


    ( *mStream ) << mExportStruct.PreTextOutput( "I" );
    for ( size_t i = 0; i < mElementList.size(); ++i )
    {
        Output( mElementList[i]->GetCurrent() );

        if ( i != mElementList.size() - 1 )
            ( *mStream ) << mExportStruct.newline << std::endl;
    }
    ( *mStream ) << mExportStruct.PostTextOutput( "I" );


    auto matA = mSimulation->mEqSystem->GetA();
    matType matM( matA.rows(), matA.cols() );
    assert( matA.rows() == matA.cols() );
    const auto dglsize = this->mSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount();
    for ( size_t i = 0; i < dglsize; ++i )
    {
        matM.coeffRef( i, i ) = 1;
    }

    ( *mStream ) << mExportStruct.PreTextOutput( "M" );
    Output( matM );
    ( *mStream ) << mExportStruct.PostTextOutput( "M" );

    ( *mStream ) << mExportStruct.PreTextOutput( "A_{DAE}" );
    Output( matA );
    ( *mStream ) << mExportStruct.PostTextOutput( "A_{DAE}" );

    ( *mStream ) << mExportStruct.PreTextOutput( "C_{DAE}" );
    Output( mSimulation->mEqSystem->GetC() );
    ( *mStream ) << mExportStruct.PostTextOutput( "C_{DAE}" );
    matType A_ODE;
    if ( DEPTH > 1 )
    {
        // ODE-system
        A_ODE = mSimulation->mEqSystem->GetAODE();

        ( *mStream ) << mExportStruct.PreTextOutput( "A_{ODE}" );
        Output( A_ODE );
        ( *mStream ) << mExportStruct.PostTextOutput( "A_{ODE}" );

        ( *mStream ) << mExportStruct.PreTextOutput( "C_{ODE}" );
        auto C_ODE = mSimulation->mEqSystem->GetCODE();
        Output( mSimulation->mEqSystem->GetCODE() );
        ( *mStream ) << mExportStruct.PostTextOutput( "C_{ODE}" );


        ( *mStream ) << mExportStruct.PreTextOutput( "dxdt_{ODE}" );
        const auto size = this->mSimulation->mStateSystemGroup.GetStateCount();
        matType x( size, 1 );
        for ( size_t i = 0; i < size; ++i )
        {
            x.coeffRef( i, 0 ) = ScalarUnit( misc::StrCont( "State" ) + misc::StrCont( i + 1 ) );
        }

        matType dxdt = A_ODE * x;
        dxdt += C_ODE;
        Output( dxdt );
        ( *mStream ) << mExportStruct.PostTextOutput( "dxdt_{ODE}" );

        ( *mStream ) << mExportStruct.PreTextOutput( "x_{ODE}" );
        this->mSimulation->UpdateSystem();
        this->mSimulation->UpdateSystemValues();
        Output( this->mSimulation->mStateSystemGroup.mStateVector );
        ( *mStream ) << mExportStruct.PostTextOutput( "x_{ODE}" );
    }
    if ( DEPTH > 2 )
    {
        typedef Eigen::Matrix< symbolic::Symbolic< double >, Dynamic, Dynamic > full_t;
        /*
        full_t a_ode( A_ODE.n_rows, A_ODE.n_cols );
        for ( size_t i = 0; i < A_ODE.n_rows; ++i )
        {

            for ( size_t j = 0; j < A_ODE.n_cols; ++j )
            {
                a_ode( i, j ) = A_ODE( i, j );
            }
        }
        */

        // Eigen::SparseMatrix< symbolic::Symbolic< double >, Eigen::RowMajor >
        full_t a_ode = A_ODE;
        FullPivHouseholderQR< full_t > lu_decomp( a_ode );
        ( *mStream ) << mExportStruct.PreTextOutput( "A_{Q}_{min}" );
        std::cout << lu_decomp.matrixQ() << std::endl;
        //    Output( output );
        ( *mStream ) << mExportStruct.PostTextOutput( "A_{Q}_{min}" );
    }

    /*
    ( *mStream ) << mExportStruct.PreTextOutput( "A_{QR}_{min}" );
    std::cout << lu_decomp.matrixQR() << std::endl;
    //    Output( output );
    ( *mStream ) << mExportStruct.PostTextOutput( "A_{QR}_{min}" );

    */
    //    Eigen::SparseQR< matType, NaturalOrdering< int > > solver;
    //    solver.compute( A_ODE );

    //( *mStream ) << mExportStruct.PreTextOutput( "A_{Q}_{min}" );

    //    auto xas = solver.matrixQ().transpose();
    //    auto f = solver.matrixR();
    //    // matType I( f.rows(), f.rows() );
    //    std::cout << f << std::endl;
    //    //    std::cout << f( 0, 0 ) << std::endl;
    //    //    matType Q;
    //    //    Q = SparseQR< matType, COLAMDOrdering< int > >( A_ODE ).matrixQ();
    //    //    Output( Q )
    //    ( *mStream ) << mExportStruct.PreTextOutput( "A_{Q}_{min}" );

    /*
    ( *mStream ) << mExportStruct.PreTextOutput( "Q_{ODE}_S" );
    Eigen::SparseMatrix< symbolic::Symbolic< double >, Eigen::ColMajor > Q_S( A_ODE.n_rows, A_ODE.n_cols );

    Q_S.leftCols( Q_S.n_cols - 1 ) = C_ODE;
    Q_S.leftCols( Q_S.n_cols - 2 ) = A_ODE * C_ODE;

    for ( size_t i = 2; i < Q_S.n_rows; ++i )
    {
        Q_S.leftCols( Q_S.n_rows - i ) = A_ODE * Q_S.leftCols( Q_S.n_rows - ( i - 1 ) );
    }

    Output( Q_S );
    ( *mStream ) << mExportStruct.PostTextOutput( "Q_{ODE}_S" );
    */
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void ExportSystem< matType, ExportClass, Parser, DEPTH >::LoopOverAllTwoPorts( const electrical::TwoPort< matType > *tp )
{
    mElementList.push_back( tp );
    if ( tp->CanHaveChildren() )
    {
        const electrical::TwoPortWithChild< matType > *twoPortWithchild =
         static_cast< const electrical::TwoPortWithChild< matType > * >( tp );
        for ( size_t i = 0; i < twoPortWithchild->size(); ++i )
        {
            LoopOverAllTwoPorts( twoPortWithchild->at( i ) );
        }
    }
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void ExportSystem< matType, ExportClass, Parser, DEPTH >::Output( const matType *mat ) const
{
    if ( !mat )
    {
        ( *mStream ) << " ";
        return;
    }
    this->Output( *mat );
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void ExportSystem< matType, ExportClass, Parser, DEPTH >::Output( const matType &mat ) const
{
    std::vector< std::vector< std::string > > tmp_vector{static_cast< size_t >( mat.rows() ),
                                                         std::vector< std::string >( mat.cols() )};
    boost::thread_group group;
    for ( size_t i = 0; i < mat.rows(); ++i )
    {
        for ( size_t j = 0; j < mat.cols(); ++j )
        {
            if ( mat.coeff( i, j ).IsEmpty() )

            {
                tmp_vector[i][j] = " 0 ";
                continue;
            }
            // PreParse( mat( i, j ), tmp_vector[i][j] );
            group.add_thread( new boost::thread( boost::bind( &ExportSystem< matType, ExportClass, Parser, DEPTH >::PreParse,
                                                              this, mat.coeff( i, j ), boost::ref( tmp_vector[i][j] ) ) ) );
        }
    }
    group.join_all();

    for ( size_t i = 0; i < mat.rows(); ++i )
    {
        for ( size_t j = 0; j < mat.cols(); ++j )
        {
            if ( j != 0 )
            {
                ( *mStream ) << mExportStruct.spacer;
            }
            ( *this->mStream ) << tmp_vector[i][j];
        }
        if ( i != mat.rows() - 1 )
            ( *mStream ) << mExportStruct.newline;
    }
}


template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void ExportSystem< matType, ExportClass, Parser, DEPTH >::PreParse( const ScalarUnit &mat, std::string &returnString ) const
{
    using boost::spirit::ascii::space;

    std::string input = std::string( mat.GetString() );
    // Parse( mat, input );
    boost::thread t( boost::bind( &ExportSystem< matType, ExportClass, Parser, DEPTH >::Parse, this, mat, input ) );
    if ( t.try_join_for( boost::chrono::seconds( mDefaultTimeout ) ) )
    {
        t.join();    // join anyway
    }
    returnString = Export( input );
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
void ExportSystem< matType, ExportClass, Parser, DEPTH >::Parse( const ScalarUnit &mat, std::string &returnvalue ) const
{
    using boost::spirit::ascii::space;

    size_t i = 0;
    std::string result = "";
    std::string input = std::string( mat.GetString() );
    Parser g;
    while ( true )
    {
        result = "";
        std::string str = input;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse( iter, end, g, space, result );
        if ( !r || iter != end )
        {
            std::string error_string = "Error parsing string: \n";
            for ( auto b = iter; b != end; ++b )
            {
                error_string += *b;
            }
            ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough", error_string.c_str() );
        }
        if ( input == result || i > 1000 )
            break;
        else
        {
            input = result;
        }
        ++i;
    }
    returnvalue = result;
}

template < typename matType, typename ExportClass, typename Parser, size_t DEPTH >
std::string ExportSystem< matType, ExportClass, Parser, DEPTH >::Export( std::string &result ) const
{
    using boost::spirit::ascii::space;

    if ( mExportStruct.FallThrough() )
        return result;
    ExportClass g;
    std::string str = result;
    std::string output = "";
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = phrase_parse( iter, end, g, space, output );
    if ( !r || iter != end )
    {
        std::string error_string = "Error transforming string: \n";
        for ( auto b = iter; b != end; ++b )
        {
            error_string += *b;
        }
        ErrorFunction< std::logic_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough", error_string.c_str() );
    }
    result = output;
    return result;
}
}    // namespace symbolic

#endif /* _SYMBOLIC_ */
#endif /* _EXPORTSYSTEM_ */
