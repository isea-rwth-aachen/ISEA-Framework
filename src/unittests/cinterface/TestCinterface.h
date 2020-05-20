/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestCinterface.h
* Creation Date : 27-08-2013
* Last Modified : Fr 21 Feb 2014 10:08:59 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTCINTERFACE_
#define _TESTCINTERFACE_
#include <cxxtest/TestSuite.h>

class TestCinterface: public CxxTest::TestSuite
{
    public:
        const char* ConfigWithCellelement();
        const char* ConfigWithParallelRC();
        void TestCinterfaceCreating();
        void TestCinterfaceReset();
        void TestCinterfaceGetSize();
        void TestCinterfaceSetCurrent();
        void TestCinterfaceRunSystem();
        void TestCinterfaceSimulationWithParallelRC();

    private:
    protected:
};
#endif /* _TESTENTITY_ */
