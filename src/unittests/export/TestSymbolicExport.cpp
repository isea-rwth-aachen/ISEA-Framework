/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSymbolicExport.cpp
* Creation Date : 10-01-2017
* Last Modified : Di 20 Jun 2017 22:51:37 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestSymbolicExport.h"
#include <boost/spirit/include/qi_parse.hpp>

#include "../../export/symbolicExport.h"
#include "../../export/exportStructs/texExport.h"

#include <string>
extern template class symbolic::SymbolicExport< std::string::const_iterator, symbolic::TexExportStruct >;


void TestSymbolicExport::TestSymbolicExportCreation()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"ADD(3.0,MUL(1.0,ID10_ObjR))", "SUB(3.0,DIV(1.0,ID10_ObjR))",
                                        "SUB(3.0,POW(1.0,ID10_ObjR))", "ADD(3.0,NEG(ID10_ObjR))"};
    std::vector< std::string > result = {"( 3.0 + ( 1.0 \\cdot R_{10} ) )", "( 3.0 -  \\frac{1.0}{R_{10}}  )",
                                         "( 3.0 -  1.0^{R_{10}}  )", "( 3.0 + -R_{10} )"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicExport< iterator_type, TexExportStruct > sp;

    TS_ASSERT_EQUALS( input.size(), result.size() );
    for ( size_t i = 0; i < input.size(); ++i )
    {

        sp g;


        std::string str = input.at( i );

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        std::string emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        TS_ASSERT( r );
        TS_ASSERT_EQUALS( result.at( i ), emp );
    }
}

void TestSymbolicExport::TestSymbolicExportCreationName()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"ADD(1,ID10_ObjR)", "NEG(ID10_ObjR)", "ID10_ObjR", "ID10_ObjBase",
                                        "ID10_ObjTau",      "ID10_ObjC",      "-1.000",    " ID1_ObjR\n "};
    std::vector< std::string > result = {"( 1 + R_{10} )", "-R_{10}", "R_{10}", "Base_{10}",
                                         "\\tau_{10}",     "C_{10}",  "-1.000", "R_{1}"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicExport< iterator_type, TexExportStruct > sp;

    TS_ASSERT_EQUALS( input.size(), result.size() );
    for ( size_t i = 0; i < input.size(); ++i )
    {

        sp g;


        std::string str = input.at( i );

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        std::string emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        TS_ASSERT( r );
        TS_ASSERT_EQUALS( result.at( i ), emp );
    }
}
/*
void TestSymbolicExport::TestSymbolicExportCreationName()
{
    typedef ExportSystem< myMatrixType, symbolic::SymbolicExport< std::string::const_iterator, symbolic::StandardExport
>,
                          symbolic::SymbolicParser< std::string::const_iterator > > ES;
}
*/
