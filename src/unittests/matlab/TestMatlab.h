/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.  * File Name : TestMmatrix.h
* Creation Date : 11-11-2012
* Last Modified : Fr 21 Feb 2014 11:36:15 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMMATRIX_
#define _TESTMMATRIX_

#include <cxxtest/TestSuite.h>



class TestMmatrix: public CxxTest::TestSuite
{
public:
    void TestMmatrixCreation();
    void TestMmatrixEqualOperator();
    void TestMmatrixFromMxArray();
    void TestMmatrixFromMmatrix();
    void TestMmatrixElementFromIdentity();
    void TestMmatrixRowIdentity();
    void TestMmatrixSubmat();
    void TestMmatrixMatrixInvertMember();
    void TestMmatrixMatrixInvert();
    void TestMmatrixEye();
    void TestMmatrixZerosUnequal();
    void TestMmatrixOperator_Plus();
    void TestMmatrixOperator_PlusEqual();
    void TestMmatrixOperator_Minus();
    void TestMmatrixOperator_DivideElementwise();
    void TestMmatrixOperator_DivideScalar();
    void TestMmatrixOperator_MultiplyEqualSize();
    void TestMmatrixOperator_MultiplyUnequalSize();
    void TestMmatrixRowCol();
    void TestMmatrixRightHandSolve();
    void testMatrixLeftHandSolve();
    void testMatrixSolve();
};

#endif /* _TESTZARC_ */
