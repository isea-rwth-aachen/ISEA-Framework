/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMatio.h
* Creation Date : 03-07-2015
* Last Modified : Mi 05 Aug 2015 16:26:20 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMATIO_
#define _TESTMATIO_
#include <cxxtest/TestSuite.h>

class TestMatio : public CxxTest::TestSuite
{
    public:
    void TestMatFileEmpty();
    void TestMatFileEmptyStruct();
    void TestMatFileFail();
    void TestMatFileContent1D();
    void TestMatFileContent2D();
    void TestMatFileContent2DSlice();
    void TestMatFileContent2DSliceTokenAcccesAtFileLevel();
    void TestMatFileContent2DSliceTokenAcccesAtDataLevel();
    void TestMadioDataCreationFromVector1D();
    void TestMadioDataCreationFromVector2D();
    void TestMatFileWriteAndRead();


    private:
    protected:
};
#endif /* _TESTMATIO_ */
