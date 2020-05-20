/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestVoltageCurrentPowerInjection.h
* Creation Date : 07-03-2016
* Last Modified : Mo 07 Mär 2016 18:28:55 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTVOLTAGECURRENTPOWERINJECTION_
#define _TESTVOLTAGECURRENTPOWERINJECTION_

#include <cxxtest/TestSuite.h>

class TestVoltageCurrentPowerInjection : public CxxTest::TestSuite
{
    public:
    void TestTwoArgumentsTwoPorts();
    void TestThreeArgumentsTwoPorts();
    void TestFourArgumentsTwoPorts();
    void TestFiveArgumentsTwoPorts();

    void TestEveryhtingElse();

    template < typename MatrixT, template < typename > class TConcrete >
    void TwoArgs();

    template < typename MatrixT, template < typename > class TConcrete >
    void ThreeArgs();

    template < typename MatrixT, template < typename > class TConcrete >
    void FourArgs();

    template < typename MatrixT, template < typename > class TConcrete >
    void FiveArgs();
};
#endif /* _TESTVOLTAGECURRENTPOWERINJECTION_ */
