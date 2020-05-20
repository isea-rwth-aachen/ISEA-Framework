/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSpiceExport.h
* Creation Date : 14-01-2014
* Last Modified : Fr 21 Feb 2014 10:50:07 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSPICEEXPORT_
#define _TESTSPICEEXPORT_
#include <cxxtest/TestSuite.h>


/// This class is testing the creation of a Spice Network
class TestSpiceExport : public CxxTest::TestSuite
{
    public:
        void testStreamConstructor();
};


#endif /* _TESTSPICEEXPORT_ */
