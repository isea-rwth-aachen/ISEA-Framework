/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSymbolicExport.h
* Creation Date : 10-01-2017
* Last Modified : Di 20 Jun 2017 12:34:37 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSYMBOLICEXPORT_
#define _TESTSYMBOLICEXPORT_
#include <cxxtest/TestSuite.h>

/// This class is testing the creation of a different Exportformats based on the intermediate language
class TestSymbolicExport : public CxxTest::TestSuite
{
    public:
    void TestSymbolicExportCreation();
    void TestSymbolicExportCreationName();
};


#endif /* _TESTSYMBOLICEXPORT_ */
