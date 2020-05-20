/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestObserverFactories.h
* Creation Date : 10-08-2015
* Last Modified : Di 11 Aug 2015 14:34:31 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTOBSERVERFACTORIES_
#define _TESTOBSERVERFACTORIES_
/// DESCRIPTION
#include <cxxtest/TestSuite.h>

class TestObserverFactories : public CxxTest::TestSuite
{
    public:
    void TestObserverFactoryCSVCreation();
    void TestObserverFactorySTDCreation();
    void TestObserverFactoryDecimationFilterCreation();
    void TestObserverFactoryMatlabFilterCreation();
    void TestObserverFactorySqliteFilterCreation();
};
#endif /* _TESTOBSERVERFACTORIES_ */
