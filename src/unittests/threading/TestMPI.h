/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMPI.h
* Creation Date : 13-11-2013
* Last Modified : Fr 21 Feb 2014 15:07:35 CET
* Created By : chris
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTMPITHREADING_
#define _TESTMPITHREADING_

#include <cxxtest/TestSuite.h>

#ifdef BOOST_MPI
BOOST_SERIALIZATION_SHARED_PTR(TaskData)
BOOST_CLASS_EXPORT(TaskData)

BOOST_SERIALIZATION_SHARED_PTR(threading::daetask::DAEInputData< myMatrixType >)
BOOST_CLASS_EXPORT(threading::daetask::DAEInputData< myMatrixType >)

BOOST_SERIALIZATION_SHARED_PTR(threading::daetask::DAEOutputData< myMatrixType >)
BOOST_CLASS_EXPORT(threading::daetask::DAEOutputData< myMatrixType >)
#endif //BOOST_MPI

class TestMPI: public CxxTest::TestSuite
{
public:

    void testMPITestParallelResistors();
};


#endif /* _TESTMPITHREADING_ */

