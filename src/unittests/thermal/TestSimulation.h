#ifndef _TESTSIMULATION_
#define _TESTSIMULATION_

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4018)
#endif

#include <boost/numeric/odeint.hpp>
#include <boost/ref.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <cxxtest/TestSuite.h>
#include <cstring>


class TestSimulation: public CxxTest::TestSuite
{
    public:
        void TestScenario();
        void TestParallelRC();
        void TestErrorHandlingAtConstruction();
        void EvaluateCsvFile(const std::string &line, size_t &i, double &timeEl, double &cur, double &volt, double &soc, double &heat, double &temp);

    private:
    void RunScenario(double electricalStopCriterion, double thermalStopCriterion, bool stopCriterionTriggered);
    struct observerOutput {
        size_t mI;
        double mTimeEl;
        double mCur;
        double mVolt;
        double mSoc;
        double mHeat;
        double mTemp;
    };
    protected:
};

#endif
