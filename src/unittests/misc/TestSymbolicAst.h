/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSymbolicAst.h
* Creation Date : 22-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSYMBOLICAST_
#define _TESTSYMBOLICAST_
#include <cxxtest/TestSuite.h>
void FinalizeOptimization( std::vector< std::string > &input, std::vector< std::string > &result, std::vector< size_t > &counts );

class TestSymbolicAst : public CxxTest::TestSuite
{
    public:
    void TestSymbolicAstNew();
    void TestSymbolicVisitor();
    void TestConstantFolding();
    void TestRestructuringAstVariables();
    void TestRestructuringRules();         // Advanced Compiler Design & Implementation Chapter 12.3
    void TestRestructuringSampleTree();    // Convert variables to other variables
};
#endif /* _TESTSYMBOLICAST_ */
