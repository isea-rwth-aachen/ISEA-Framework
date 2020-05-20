#ifndef _TESTTIMESERIES_
#define _TESTTIMESERIES_

#include <cxxtest/TestSuite.h>


class TestTimeSeries : public CxxTest::TestSuite
{
    public:
    void TestEvalNoInterpolation();
    void TestEvalLinearInterpolation();
    void TestTimeSeriesCreation();
    void TestTimeSeriesNoInterpolation();
    void TestTimeSeriesWithLinearInterpolation();
    void TestTimeSeriesScaling();
    void TestHeaderParsing();

    private:
    protected:
};

#endif
