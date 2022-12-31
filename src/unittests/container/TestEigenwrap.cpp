/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestEigenwrap.cpp
* Creation Date : 31-01-2017
* Last Modified : Mo 19 Jun 2017 15:54:42 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestEigenwrap.h"
#include <Eigen/Sparse>

namespace CxxTest
{
ValueTraits< symbolic::Symbolic< symbolic::OutType > >::ValueTraits( const symbolic::Symbolic< symbolic::OutType > &t )
    : mOutputString( misc::StrCont( "{ " ) + t.GetString() + misc::StrCont( " }" ) )
{
}


const char *ValueTraits< symbolic::Symbolic< symbolic::OutType > >::asString( void ) const { return mOutputString; }
}    // namespace CxxTest

void TestEigenWrapper::TestEigenWrap()
{
    typedef symbolic::Symbolic< double > Scalar;
    typedef Eigen::SparseMatrix< Scalar, Eigen::RowMajor > Mat;
    Mat a( 3, 3 );
    a.coeffRef( 0, 0 ) = "A";
    a.coeffRef( 0, 1 ) = 1;
    a.coeffRef( 0, 2 ) = 1;
    a.coeffRef( 1, 0 ) = 2;
    a.coeffRef( 1, 1 ) = "B";
    a.coeffRef( 1, 2 ) = 2;
    a.coeffRef( 2, 0 ) = 3;
    a.coeffRef( 2, 1 ) = 3;
    a.coeffRef( 2, 2 ) = "C";

    Mat b = a + a;
    Mat c = a * a;

    TS_ASSERT_EQUALS( a.coeff( 0, 1 ), Scalar( "1.000000" ) );
    TS_ASSERT_EQUALS( a.coeff( 0, 2 ), Scalar( "1.000000" ) );

    /*
    TS_ASSERT_EQUALS( a( 1, 0 ), Scalar( "2.000000" ) );
    TS_ASSERT_EQUALS( a( 1, 2 ), Scalar( "2.000000" ) );

    TS_ASSERT_EQUALS( a( 2, 0 ), Scalar( "3.000000" ) );
    TS_ASSERT_EQUALS( a( 2, 1 ), Scalar( "3.000000" ) );


    TS_ASSERT_EQUALS( a( 0, 0 ), Scalar( "A" ) );
    TS_ASSERT_EQUALS( a( 1, 1 ), Scalar( "B" ) );
    TS_ASSERT_EQUALS( a( 2, 2 ), Scalar( "C" ) );

    TS_ASSERT_EQUALS( b( 0, 1 ), Scalar( "ADD(1.000000,1.000000)" ) );
    TS_ASSERT_EQUALS( b( 0, 2 ), Scalar( "ADD(1.000000,1.000000)" ) );

    TS_ASSERT_EQUALS( b( 1, 0 ), Scalar( "ADD(2.000000,2.000000)" ) );
    TS_ASSERT_EQUALS( b( 1, 2 ), Scalar( "ADD(2.000000,2.000000)" ) );

    TS_ASSERT_EQUALS( b( 2, 0 ), Scalar( "ADD(3.000000,3.000000)" ) );
    TS_ASSERT_EQUALS( b( 2, 1 ), Scalar( "ADD(3.000000,3.000000)" ) );


    TS_ASSERT_EQUALS( b( 0, 0 ), Scalar( "ADD(A,A)" ) );
    TS_ASSERT_EQUALS( b( 1, 1 ), Scalar( "ADD(B,B)" ) );
    TS_ASSERT_EQUALS( b( 2, 2 ), Scalar( "ADD(C,C)" ) );
    */
}

void TestEigenWrapper::TestEigenEmpty()
{
    /*
    typedef symbolic::Symbolic< double > Scalar;
    typedef Eigen::SparseMatrix< Scalar, Eigen::RowMajor > Mat;
    Mat a( 3, 3 );
    a( 2, 2 ) = "C";
    TS_ASSERT_EQUALS( a( 0, 0 ).IsEmpty(), true );
    TS_ASSERT_EQUALS( a( 0, 1 ).IsEmpty(), true );
    TS_ASSERT_EQUALS( a( 0, 2 ).IsEmpty(), true );
    TS_ASSERT_EQUALS( a( 1, 0 ).IsEmpty(), true );
    TS_ASSERT_EQUALS( a( 1, 1 ).IsEmpty(), true );
    TS_ASSERT_EQUALS( a( 1, 2 ).IsEmpty(), true );
    TS_ASSERT_EQUALS( a( 2, 0 ).IsEmpty(), true );
    TS_ASSERT_EQUALS( a( 2, 1 ).IsEmpty(), true );

    TS_ASSERT_EQUALS( a( 2, 2 ).IsEmpty(), false );
    */
}
