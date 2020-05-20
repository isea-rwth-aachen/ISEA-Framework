#ifndef _TESTFACTORIES_
#define _TESTFACTORIES_

#include <cstring>
#include <cxxtest/TestSuite.h>


class TestFactories : public CxxTest::TestSuite
{
    public:
    void TestCoolingFactory();
    void TestCoolingBlockFactory();
    void TestMaterialFactory();
    void TestBlockFactory();
    void TestThermalFactory();

    private:
    protected:
};

#endif
