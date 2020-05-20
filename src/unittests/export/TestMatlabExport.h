/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMatlabExport.h
* Creation Date : 23-06-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMATLABEXPORT_
#define _TESTMATLABEXPORT_
#include <cxxtest/TestSuite.h>
/// DESCRIPTION
class TestMatlabExport : public CxxTest::TestSuite
{
    public:
    void TestCreation();
    void TestCreationFaulty();
    void TestSystems();
};
#endif /* _TESTMATLABEXPORT_ */
