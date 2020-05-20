/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSymbolicParser.h
* Creation Date : 04-01-2017
* Last Modified : Mo 16 Jan 2017 13:44:52 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSYMBOLICPARSER_
#define _TESTSYMBOLICPARSER_
#include <cxxtest/TestSuite.h>
class TestSymbolicParser : public CxxTest::TestSuite
{
    public:
    void TestSymbolicNewTest();
    void TestSymbolicParserGrammar();
    void TestSymbolicParserAdder();
    void TestSymbolicParserNegation();
    void TestSymbolicExpansion();
    void TestSymbolicParserDivision2Mulitplication();
    void TestSymbolicParserSubtraction();
    void TestSymbolicParserAddDivDiv();
    void TestSymbolicParserMulNeg();
    void estSymbolicParserLONGSTRING();
    void TestSymbolicAllRules();
    void TestSymbolicFinalOptimzation();
    void TestSymbolicSearchForError();
};

#endif /* _TESTSYMBOLICPARSER_ */
