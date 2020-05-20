/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestTwoPortsWithChildren.h
* Creation Date : 02-11-2012
* Last Modified : Fr 21 Feb 2014 10:33:30 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTTWOPORTSWIILDREN_
#define _TESTTWOPORTSWIILDREN_
#include <cxxtest/TestSuite.h>

class TestTwoPortsWithChildren : public CxxTest::TestSuite
{
    public:
    void testParallelPort();
    void testSerialPort();
    void testParallelPortOperation();
    void testSerialPortOperation();
    void testParallelPortChildren();
    void testSerialPortChildren();
    void testTwoPortWithSoc();
    void testResistanceFactor();
};
#endif /* _TESTTWOPORTSWIILDREN_ */
