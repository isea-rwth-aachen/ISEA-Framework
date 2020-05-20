#ifndef _TESTFACTORYLOOKUPCELLELEMENTS_
#define _TESTFACTORYLOOKUPCELLELEMENTS_

#include <cxxtest/TestSuite.h>



class TestFactoryLookUpCellElements: public CxxTest::TestSuite
{
    public:
        void TestGetElectricalDiscretization();
        void TestStateFactory();
        void TestObjectFactory();
        void TestElectricalFactory();
        void TestThermalFactory();

    private:
    protected:
};

#endif
