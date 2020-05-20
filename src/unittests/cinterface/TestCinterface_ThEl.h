#ifndef _TEST_CINTERFACE_TH_EL_
#define _TEST_CINTERFACE_TH_EL_
#include <cxxtest/TestSuite.h>

class TestCinterface_ThEl : public CxxTest::TestSuite
{
    public:
    void TestCreation();
    void TestScenario();
    void TestScenarioDiscreteSimuDGL();

    private:
    protected:
};
#endif /* _TEST_CINTERFACE_TH_EL */
