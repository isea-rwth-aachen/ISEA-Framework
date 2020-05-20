/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMatlab.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 11:33:40 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestMatlab.h"

#ifdef _MATLAB_
#include "../../matlab/mmatrix.h"
#endif

void TestMmatrix::TestMmatrixCreation()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(10,10);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixEqualOperator()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(2,2);
    matlab::MMatrix<mxDOUBLE_CLASS> test2(2,2);
    test(0,0) = 1;
    test(0,1) = 2;
    test(1,0) = 3;
    test(1,1) = 4;

    test2(0,0) = 5;
    test2(0,1) = 6;
    test2(1,0) = 7;
    test2(1,1) = 8;
    test = test2;
#endif /* _MATLAB_ */
}

void TestMmatrix::TestMmatrixFromMxArray()
{
#ifdef _MATLAB_
    mxArray *c  = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
    void *firstElement = mxGetData(c);
    *static_cast<double*>(firstElement) = 1000;
    matlab::MMatrix<mxDOUBLE_CLASS> f(c);
    TS_ASSERT_EQUALS(f.GetElement(0,0), 1000);

    TS_ASSERT_EQUALS(f.GetColCount(), 1);
    TS_ASSERT_EQUALS(f.GetRowCount(), 1);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixFromMmatrix()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(10,10);
    TS_ASSERT_EQUALS(test.GetColCount(), 10);
    TS_ASSERT_EQUALS(test.GetRowCount(), 10);
    for( size_t i = 0; i< 10; ++i)
    {
        test(i,i) = 1;
        TS_ASSERT_EQUALS(test.GetElement(i,i), 1.0);
    }

    matlab::MMatrix<mxDOUBLE_CLASS> testCopy(test);

    TS_ASSERT_EQUALS(testCopy.GetColCount(), 10);
    TS_ASSERT_EQUALS(testCopy.GetRowCount(), 10);
    for( size_t j = 0; j< 10; ++j)
    {
        for( size_t i = 0; i< 10; ++i)
        {
            test(i,i) = 1;
            TS_ASSERT_EQUALS(testCopy.GetElement(j,i), test.GetElement(j,i) );
        }
    }

#endif /* _MATLAB_ */
}

void TestMmatrix::TestMmatrixElementFromIdentity()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(10,10);
    TS_ASSERT_EQUALS(test.GetColCount(), 10);
    TS_ASSERT_EQUALS(test.GetRowCount(), 10);
    for( size_t i = 0; i< 10; ++i)
    {
        test(i,i) = 1;
        TS_ASSERT_EQUALS(test.GetElement(i,i), 1.0);
    }
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixRowIdentity()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(10,10);
    TS_ASSERT_EQUALS(test.GetColCount(), 10);
    TS_ASSERT_EQUALS(test.GetRowCount(), 10);
    for( size_t i = 0; i< 10; ++i)
    {
        test(i,0) = 1;
        TS_ASSERT_EQUALS(test.GetElement(i,0), 1.0);
    }
#endif /* _MATLAB_ */
}

void TestMmatrix::TestMmatrixSubmat()
{
#ifdef _MATLAB_
    //        matlab::MMatrix<mxDOUBLE_CLASS> test(3,3);
    matlab::MMatrix<mxDOUBLE_CLASS> test(10,10);
    TS_ASSERT_EQUALS(test.GetColCount(), 10);
    TS_ASSERT_EQUALS(test.GetRowCount(), 10);
    for( size_t i = 0; i< 10; ++i)
    {
        test(i,i) = i;
        TS_ASSERT_EQUALS(test.GetElement(i,i), i);
    }

    const matlab::MMatrix<mxDOUBLE_CLASS> c = test.Submat(0,2,2,2);
    TS_ASSERT_EQUALS(c.GetElement(0,0), 0);
    TS_ASSERT_EQUALS(c.GetElement(1,0), 0);
    TS_ASSERT_EQUALS(c.GetElement(2,0), 2);

    const matlab::MMatrix<mxDOUBLE_CLASS> d = test.Submat(7,7,9,9);
    TS_ASSERT_EQUALS(d.GetElement(0,0), 7);
    TS_ASSERT_EQUALS(d.GetElement(1,0), 0);
    TS_ASSERT_EQUALS(d.GetElement(2,0), 0);
    TS_ASSERT_EQUALS(d.GetElement(0,1), 0);
    TS_ASSERT_EQUALS(d.GetElement(1,1), 8);
    TS_ASSERT_EQUALS(d.GetElement(2,1), 0);
    TS_ASSERT_EQUALS(d.GetElement(0,2), 0);
    TS_ASSERT_EQUALS(d.GetElement(1,2), 0);
    TS_ASSERT_EQUALS(d.GetElement(2,2), 9);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixMatrixInvertMember()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(3,3);
    test(0,0) = 8;
    test(0,1) = 3;
    test(0,2) = 4;
    test(1,0) = 1;
    test(1,1) = 5;
    test(1,2) = 9;
    test(2,0) = 6;
    test(2,1) = 7;
    test(2,2) = 2;
    test.Invert();
    TS_ASSERT_DELTA(test.GetElement(0,0), 0.147222, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(1,0), -0.144444, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(2,0), 0.0638889, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(0,1), -0.0611111, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(1,1), 0.0222222, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(2,1), 0.1055569, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(0,2), -0.0194444, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(1,2), 0.1888894, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(2,2), -0.102778, 0.00001);

#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixMatrixInvert()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(3,3);
    test(0,0) = 8;
    test(0,1) = 3;
    test(0,2) = 4;
    test(1,0) = 1;
    test(1,1) = 5;
    test(1,2) = 9;
    test(2,0) = 6;
    test(2,1) = 7;
    test(2,2) = 2;
    test = matlab::Invert(test);
    TS_ASSERT_DELTA(test.GetElement(0,0), 0.147222, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(1,0), -0.144444, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(2,0), 0.0638889, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(0,1), -0.0611111, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(1,1), 0.0222222, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(2,1), 0.1055569, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(0,2), -0.0194444, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(1,2), 0.1888894, 0.00001);
    TS_ASSERT_DELTA(test.GetElement(2,2), -0.102778, 0.00001);

#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixEye()
{
#ifdef _MATLAB_
    size_t testElement(10);

    matlab::MMatrix<mxDOUBLE_CLASS> test = matlab::Eye<mxDOUBLE_CLASS>(testElement);
    for (size_t i = 0; i < testElement; ++i)
        TS_ASSERT_EQUALS(test.GetElement(i,i), 1.0);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixZerosUnequal()
{
#ifdef _MATLAB_
    size_t testElement(10);

    matlab::MMatrix<mxDOUBLE_CLASS> test (matlab::zeros<mxDOUBLE_CLASS>(testElement, testElement + 1));
    for (size_t i = 0; i < testElement; ++i)
        for (size_t j = 0; j < testElement; ++j)
            TS_ASSERT_EQUALS(test.GetElement(i,j), 0.0);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixOperator_Plus()
{
#ifdef _MATLAB_
    size_t testElement(10);

    matlab::MMatrix<mxDOUBLE_CLASS> testA (matlab::Eye<mxDOUBLE_CLASS>(testElement));
    matlab::MMatrix<mxDOUBLE_CLASS> testB (matlab::Eye<mxDOUBLE_CLASS>(testElement));
    matlab::MMatrix<mxDOUBLE_CLASS> testC (testA + testB);
    for (size_t i = 0; i < testElement; ++i)
        TS_ASSERT_DELTA(testC.GetElement(i,i), 2.0, 0.00001);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixOperator_PlusEqual()
{
#ifdef _MATLAB_
    size_t testElement(10);

    matlab::MMatrix<mxDOUBLE_CLASS> testA (matlab::Eye<mxDOUBLE_CLASS>(testElement));
    testA += matlab::Eye<mxDOUBLE_CLASS>(testElement);
    for (size_t i = 0; i < testElement; ++i)
        TS_ASSERT_DELTA(testA.GetElement(i,i), 2.0, 0.00001);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixOperator_Minus()
{
#ifdef _MATLAB_
    size_t testElement(10);

    matlab::MMatrix<mxDOUBLE_CLASS> testA (matlab::Eye<mxDOUBLE_CLASS>(testElement));
    matlab::MMatrix<mxDOUBLE_CLASS> testB (matlab::Eye<mxDOUBLE_CLASS>(testElement));
    matlab::MMatrix<mxDOUBLE_CLASS> testC (testA - testB);
    for (size_t i = 0; i < testElement; ++i)
    {
        TS_ASSERT_DELTA(testC.GetElement(i,i), 0.0, 0.00001);
        TS_ASSERT_DELTA(testA.GetElement(i,i), 1.0, 0.00001);
        TS_ASSERT_DELTA(testB.GetElement(i,i), 1.0, 0.00001);
    }
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixOperator_DivideElementwise()
{
#ifdef _MATLAB_
    size_t testElement(10);
    matlab::MMatrix<mxDOUBLE_CLASS> testA (10 * matlab::Ones<mxDOUBLE_CLASS>(testElement, testElement ));
    matlab::MMatrix<mxDOUBLE_CLASS> testB (20 * matlab::Ones<mxDOUBLE_CLASS>(testElement, testElement ));
    matlab::MMatrix<mxDOUBLE_CLASS> testC (testA / testB);
    for (size_t i = 0; i < testElement; ++i)
    {
        for (size_t j = 0; j < testElement; ++j)
        {
            TS_ASSERT_DELTA(testC.GetElement(i,j), 0.5, 0.00001);
            TS_ASSERT_DELTA(testA.GetElement(i,j), 10.0, 0.00001);
            TS_ASSERT_DELTA(testB.GetElement(i,j), 20.0, 0.00001);
        }
    }
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixOperator_DivideScalar()
{
#ifdef _MATLAB_
    size_t testElement(10);
    matlab::MMatrix<mxDOUBLE_CLASS> testA (10 * matlab::Ones<mxDOUBLE_CLASS>(testElement, testElement ));
    double scalar = 20;
    matlab::MMatrix<mxDOUBLE_CLASS> testC (testA / scalar);
    for (size_t i = 0; i < testElement; ++i)
    {

        for (size_t j = 0; j < testElement; ++j)
        {
            TS_ASSERT_DELTA(testC.GetElement(i,j), 0.5, 0.00001);
            TS_ASSERT_DELTA(testA.GetElement(i,j), 10.0, 0.00001);
        }
    }
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixOperator_MultiplyEqualSize()
{
#ifdef _MATLAB_
    size_t testElement(10);
    matlab::MMatrix<mxDOUBLE_CLASS> testA (10 * matlab::Ones<mxDOUBLE_CLASS>(testElement, testElement ));
    matlab::MMatrix<mxDOUBLE_CLASS> testB (0.1 * matlab::Ones<mxDOUBLE_CLASS>(testElement, testElement ));
    matlab::MMatrix<mxDOUBLE_CLASS> testC (testA * testB);
    for (size_t i = 0; i < testElement; ++i)
        for (size_t j = 0; j < testElement; ++j)
            TS_ASSERT_DELTA(testC.GetElement(i,j), 10.0, 0.00001);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixOperator_MultiplyUnequalSize()
{
#ifdef _MATLAB_
    const size_t testElement(10);
    const size_t final_x = 3, final_y = 5;
    matlab::MMatrix<mxDOUBLE_CLASS> testA (10 * matlab::Ones<mxDOUBLE_CLASS>(final_x, testElement ));
    matlab::MMatrix<mxDOUBLE_CLASS> testB (0.1 * matlab::Ones<mxDOUBLE_CLASS>(testElement, final_y ));
    matlab::MMatrix<mxDOUBLE_CLASS> testC (testA * testB);
    for (size_t i = 0; i < final_x; ++i)
        for (size_t j = 0; j < final_y; ++j)
            TS_ASSERT_DELTA(testC.GetElement(i,j), 10.0, 0.00001);
#endif /* _MATLAB_ */
}
void TestMmatrix::TestMmatrixRowCol()
{
#ifdef _MATLAB_
    matlab::MMatrix<mxDOUBLE_CLASS> test(2,2);
    {
        test(0,0) = 1;
        test(0,1) = 0;
        test(1,0) = 1;
        test(1,1) = 0;
        TS_ASSERT_EQUALS(test.Row(0).GetElement(0,0) , 1)
            TS_ASSERT_EQUALS(test.Row(0).GetElement(0,1) , 0)

            TS_ASSERT_EQUALS(test.Row(1).GetElement(0,0) , 1)
            TS_ASSERT_EQUALS(test.Row(1).GetElement(0,1) , 0)

            TS_ASSERT_EQUALS(test.Col(0).GetElement(0,0) , 1)
            TS_ASSERT_EQUALS(test.Col(0).GetElement(1,0) , 1)

            TS_ASSERT_EQUALS(test.Col(1).GetElement(0,0) , 0)
            TS_ASSERT_EQUALS(test.Col(1).GetElement(1,0) , 0)

    }
#endif /* _MATLAB_ */
}

void TestMmatrix::TestMmatrixRightHandSolve()
{
#ifdef _MATLAB_
    matlab::MMatrix<> matA(3,3);
    for( size_t i = 0 ; i < 3 ; ++i )
        matA(i,i) = 1;
    matA(0,1) = 2;
    matA(0,2) = 3;
    matA(1,2) = 1;
    matlab::MMatrix<> matB(3,1);
    matB(0,0) = 4;
    matB(1,0) = 2;
    matB(2,0) = 3;
    matlab::MMatrix<> matC = RightHandSolve(matA,matB);
    TS_ASSERT_EQUALS(matC(0,0), -3);
    TS_ASSERT_EQUALS(matC(1,0), -1);
    TS_ASSERT_EQUALS(matC(2,0),  3);

#endif /* _MATLAB_ */
}

void TestMmatrix::testMatrixLeftHandSolve()
{
#ifdef _MATLAB_
    matlab::MMatrix<> matA(3,3);
    for( size_t i = 0 ; i < 3 ; ++i )
        matA(i,i) = 1;
    matA(1,0) = 2;
    matA(2,0) = 3;
    matA(2,1) = 1;
    matlab::MMatrix<> matB(3,1);
    matB(0,0) = 4;
    matB(1,0) = 2;
    matB(2,0) = 3;
    matlab::MMatrix<> matC = LeftHandSolve(matA,matB);
    TS_ASSERT_DELTA(matC(0,0),  4, 0.0001);
    TS_ASSERT_DELTA(matC(1,0), -6, 0.0001);
    TS_ASSERT_DELTA(matC(2,0), -3, 0.0001);
#endif /* _MATLAB_ */
}

void TestMmatrix::testMatrixSolve()
{
#ifdef _MATLAB_
    matlab::MMatrix<> matA(3,3);
    matA(0,0) = 8;
    matA(1,0) = 3;
    matA(2,0) = 4;

    matA(0,1) = 1;
    matA(1,1) = 5;
    matA(2,1) = 9;

    matA(0,2) = 6;
    matA(1,2) = 7;
    matA(2,2) = 2;

    matlab::MMatrix<> matB(3,1);
    matB(0,0) = 1;
    matB(1,0) = 2;
    matB(2,0) = 3;
    matlab::MMatrix<> matC = solve(matA,matB);
    TS_ASSERT_DELTA(matC(0,0), 0.05, 0.00001);
    TS_ASSERT_DELTA(matC(1,0), 0.3, 0.00001);
    TS_ASSERT_DELTA(matC(2,0), 0.05, 0.00001);


        /*
           8.00000   1.00000   6.00000
           0.50000   8.50000  -1.00000
           0.37500   0.54412   5.29412

           8   1   6
           3   5   7
           4   9   2
           */
#endif /* _MATLAB_ */
}









