/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestDaeSystem.h
* Creation Date : 03-11-2012
* Last Modified : Mi 13 Aug 2014 09:55:57 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTDAESYSTEM_
#define _TESTDAESYSTEM_
#include <cxxtest/TestSuite.h>
#include <vector>

class TestDaeSystem: public CxxTest::TestSuite
{
    public:

        void testDaeSystemMixedSystemCurrentUnsolveable();
        void testDaeSystemMixedSystemCurrentSolveable();
        void testDaeSystemMixedSystemCurrentSolveableIntegrate();
        void testRC();
        void testRCParallel();
        void testRCSerial();
        void testParallelRCElement();
        void testConstantZarcSynthetic();
        void testRealZarc();
        void testSingleCellellement();
        void testSingleCellellementBalancing();
        void testMultiCellellementBalancing();
        void testVariableStepSolverWithParallelRCMindingResults();
    private:
        std::vector< std::vector<double> > CopyToVector(const double data[7][4]);

};
#endif /* _TESTDAESYSTEM_ */
