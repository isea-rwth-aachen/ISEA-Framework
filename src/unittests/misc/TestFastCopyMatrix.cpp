#include "TestFastCopyMatrix.h"
#include "../../misc/fast_copy_matrix.h"
#include <vector>

using misc::FastCopyMatrix;
static const double sDelta = 0.000001;

void TestFastCopyMatrix::TestFastCopyMatEqualSize()
{
    myMatrixType mat;
    mat.resize( 4, 1 );
    mat.setZero();
    mat.coeffRef( 0, 0 ) = 1.1;
    mat.coeffRef( 2, 0 ) = -2.04;
    mat.coeffRef( 3, 0 ) = 3.4;
    std::vector< double > vec( 4, -1111111111.1 );
    FastCopyMatrix( &vec.at( 0 ), mat, vec.size() );

    TS_ASSERT_DELTA( vec.at( 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( vec.at( 1 ), 0.0, sDelta );
    TS_ASSERT_DELTA( vec.at( 2 ), -2.04, sDelta );
    TS_ASSERT_DELTA( vec.at( 3 ), 3.4, sDelta );
}
void TestFastCopyMatrix::TestFastCopyMatUnequalSize()
{
    myMatrixType mat;
    mat.resize( 4, 1 );
    mat.setZero();
    mat.coeffRef( 0, 0 ) = 1.1;
    mat.coeffRef( 2, 0 ) = -2.04;
    mat.coeffRef( 3, 0 ) = 3.4;
    std::vector< double > vec( 4, -1111111111.1 );
    FastCopyMatrix( &vec.at( 0 ), mat, vec.size() - 1 );

    TS_ASSERT_DELTA( vec.at( 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( vec.at( 1 ), 0.0, sDelta );
    TS_ASSERT_DELTA( vec.at( 2 ), -2.04, sDelta );
    TS_ASSERT_DELTA( vec.at( 3 ), -1111111111.1, sDelta );
}

void TestFastCopyMatrix::TestFastCopyVecToMat()
{
    std::vector< double > vec( 4, 0 );
    vec.at( 0 ) = 1.1;
    vec.at( 2 ) = -2.04;
    vec.at( 3 ) = 3.4;
    myMatrixType mat( 4, 1 );
    FastCopyMatrix( mat, &vec.at( 0 ), vec.size() );

    TS_ASSERT_DELTA( mat.coeff( 0, 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( mat.coeff( 1, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( mat.coeff( 2, 0 ), -2.04, sDelta );
    TS_ASSERT_DELTA( mat.coeff( 3, 0 ), 3.4, sDelta );
}
void TestFastCopyMatrix::TestFastCopyVecToMatUnequalSize()
{
    std::vector< double > vec( 4, 0 );
    vec.at( 0 ) = 1.1;
    vec.at( 2 ) = -2.04;
    vec.at( 3 ) = 3.4;
    myMatrixType mat( 4, 1 );
    mat.coeffRef( 0, 0 ) = -1111111111.1;
    mat.coeffRef( 1, 0 ) = -1111111111.1;
    mat.coeffRef( 2, 0 ) = -1111111111.1;
    mat.coeffRef( 3, 0 ) = -1111111111.1;
    FastCopyMatrix( mat, &vec.at( 0 ), vec.size() - 1 );

    TS_ASSERT_DELTA( mat.coeff( 0, 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( mat.coeff( 1, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( mat.coeff( 2, 0 ), -2.04, sDelta );
    TS_ASSERT_DELTA( mat.coeff( 3, 0 ), -1111111111.1, sDelta );
}

void TestFastCopyMatrix::TestMergeOfTwoMatrixes()
{
    myMatrixType res;
    res.resize( 7, 1 );
    res.setZero();
    myMatrixType mat;
    mat.resize( 4, 1 );
    mat.setZero();
    mat.coeffRef( 0, 0 ) = 1.1;
    mat.coeffRef( 2, 0 ) = -2.04;
    mat.coeffRef( 3, 0 ) = 3.4;
    std::vector< double > vec( 3, 0 );
    vec.at( 1 ) = 23.4;
    vec.at( 2 ) = -1.2;
    FastCopyMatrix( res, mat, mat.rows(), &vec.at( 0 ), vec.size() );

    TS_ASSERT_DELTA( res.coeff( 0, 0 ), 1.1, sDelta );
    TS_ASSERT_DELTA( res.coeff( 1, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( res.coeff( 2, 0 ), -2.04, sDelta );
    TS_ASSERT_DELTA( res.coeff( 3, 0 ), 3.4, sDelta );
    TS_ASSERT_DELTA( res.coeff( 4, 0 ), 0.0, sDelta );
    TS_ASSERT_DELTA( res.coeff( 5, 0 ), 23.4, sDelta );
    TS_ASSERT_DELTA( res.coeff( 6, 0 ), -1.2, sDelta );
}
