/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSvgLibraryInterpreter.h
* Creation Date : 15-04-2015
* Last Modified : Mo 20 Apr 2015 20:21:49 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSVGLIBRARYINTERPRETER_
#define _TESTSVGLIBRARYINTERPRETER_
#include <cxxtest/TestSuite.h>

class TestSvgLibraryInterpreter : public CxxTest::TestSuite
{
    public:
    void testCreation();
    void testFindAttribute();
    void testUnfindableTagId();
    void testUngetableTagId();
    void testGetValidTagIdSimple();
    void testGetValidTagWithPlace();
};
#endif /* _TESTSVGLIBRARYINTERPRETER_ */
