#include "TestSymbolic.h"
#include "../../misc/matrixInclude.h"
#include "../thermal/exception_tester.h"
#include <cstring>
#include <iostream>

static const double sDelta = 0.000001;

#ifdef _SYMBOLIC_
using namespace symbolic;
using misc::StrCont;
#include "../../misc/symbolic.h"
#endif


void TestSymbolic::TestSymbolicUnit()
{
#ifdef _SYMBOLIC_
    {
        symbolic::Symbolic< OutType > symbol1( "var" );
        symbolic::Symbolic< OutType > symbol2( "var" );
        symbolic::Symbolic< OutType > symbol3( "const" );

        TS_ASSERT( symbol1 == symbol2 );
        TS_ASSERT( !( symbol1 == symbol3 ) );
    }

    symbolic::Symbolic< OutType > empty;
    TS_ASSERT_EQUALS( strcmp( empty.GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT( empty.IsEmpty() );

    symbolic::Symbolic< OutType > one( 1.0 );
    symbolic::Symbolic< OutType > intOne( 1 );
    symbolic::Symbolic< OutType > notOne( 1.001 );
    TS_ASSERT( !one.IsEmpty() );
    TS_ASSERT( one.IsOne() );
    TS_ASSERT( intOne.IsOne() );
    TS_ASSERT( !notOne.IsOne() )

    symbolic::Symbolic< OutType > symbolicStrCont( StrCont( "AString" ) );
    TS_ASSERT_EQUALS( strcmp( symbolicStrCont.GetString(), StrCont( "AString" ) ), 0 );
    TS_ASSERT( !symbolicStrCont.IsEmpty() );

    symbolic::Symbolic< OutType > symbolicStr( "AnotherString" );
    TS_ASSERT_EQUALS( strcmp( symbolicStr.GetString(), StrCont( "AnotherString" ) ), 0 );
    TS_ASSERT( !symbolicStr.IsEmpty() );

    symbolic::Symbolic< OutType > symbolicSizet( 43 );
    TS_ASSERT_EQUALS( strcmp( symbolicSizet.GetString(), StrCont( "43" ) ), 0 );
    TS_ASSERT( !symbolicSizet.IsEmpty() );

    symbolic::Symbolic< OutType > symbolicInt( -5 );
    TS_ASSERT_EQUALS( strcmp( symbolicInt.GetString(), StrCont( "-5" ) ), 0 );
    TS_ASSERT( !symbolicInt.IsEmpty() );

    symbolic::Symbolic< OutType > symbolicDouble1( 23.5 );
    TS_ASSERT_EQUALS( strcmp( symbolicDouble1.GetString(), StrCont( "23.500000" ) ), 0 );
    TS_ASSERT( !symbolicDouble1.IsEmpty() );

    symbolic::Symbolic< OutType > symbolicDouble2( -1.0 );
    TS_ASSERT_EQUALS( strcmp( symbolicDouble2.GetString(), StrCont( "-1.000000" ) ), 0 );
    TS_ASSERT( !symbolicDouble2.IsEmpty() );

    symbolic::Symbolic< OutType > symbolicIntegerEmpty( 0 );
    symbolic::Symbolic< OutType > symbolicDoubleEmpty( 0.0 );
    TS_ASSERT_EQUALS( strcmp( symbolicIntegerEmpty.GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( symbolicDoubleEmpty.GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT( symbolicIntegerEmpty.IsEmpty() );
    TS_ASSERT( symbolicDoubleEmpty.IsEmpty() );


    symbolic::Symbolic< OutType > left( "left" );
    symbolic::Symbolic< OutType > right( "right" );
    symbolic::Symbolic< OutType > var( "var" );
    symbolic::Symbolic< OutType > tmp;

    TS_ASSERT_EQUALS( strcmp( ( -var ).GetString(), StrCont( "NEG(var)" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( -empty ).GetString(), StrCont( "" ) ), 0 );
    TS_ASSERT( ( -empty ).IsEmpty() );

    TS_ASSERT_EQUALS( strcmp( ( left + right ).GetString(), StrCont( "ADD(left,right)" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( var + empty ).GetString(), StrCont( "var" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( empty + var ).GetString(), StrCont( "var" ) ), 0 );

    TS_ASSERT_EQUALS( strcmp( ( left - right ).GetString(), StrCont( "SUB(left,right)" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( var - empty ).GetString(), StrCont( "var" ) ), 0 );
    TS_ASSERT_EQUALS( strcmp( ( empty - var ).GetString(), StrCont( "NEG(var)" ) ), 0 );

    TS_ASSERT_EQUALS( strcmp( ( left * right ).GetString(), StrCont( "MUL(left,right)" ) ), 0 );
    TS_ASSERT( ( var * empty ).IsEmpty() );
    TS_ASSERT( ( empty * var ).IsEmpty() );

    TS_ASSERT_EQUALS( strcmp( ( left / right ).GetString(), StrCont( "DIV(left,right)" ) ), 0 );
    TS_ASSERT_THROWS_EQUALS( var / empty, const std::runtime_error &e,
                             std::string( e.what() ).find( "Empty symbolic instance is considered a zero, and division by zero is not valid" ) !=
                              std::string::npos,
                             1 );
    TS_ASSERT( ( empty / var ).IsEmpty() );

    tmp = left;
    tmp += right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "ADD(left,right)" ) ), 0 );
    tmp = var;
    tmp += empty;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
    tmp = empty;
    tmp += var;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );

    tmp = left;
    tmp -= right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "SUB(left,right)" ) ), 0 );
    tmp = var;
    tmp -= empty;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
    tmp = empty;
    tmp -= var;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "NEG(var)" ) ), 0 );

    tmp = left;
    tmp *= right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "MUL(left,right)" ) ), 0 );
    tmp = var;
    tmp *= empty;
    TS_ASSERT( tmp.IsEmpty() );
    tmp = empty;
    tmp *= var;
    TS_ASSERT( tmp.IsEmpty() );
    tmp = one;
    tmp *= var;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
    tmp = var;
    tmp *= one;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );

    tmp = left;
    tmp /= right;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "DIV(left,right)" ) ), 0 );
    tmp = var;
    TS_ASSERT_THROWS_EQUALS( tmp /= empty, const std::runtime_error &e,
                             std::string( e.what() ).find( "Empty symbolic instance is considered a zero, and division by zero is not valid" ) !=
                              std::string::npos,
                             1 );
    tmp = empty;
    tmp /= var;
    TS_ASSERT( tmp.IsEmpty() );
    tmp = var;
    tmp /= one;
    TS_ASSERT_EQUALS( strcmp( tmp.GetString(), StrCont( "var" ) ), 0 );
#endif
}

void TestSymbolic::TestSymbolicMatrix()
{
#ifdef _SYMBOLIC_
    SymbolicMatrix< OutType > empty;
    TS_ASSERT_EQUALS( empty.rows(), 0 );
    TS_ASSERT_EQUALS( empty.cols(), 0 );
    TS_ASSERT_EQUALS( empty.size(), 0 );

    SymbolicMatrix< OutType > test( 2, 3 );
    TS_ASSERT_EQUALS( test.rows(), 2 );
    TS_ASSERT_EQUALS( test.cols(), 3 );
    TS_ASSERT_EQUALS( test.size(), 6 );
    TS_ASSERT( test.coeff( 0, 0 ).IsEmpty() );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 0, 2 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 0 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 2 ).IsEmpty() );

    test.coeffRef( 0, 0 ) = symbolic::Symbolic< OutType >( "VarL1" );
    test.coeffRef( 1, 1 ) = symbolic::Symbolic< OutType >( "VarL2" );
    test.coeffRef( 0, 2 ) = symbolic::Symbolic< OutType >( "VarR1" );
    test.coeffRef( 1, 2 ) = symbolic::Symbolic< OutType >( "VarR2" );
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "VarL1" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "VarL2" ) );
    TS_ASSERT_EQUALS( test.coeff( 0, 2 ), symbolic::Symbolic< OutType >( "VarR1" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 2 ), symbolic::Symbolic< OutType >( "VarR2" ) );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 0 ).IsEmpty() );
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "VarL1" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "VarL2" ) );
    TS_ASSERT_EQUALS( test.coeff( 0, 2 ), symbolic::Symbolic< OutType >( "VarR1" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 2 ), symbolic::Symbolic< OutType >( "VarR2" ) );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 0 ).IsEmpty() );

    SymbolicMatrix< OutType > left, right;
    left = test.leftCols( 2 );
    right = test.rightCols( 1 );
    TS_ASSERT_EQUALS( left.rows(), 2 );
    TS_ASSERT_EQUALS( left.cols(), 2 );
    TS_ASSERT_EQUALS( left.size(), 4 );
    TS_ASSERT_EQUALS( right.rows(), 2 );
    TS_ASSERT_EQUALS( right.cols(), 1 );
    TS_ASSERT_EQUALS( right.size(), 2 );
    TS_ASSERT_EQUALS( left.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "VarL1" ) );
    TS_ASSERT_EQUALS( left.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "VarL2" ) );
    TS_ASSERT_EQUALS( right.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "VarR1" ) );
    TS_ASSERT_EQUALS( right.coeff( 1, 0 ), symbolic::Symbolic< OutType >( "VarR2" ) );
    TS_ASSERT( left.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( left.coeff( 1, 0 ).IsEmpty() );

    test.resize( 1, 5 );
    test.setZero();
    TS_ASSERT_EQUALS( test.rows(), 1 );
    TS_ASSERT_EQUALS( test.cols(), 5 );
    TS_ASSERT_EQUALS( test.size(), 5 );
    TS_ASSERT( test.coeff( 0, 0 ).IsEmpty() );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 0, 2 ).IsEmpty() );
    TS_ASSERT( test.coeff( 0, 3 ).IsEmpty() );
    TS_ASSERT( test.coeff( 0, 4 ).IsEmpty() );


    test = -left;
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "NEG(VarL1)" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "NEG(VarL2)" ) );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 0 ).IsEmpty() );

    right = test;
    right.coeffRef( 1, 0 ) = symbolic::Symbolic< OutType >( "another_var" );
    test = left + right;
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "ADD(VarL1,NEG(VarL1))" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "ADD(VarL2,NEG(VarL2))" ) );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT_EQUALS( test.coeff( 1, 0 ), symbolic::Symbolic< OutType >( "another_var" ) );

    test = left - test;
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "SUB(VarL1,ADD(VarL1,NEG(VarL1)))" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "SUB(VarL2,ADD(VarL2,NEG(VarL2)))" ) );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT_EQUALS( test.coeff( 1, 0 ), symbolic::Symbolic< OutType >( "NEG(another_var)" ) );

    SymbolicMatrix< OutType > leftFactor( 2, 3 );
    SymbolicMatrix< OutType > rightFactor( 3, 2 );
    leftFactor.coeffRef( 0, 0 ) = symbolic::Symbolic< OutType >( "L00" );
    leftFactor.coeffRef( 0, 2 ) = symbolic::Symbolic< OutType >( "L02" );
    leftFactor.coeffRef( 1, 1 ) = symbolic::Symbolic< OutType >( "L11" );
    leftFactor.coeffRef( 1, 2 ) = symbolic::Symbolic< OutType >( "L12" );
    rightFactor.coeffRef( 0, 0 ) = symbolic::Symbolic< OutType >( "R00" );
    rightFactor.coeffRef( 0, 1 ) = symbolic::Symbolic< OutType >( "R01" );
    rightFactor.coeffRef( 1, 0 ) = symbolic::Symbolic< OutType >( "R10" );
    rightFactor.coeffRef( 1, 1 ) = symbolic::Symbolic< OutType >( "R11" );
    rightFactor.coeffRef( 2, 0 ) = symbolic::Symbolic< OutType >( "R20" );
    rightFactor.coeffRef( 2, 1 ) = symbolic::Symbolic< OutType >( "R21" );
    test = leftFactor * rightFactor;
    TS_ASSERT_EQUALS( test.rows(), 2 );
    TS_ASSERT_EQUALS( test.cols(), 2 );
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "ADD(MUL(R00,L00),MUL(R20,L02))" ) );
    TS_ASSERT_EQUALS( test.coeff( 0, 1 ), symbolic::Symbolic< OutType >( "ADD(MUL(R01,L00),MUL(R21,L02))" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 0 ), symbolic::Symbolic< OutType >( "ADD(MUL(R10,L11),MUL(R20,L12))" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "ADD(MUL(R11,L11),MUL(R21,L12))" ) );

    test = left * symbolic::Symbolic< OutType >( "Coeff" );
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "MUL(VarL1,Coeff)" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "MUL(VarL2,Coeff)" ) );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 0 ).IsEmpty() );

    test = left / symbolic::Symbolic< OutType >( "Coeff" );
    TS_ASSERT_EQUALS( test.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "DIV(VarL1,Coeff)" ) );
    TS_ASSERT_EQUALS( test.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "DIV(VarL2,Coeff)" ) );
    TS_ASSERT( test.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( test.coeff( 1, 0 ).IsEmpty() );

    right = -left;
    test = left;
    test += right;
    // TS_ASSERT_EQUALS( test, left + right );

    right = -left;
    test = left;
    test -= right;

    // TS_ASSERT_EQUALS( test, left - right );

    SymbolicMatrix< OutType > subMat = leftFactor.block( 0, 0, 2, 2 );
    TS_ASSERT_EQUALS( subMat.rows(), 2 );
    TS_ASSERT_EQUALS( subMat.cols(), 2 );
    TS_ASSERT_EQUALS( subMat.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "L00" ) );
    TS_ASSERT_EQUALS( subMat.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "L11" ) );

    TS_ASSERT( subMat.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( subMat.coeff( 1, 0 ).IsEmpty() );

    subMat = leftFactor.block( 0, 0, 2, 3 );
    TS_ASSERT_EQUALS( subMat.rows(), 2 );
    TS_ASSERT_EQUALS( subMat.cols(), 3 );
    TS_ASSERT_EQUALS( subMat.coeff( 0, 0 ), symbolic::Symbolic< OutType >( "L00" ) );
    TS_ASSERT( subMat.coeff( 0, 1 ).IsEmpty() );
    TS_ASSERT( subMat.coeff( 1, 0 ).IsEmpty() );


    TS_ASSERT_EQUALS( subMat.coeff( 1, 1 ), symbolic::Symbolic< OutType >( "L11" ) );
    TS_ASSERT_EQUALS( subMat.coeff( 1, 2 ), symbolic::Symbolic< OutType >( "L12" ) );
#endif
}

namespace Eigen
{
template < typename _Scalar, int _Options, typename _StorageIndex >
SparseMatrix< _Scalar, _Options, _StorageIndex >
solve123( SparseMatrix< _Scalar, _Options, _StorageIndex > &matrixA, SparseMatrix< _Scalar, _Options, _StorageIndex > &vectorC )
{
    SparseLU< SparseMatrix< _Scalar, Eigen::ColMajor, _StorageIndex > > solver;
    SparseMatrix< _Scalar, Eigen::ColMajor, _StorageIndex > tmpMatrix = matrixA;
    SparseMatrix< _Scalar, Eigen::ColMajor, _StorageIndex > tmpVec = vectorC;
    solver.analyzePattern( tmpMatrix );
    solver.factorize( tmpMatrix );
    SparseMatrix< _Scalar, Eigen::ColMajor, _StorageIndex > res = solver.solve( tmpVec );
    return res;
};
}    // namespace Eigen


void TestSymbolic::TestSolve()
{
#ifdef _SYMBOLIC_

    SymbolicMatrix< OutType > matA( 3, 3 );
    matA.coeffRef( 0, 0 ) = symbolic::Symbolic< OutType >( "a" );
    matA.coeffRef( 1, 0 ) = symbolic::Symbolic< OutType >( "d" );
    matA.coeffRef( 2, 0 ) = symbolic::Symbolic< OutType >( "g" );

    matA.coeffRef( 0, 1 ) = symbolic::Symbolic< OutType >( "b" );
    matA.coeffRef( 1, 1 ) = symbolic::Symbolic< OutType >( "e" );
    matA.coeffRef( 2, 1 ) = symbolic::Symbolic< OutType >( "h" );

    matA.coeffRef( 0, 2 ) = symbolic::Symbolic< OutType >( "c" );
    matA.coeffRef( 1, 2 ) = symbolic::Symbolic< OutType >( "f" );
    matA.coeffRef( 2, 2 ) = symbolic::Symbolic< OutType >( "i" );

    SymbolicMatrix< OutType > matB( 3, 1 );
    matB.coeffRef( 0, 0 ) = symbolic::Symbolic< OutType >( "x1" );
    matB.coeffRef( 1, 0 ) = symbolic::Symbolic< OutType >( "x2" );
    matB.coeffRef( 2, 0 ) = symbolic::Symbolic< OutType >( "x3" );
    SymbolicMatrix< OutType > sol = Eigen::solve123( matA, matB );


    //    std::cout << "Here is the matrix A:\n" << matA << std::endl;
    //    std::cout << "Here is the matrix B:\n" << matB << std::endl;
    //    std::cout << "Here is the solution for coeffRef(0,0):\n" << sol.coeffRef( 0, 0 ) << std::endl;
    //    std::cout << "Here is the solution for coeffRef(1,0):\n" << sol.coeffRef( 1, 0 ) << std::endl;
    //    std::cout << "Here is the solution for coeffRef(2,0):\n" << sol.coeffRef( 2, 0 ) << std::endl;


    TS_ASSERT_EQUALS( sol.coeffRef( 0, 0 ),
                      symbolic::Symbolic< OutType >(
                       "MUL(SUB(SUB(x1,MUL(MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a)))),MUL(SUB(x2,MUL(x1,MUL(d,DIV("
                       "1.000000,a)))),MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,"
                       "DIV(1.000000,a)),b),-1),e))))),DIV(1,ADD(i,MUL(-1,ADD(MUL(MUL(g,DIV(1.000000,a)),c),MUL(MUL("
                       "ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b)"
                       ",-1),e))),SUB(f,MUL(c,MUL(d,DIV(1.000000,a)))))))))),c)),MUL(MUL(SUB(SUB(x2,MUL(x1,MUL(d,DIV(1."
                       "000000,a)))),MUL(MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a)))),MUL(SUB(x2,MUL(x1,MUL(d,DIV(1."
                       "000000,a)))),MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,"
                       "DIV(1.000000,a)),b),-1),e))))),DIV(1,ADD(i,MUL(-1,ADD(MUL(MUL(g,DIV(1.000000,a)),c),MUL(MUL("
                       "ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b)"
                       ",-1),e))),SUB(f,MUL(c,MUL(d,DIV(1.000000,a)))))))))),SUB(f,MUL(c,MUL(d,DIV(1.000000,a)))))),"
                       "DIV(1,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))),b)),DIV(1,a))" ) );
    TS_ASSERT_EQUALS( sol.coeff( 1, 0 ),
                      symbolic::Symbolic< OutType >(
                       "MUL(SUB(SUB(x2,MUL(x1,MUL(d,DIV(1.000000,a)))),MUL(MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a))"
                       ")),MUL(SUB(x2,MUL(x1,MUL(d,DIV(1.000000,a)))),MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),"
                       "DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))))),DIV(1,ADD(i,MUL(-1,ADD(MUL(MUL(g,"
                       "DIV(1.000000,a)),c),MUL(MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL("
                       "MUL(MUL(d,DIV(1.000000,a)),b),-1),e))),SUB(f,MUL(c,MUL(d,DIV(1.000000,a)))))))))),SUB(f,MUL(c,"
                       "MUL(d,DIV(1.000000,a)))))),DIV(1,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e)))" ) );
    TS_ASSERT_EQUALS( sol.coeff( 2, 0 ),
                      symbolic::Symbolic< OutType >(
                       "MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a)))),MUL(SUB(x2,MUL(x1,MUL(d,DIV(1.000000,a)))),MUL("
                       "ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b)"
                       ",-1),e))))),DIV(1,ADD(i,MUL(-1,ADD(MUL(MUL(g,DIV(1.000000,a)),c),MUL(MUL(ADD(MUL(MUL(MUL(g,DIV("
                       "1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))),SUB(f,MUL(c,"
                       "MUL(d,DIV(1.000000,a))))))))))" ) );

    /*
        TS_ASSERT_EQUALS( sol.coeffRef( 0, 0 ),
                          symbolic::Symbolic< OutType >(
                           "MUL(SUB(SUB(x1,MUL(MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a)))),MUL(SUB(x2,MUL(x1,MUL(d,DIV("
                           "1.000000,a)))),MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,"
                           "DIV(1.000000,a)),b),-1),e))))),DIV(1,ADD(MUL(ADD(MUL(MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),"
                           "-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))),SUB(f,MUL(c,MUL(d,DIV(1."
                           "000000,a))))),MUL(MUL(g,DIV(1.000000,a)),c)),-1),i))),c)),MUL(MUL(SUB(SUB(x2,MUL(x1,MUL(d,DIV("
                           "1.000000,a)))),MUL(MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a)))),MUL(SUB(x2,MUL(x1,MUL(d,DIV("
                           "1.000000,a)))),MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,"
                           "DIV(1.000000,a)),b),-1),e))))),DIV(1,ADD(MUL(ADD(MUL(MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),"
                           "-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))),SUB(f,MUL(c,MUL(d,DIV(1."
                           "000000,a))))),MUL(MUL(g,DIV(1.000000,a)),c)),-1),i))),SUB(f,MUL(c,MUL(d,DIV(1.000000,a)))))),"
                           "DIV(1,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))),b)),DIV(1,a))" ) );

        TS_ASSERT_EQUALS( sol.coeff( 1, 0 ),
                          symbolic::Symbolic< OutType >(
                           "MUL(SUB(SUB(x2,MUL(x1,MUL(d,DIV(1.000000,a)))),MUL(MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a))"
                           ")),MUL(SUB(x2,MUL(x1,MUL(d,DIV(1.000000,a)))),MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),"
                           "DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))))),DIV(1,ADD(MUL(ADD(MUL(MUL(ADD("
                           "MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1)"
                           ",e))),SUB(f,MUL(c,MUL(d,DIV(1.000000,a))))),MUL(MUL(g,DIV(1.000000,a)),c)),-1),i))),SUB(f,MUL("
                           "c,MUL(d,DIV(1.000000,a)))))),DIV(1,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e)))" ) );
        TS_ASSERT_EQUALS( sol.coeff( 2, 0 ),
                          symbolic::Symbolic< OutType >(
                           "MUL(SUB(SUB(x3,MUL(x1,MUL(g,DIV(1.000000,a)))),MUL(SUB(x2,MUL(x1,MUL(d,DIV(1.000000,a)))),MUL("
                           "ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1.000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b)"
                           ",-1),e))))),DIV(1,ADD(MUL(ADD(MUL(MUL(ADD(MUL(MUL(MUL(g,DIV(1.000000,a)),b),-1),h),DIV(1."
                           "000000,ADD(MUL(MUL(MUL(d,DIV(1.000000,a)),b),-1),e))),SUB(f,MUL(c,MUL(d,DIV(1.000000,a))))),"
                           "MUL(MUL(g,DIV(1.000000,a)),c)),-1),i)))" ) );
    */


    // MATLAB / Octave code
    //    a = 1; b=2; c = -1; d=2; e=2; f=2; g = 1; h = -1; i = 2;
    //    x1 = 2; x2= 12; x3=5;
    //    A = [a,b, c; d,e,f; g,h,i];
    //    C = [x1;x2;x3];
    //    A\C
    //    sollte : [1;2;3] sein
    //    ADD = @plus;
    //    DIV = @rdivide;
    //    SUB = @minus;
    //    MUL = @times;
    //


#endif
}

void TestSymbolic::TestInvert()
{
#ifdef _SYMBOLIC_
    SymbolicMatrix< OutType > leftFactor( 3, 3 );
    leftFactor.coeffRef( 0, 0 ) = symbolic::Symbolic< OutType >( "L00" );
    leftFactor.coeffRef( 1, 1 ) = symbolic::Symbolic< OutType >( "L11" );
    leftFactor.coeffRef( 1, 2 ) = symbolic::Symbolic< OutType >( "L12" );
    leftFactor.coeffRef( 2, 1 ) = symbolic::Symbolic< OutType >( "R21" );
    leftFactor.coeffRef( 2, 2 ) = symbolic::Symbolic< OutType >( "R22" );
/*
auto x = inv( leftFactor );

auto rhs = symbolic::Symbolic< OutType >( "DIV(1.000000,L00)" );
TS_ASSERT( x( 0, 0 ) == rhs );
TS_ASSERT( x( 0, 1 ).IsEmpty() );
TS_ASSERT( x( 0, 2 ).IsEmpty() );
TS_ASSERT( x( 1, 0 ).IsEmpty() );
TS_ASSERT( x( 2, 0 ).IsEmpty() );
rhs = symbolic::Symbolic< OutType >( "DIV(NEG(MUL(R22,DIV(1.000000,SUB(L12,MUL(MUL(L11,DIV(1.000000,R21)),R22))))),R21)"
);
TS_ASSERT( x( 1, 1 ) == rhs );
rhs = symbolic::Symbolic< OutType >(
 "DIV(SUB(1.000000,MUL(R22,DIV(NEG(MUL(L11,DIV(1.000000,R21))),SUB(L12,MUL(MUL(L11,DIV(1.000000,R21)),R22))))),"
 "R21)" );
TS_ASSERT( x( 1, 2 ) == rhs );
rhs = symbolic::Symbolic< OutType >( "DIV(1.000000,SUB(L12,MUL(MUL(L11,DIV(1.000000,R21)),R22)))" );
TS_ASSERT( x( 2, 1 ) == rhs );
rhs = symbolic::Symbolic< OutType >( "DIV(NEG(MUL(L11,DIV(1.000000,R21))),SUB(L12,MUL(MUL(L11,DIV(1.000000,R21)),R22)))"
);

TS_ASSERT( x( 2, 2 ) == rhs );
*/
#endif
}
