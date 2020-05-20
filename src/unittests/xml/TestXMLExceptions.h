/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestZarc.h
* Creation Date : 11-11-2012
* Last Modified : Fr 21 Feb 2014 12:38:33 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTXMlEXCEPTIONS_
#define _TESTXMlEXCEPTIONS_
#include <cxxtest/TestSuite.h>
#include <cstring>

class TestXMLExceptions: public CxxTest::TestSuite
{
public:

    void testXMLParserWrongFilename();
    void testXMLParserNullFilename();
    void testXMLParserWrongMemAddr();
    void testXMLParserNullMemAddr();
    void testXMLNotMatchingElements();
    void testXMLNoRootElement();
    void testXMLNoReferencedElement();
    void testXMLCacheRefInstance();
    void testXMLClassNotFound();
    void testXMLAttributeNotFound();
    void testXMLElementContentNotReadable();
    void testXMLCatchNegativeValue();

};
#endif /* _TESTXMlEXCEPTIONS_ */
