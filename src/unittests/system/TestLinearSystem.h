/*
 * TestLinearSystem.h
 *
 *  Created on: 04.12.2013
 *      Author: chris
 */

#ifndef TESTLINEARSYSTEM_H_
#define TESTLINEARSYSTEM_H_

#include <cxxtest/TestSuite.h>

class TestLinearSystem: public CxxTest::TestSuite
{
public:
    void testParallelRCAlgElement();
    void testSingleAlgCellellement();
private:
    std::vector< std::vector<double> > CopyToVector(const double data[7][4]);
};

#endif /* TESTLINEARSYSTEM_H_ */
