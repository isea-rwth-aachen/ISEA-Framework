#ifndef _TESTMD5CHECKSUM_
#define _TESTMD5CHECKSUM_

#include <cxxtest/TestSuite.h>

class TestChecksum : public CxxTest::TestSuite
{
    public:
    void TestStringChecksum();
    void TestXMLChecksum();
};

#endif