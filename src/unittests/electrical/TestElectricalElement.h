/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestElectricalElement.h
* Creation Date : 01-11-2012
* Last Modified : Fr 21 Feb 2014 10:29:44 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTELECTRICALELEMENT_
#define _TESTELECTRICALELEMENT_
#include <cxxtest/TestSuite.h>

class TestElectricalElement : public CxxTest::TestSuite
{
    public:
    void testElectricalElementOhmicConst();
    void testElectricalElementOhmic1DLookup();
    void testElectricalElementVoltageSourceConst();
    void testElectricalElementOhmic2DLookup();
    void testElectricalElementVoltageSource1DLookup();
    void testElectricalElementVoltageSource2DLookup();
    void testElectricalParllelRCCreation();
    void testElectricalElementOhmPower();
    void testElectricalElementCapPower();
    void testElectricalElementParallelRCPower();
    void testCellelementWithReversibleHeat();
    void testCellelementWithoutReverisbleHeat();
    void testTotalCapacitySingleCell();
    void testTotalCapacitySerialUnequalCapacity();
    void testTotalCapacityParallelCapacity();
    void testTotalCapacitySerial();
    void testTotalCapacitySerialChildWithoutCapacity();
};
#endif /* _TESTELECTRICALELEMENT_ */
