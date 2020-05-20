#ifndef _TESTFASTCOPYMATRIX_
#define _TESTFASTCOPYMATRIX_
#include <cxxtest/TestSuite.h>

class TestFastCopyMatrix : public CxxTest::TestSuite
{
    public:
    void TestFastCopyMatEqualSize();
    void TestFastCopyMatUnequalSize();
    void TestFastCopyVecToMat();
    void TestFastCopyVecToMatUnequalSize();

    void TestMergeOfTwoMatrixes();
};
#endif    // _TESTFASTCOPYMATRIX_
