/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestView.h
* Creation Date : 09-04-2015
* Last Modified : Mo 06 Jul 2015 11:10:43 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTVIEW_
#define _TESTVIEW_

#include <cxxtest/TestSuite.h>

/// Testsuite for the View class
class TestView : public CxxTest::TestSuite
{
    public:
    void TestViewCreation();
    void TestViewCreationFromArray();
    void TestFill();
    void TestViewView();
    void TestCorruptView();
};
#endif /* _TESTVIEW_ */
