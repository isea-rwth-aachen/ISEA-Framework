/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestXML.h
* Creation Date : 11-11-2012
* Last Modified : Di 16 Feb 2016 14:48:53 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTELECTRICALSTUFF_
#define _TESTELECTRICALSTUFF_
#include <cxxtest/TestSuite.h>

namespace electrical
{
template < typename T >
class TwoPort;
}

class TestXML : public CxxTest::TestSuite
{
    public:
    void TestXmlGetAttributeDoubleGetUnrefrencedChild();
    void testXMLCacheRefInstance();
    void testXMLTestConfigfile();
    void testXMLReferencedCapacitance();
    void testXMLParallelPortChildren();
    void testXMLElectricalElementOhmicConst();
    void testXMLElectricalElementOhmic1DLookup();
    void testXMLParallelRCTestSimplificationFastRCToOhmicResistance();
    void testXMLWarburgTestReductionOfOhmicResistance();
    void testXMLWarburgTestCreationWithFullRCImplementation();
    void testXMLWarburgTestWrongSampleRate();
    void testXMLWarburgCothTestCreationWithFullRCImplementation();
    void testXMLWarburgCothTestCreationWithFullRCImplementationAndLookup1D();
    void testXMLWarburgCothTestCreationWithFullRCImplementationAndLookup2D();
    void testXMLWarburgCotanhTestReductionOfOhmicResistance();
    void testXMLWarburgCotanhTestWrongSampleRate();
    void testXMLWarburgCotanhTestSimplificationOfRCImpedance();
    void testXMLWarburgCotanhTestWithAndWithoutCapacity();
    void testXMLWarburgTanhTestSimplificationOfRCImpedance();
    void testXMLElectricalElementOhmic2DLookup();
    void testXMLTemperatureOperation();
    void testXMLSocOperations();
    void testXMLZarcOperations();
    void testXMLCellelementOperations();
    void testNegativeSampleRateForZarc();

    private:
    template < typename T >
    bool CheckTwoPortTypeAndValue( electrical::TwoPort< T > *testedTwoPort, const char *expectedType, double expectedValue );
};
#endif /* _TESTELECTRICALSTUFF_ */
