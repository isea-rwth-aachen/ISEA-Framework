/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSymbolicParser.cpp
* Creation Date : 04-01-2017
* Last Modified : Mo 16 Jan 2017 16:44:01 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestSymbolicParser.h"
#include "../../misc/symbolic.h"
#include "../../misc/symbolicParser.h"
#include <string>
extern template struct symbolic::SymbolicParser< std::string::const_iterator >;

void TestSymbolicParser::TestSymbolicNewTest()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"SUB(ADD(ID2_ObjR,ID1_ObjR),ID2_ObjR)",
                                        "SUB(ADD(ADD(ID2_ObjR,ID1_ObjR),ID1_ObjR),ID2_ObjR)",
                                        "SUB(ADD(ID1_ObjR,ID2_ObjR),ID2_ObjR)",
                                        "SUB(ADD(ADD(ID1_ObjR,ID2_ObjR),ID1_ObjR),ID2_ObjR)"};
    std::vector< std::string > result = {"ID1_ObjR", "ADD(ID1_ObjR,ID1_ObjR)", "ID1_ObjR", "ADD(ID1_ObjR,ID1_ObjR)"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

void TestSymbolicParser::TestSymbolicParserGrammar()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;

    // Symbolic< OutType > symbol1( "MUL(SQRT(A),ADD(A,SUB(C,D)))" );
    Symbolic< OutType > symbol1( "ADD(3.0,MUL(1.0,ID10_ObjR))" );

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;
    sp g;


    std::string str( (const char *)symbol1.GetString() );

    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    std::string emp;
    bool r = phrase_parse( iter, end, g, space, emp );
    TS_ASSERT( r );
    TS_ASSERT_EQUALS( std::string( "ADD(3,ID10_ObjR)" ), emp );
}


void TestSymbolicParser::TestSymbolicExpansion()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"ADD(DIV(ID1_ObjR,ID2_ObjR),DIV(ID3_ObjR,ID4_ObjR))",
                                        "SUB(DIV(ID1_ObjR,ID2_ObjR),DIV(ID3_ObjR,ID4_ObjR))",
                                        "ADD(DIV(ID1_ObjR,ID2_ObjR),ID3_ObjR)",
                                        "SUB(ID7_ObjR,DIV(MUL(ID8_ObjR,ID1_ObjR),ID2_ObjR))",
                                        "SUB(DIV(ID7_ObjR,ID8_ObjR),ID9_ObjR)"};
    std::vector< std::string > result = {
     "DIV(ADD(MUL(ID1_ObjR,ID4_ObjR),MUL(ID2_ObjR,ID3_ObjR)),MUL(ID2_ObjR,ID4_ObjR))",
     "DIV(SUB(MUL(ID1_ObjR,ID4_ObjR),MUL(ID2_ObjR,ID3_ObjR)),MUL(ID2_ObjR,ID4_ObjR))",
     "DIV(ADD(ID1_ObjR,MUL(ID2_ObjR,ID3_ObjR)),ID2_ObjR)",
     "DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR)",
     "DIV(SUB(ID7_ObjR,MUL(ID9_ObjR,ID8_ObjR)),ID8_ObjR)"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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
void TestSymbolicParser::TestSymbolicParserAdder()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"ADD(3.0,5.0)", "SUB(3.0,5.0)",    "MUL(3.0,5.0)",
                                        "DIV(6.0,3.0)", "ADD(1,ADD(2,3))", "ADD(SUB(2.0,ID10_ObjR),1.0)"};
    std::vector< std::string > result = {"8", "-2", "15", "2", "ADD(1,5)", "ADD(SUB(2,ID10_ObjR),1)"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

void TestSymbolicParser::TestSymbolicParserNegation()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"NEG(NEG(1))", "NEG(NEG(ADD(1,2)))", "SQRT(10.0)",
                                        "ADD(NEG(ID8_ObjR),ID7_ObjR)"};
    std::vector< std::string > result = {"1", "3", "3.1622776601683795", "SUB(ID7_ObjR,ID8_ObjR)"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

void TestSymbolicParser::TestSymbolicParserAddDivDiv()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"ADD(DIV(ID3_ObjR,ID1_ObjR),DIV(ID2_ObjR,ID1_ObjR))"};    // DIV( MUL(1,1) , 2)
    std::vector< std::string > result = {"DIV(ADD(ID3_ObjR,ID2_ObjR),ID1_ObjR)"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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


void TestSymbolicParser::TestSymbolicParserMulNeg()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"MUL(NEG(ID1_ObjR),ID2_ObjR)", "DIV(NEG(ID1_ObjR),ID2_ObjR)",
                                        "MUL(ID2_ObjR,NEG(ID1_ObjR))", "DIV(NEG(ID1_ObjR),ID2_ObjR)"};
    std::vector< std::string > result = {"NEG(MUL(ID1_ObjR,ID2_ObjR))", "NEG(DIV(ID1_ObjR,ID2_ObjR))",
                                         "NEG(MUL(ID2_ObjR,ID1_ObjR))", "NEG(DIV(ID1_ObjR,ID2_ObjR))"};


    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

void TestSymbolicParser::TestSymbolicParserDivision2Mulitplication()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"DIV(1,DIV(2,MUL(1,1)))", "DIV(MUL(1.0,1.000000),2)"};    // DIV( MUL(1,1) , 2)
    std::vector< std::string > result = {"DIV(MUL(1,1),2)", "DIV(1,2)"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

void TestSymbolicParser::TestSymbolicSearchForError()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {
     "MUL(ID8_ObjR,SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR)))",    // ID_8*( (ID_6*ID_2)-(ID_5*ID_3))
     "DIV(MUL(ID8_ObjR,SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR)",    // ID_8*(
                                                                                          // (ID_6*ID_2)-(ID_5*ID_3)) /
                                                                                          // ID_2
     "MUL(DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_"
     "ObjR)))",
     "DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR)",
     "SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))",
     "DIV(MUL(DIV(SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR)),ID2_ObjR),SUB(MUL(ID4_ObjR,ID2_"
     "ObjR),MUL(ID5_ObjR,ID1_ObjR))),ID2_ObjR)"


     /*

NEG(DIV(MUL(DIV(MUL(SUB(DIV(MUL(DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR),DIV(MUL(DIV(SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR)),ID2_ObjR),SUB(MUL(ID4_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID1_ObjR))),ID2_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),DIV(MUL(ID5_ObjR,SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR))),ID2_ObjR)),DIV(MUL(SUB(DIV(MUL(DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR),DIV(MUL(DIV(SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR)),ID2_ObjR),SUB(MUL(ID4_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID1_ObjR))),ID2_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR)))))*/

     /*
ADD(DIV(MUL(ID8_ObjR,SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR),NEG(DIV(MUL(DIV(MUL(SUB(DIV(MUL(DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR),DIV(MUL(DIV(SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR)),ID2_ObjR),SUB(MUL(ID4_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID1_ObjR))),ID2_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),DIV(MUL(ID5_ObjR,SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR))),ID2_ObjR)),DIV(MUL(SUB(DIV(MUL(DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR),DIV(MUL(DIV(SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR)),ID2_ObjR),SUB(MUL(ID4_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID1_ObjR))),ID2_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))))))",
  */
     /* ORIGINIAL
     "ADD(DIV(MUL(ID8_ObjR,SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR),NEG(DIV(MUL(DIV(MUL(SUB(DIV("
     "MUL(DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_"
     "ObjR))),ID2_ObjR),DIV(MUL(DIV(SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR)),ID2_ObjR),SUB(MUL(ID4_ObjR,ID2_"
     "ObjR),MUL(ID5_ObjR,ID1_ObjR))),ID2_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),DIV(MUL("
     "ID5_ObjR,SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR))),ID2_ObjR)),DIV(MUL(SUB(DIV(MUL(DIV(SUB(MUL(ID7_ObjR,"
     "ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR),DIV("
     "MUL(DIV(SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR)),ID2_ObjR),SUB(MUL(ID4_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID1_"
     "ObjR))),ID2_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))))))"*/
    };
    std::vector< std::string > result = {
     "MUL(ID8_ObjR,SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR)))",
     "DIV(MUL(ID8_ObjR,SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))),ID2_ObjR)",    // ID_8*(
                                                                                          // (ID_6*ID_2)-(ID_5*ID_3)) /
                                                                                          // ID_2
                                                                                          //"MUL(DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR)))",
                                                                                          // DIV(MUL(ID2_ObjR,SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR))),SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR)))

     "DIV(MUL(SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR)),SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR))),"
     "ID2_ObjR)",
     "DIV(SUB(MUL(ID7_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID1_ObjR)),ID2_ObjR)",
     "SUB(MUL(ID6_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID3_ObjR))",
     "DIV(DIV(MUL(SUB(MUL(ID4_ObjR,ID2_ObjR),MUL(ID5_ObjR,ID1_ObjR)),SUB(MUL(ID9_ObjR,ID2_ObjR),MUL(ID8_ObjR,ID3_ObjR))"
     "),ID2_ObjR),ID2_ObjR)"


    };

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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


void TestSymbolicParser::TestSymbolicParserSubtraction()
{

    /*
    using namespace symbolic;
    Symbolic< OutType > symbolresult, symbol1( "A" ), symbolB( "B" ), symbolC( "C" ), symbolD( "D" );
    symbolresult = symbol1 / ( symbolB + symbolC - symbolB + symbolD );
    // DIV(A,ADD(SUB(ADD(B,C),B),D) --> DIV(A,ADD(C,D))
    */

    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"SUB(ID10_ObjR,ID10_ObjR)",
                                        "ADD(SUB(ID10_ObjR,ID10_ObjR),1)",
                                        "ADD(1,SUB(ID10_ObjR,1))",
                                        "SUB(SUB(ID10_ObjR,1),ID10_ObjR)",
                                        "SUB(SUB(SUB(ID10_ObjR,1),ID1_ObjR),ID10_ObjR)",

                                        //"ADD(1,ADD(SUB(ID10_ObjR,1)))" // TODO
                                        "SUB(ADD(ID10_ObjR,1),ID10_ObjR)"};    // DIV( MUL(1,1) , 2)
    std::vector< std::string > result = {"0", "ADD(0,1)", "ID10_ObjR", "NEG(1)", "SUB(NEG(1),ID1_ObjR)", "1"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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


void TestSymbolicParser::estSymbolicParserLONGSTRING()
{
    /*ADD(ADD(MUL(DIV(NEG(ADD(MUL(ID8_ObjR,DIV(1.000000,SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)))))),MUL(ID5_ObjR,DIV(NEG(MUL(SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),DIV(1.000000,SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR))))))),SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))))),ID2_ObjR),ID10_ObjR),MUL(DIV(SUB(1.000000,ADD(MUL(ID8_ObjR,DIV(NEG(ADD(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),NEG(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)))))),SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)))))),MUL(ID5_ObjR,DIV(SUB(NEG(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR))),MUL(SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),DIV(NEG(ADD(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),NEG(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)))))),SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR))))))),SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))))),ID2_ObjR),ID11_ObjR)),MUL(DIV(NEG(ADD(MUL(ID8_ObjR,DIV(NEG(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR))))),SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)))))),MUL(ID5_ObjR,DIV(SUB(1.000000,MUL(SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),DIV(NEG(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR))))),SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR))))))),SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))))),ID2_ObjR),ID12_ObjR))*/

    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {
     "ADD(ADD(MUL(DIV(NEG(ADD(MUL(ID8_ObjR,DIV(1.000000,SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_"
     "ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL("
     "MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)"
     "))))),MUL(ID5_ObjR,DIV(NEG(MUL(SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),DIV(1.000000,SUB("
     "SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1."
     "000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB("
     "ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR))))))),SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_"
     "ObjR)),ID5_ObjR)))))),ID2_ObjR),ID10_ObjR),MUL(DIV(SUB(1.000000,ADD(MUL(ID8_ObjR,DIV(NEG(ADD(MUL(ID1_ObjR,DIV(1."
     "000000,ID2_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_"
     "ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),NEG(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)))))),SUB(SUB("
     "ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,"
     "ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_"
     "ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)))))),MUL(ID5_ObjR,DIV(SUB(NEG(MUL(ID3_ObjR,DIV(1.000000,"
     "ID2_ObjR))),MUL(SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),DIV(NEG(ADD(MUL(ID1_ObjR,DIV(1."
     "000000,ID2_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_"
     "ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),NEG(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)))))),SUB(SUB("
     "ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,"
     "ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB(ID9_"
     "ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR))))))),SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_"
     "ObjR)),ID5_ObjR)))))),ID2_ObjR),ID11_ObjR)),MUL(DIV(NEG(ADD(MUL(ID8_ObjR,DIV(NEG(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_"
     "ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_"
     "ObjR))))),SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_"
     "ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_"
     "ObjR)))),SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)))))),MUL(ID5_ObjR,DIV(SUB(1.000000,MUL("
     "SUB(ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),DIV(NEG(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1."
     "000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR))))),"
     "SUB(SUB(ID7_ObjR,MUL(MUL(ID1_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR)),MUL(MUL(SUB(ID4_ObjR,MUL(MUL(ID1_ObjR,DIV(1."
     "000000,ID2_ObjR)),ID5_ObjR)),DIV(1.000000,SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID5_ObjR)))),SUB("
     "ID9_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_ObjR)),ID8_ObjR))))))),SUB(ID6_ObjR,MUL(MUL(ID3_ObjR,DIV(1.000000,ID2_"
     "ObjR)),ID5_ObjR)))))),ID2_ObjR),ID12_ObjR))"};
    std::vector< std::string > result = input;

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

void TestSymbolicParser::TestSymbolicAllRules()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {
     "DIV(ID1_ObjR,DIV(ID0_ObjR,ID2_ObjR))",                  // division2multiplication_1
     "DIV(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR)",                  // division2multiplication_2
     "DIV(NEG(ID0_ObjR),ID1_ObjR)",                           // mulNeg1
     "MUL(NEG(ID0_ObjR),ID1_ObjR)",                           // mulNeg2
     "MUL(ID0_ObjR,NEG(ID1_ObjR))",                           // mulNeg3
     "DIV(ID0_ObjR,NEG(ID1_ObjR))",                           // mulNeg4
     "ADD(DIV(ID0_ObjR,ID1_ObjR),DIV(ID2_ObjR,ID1_ObjR))",    // addDivDiv
     "MUL(ID0_ObjR,DIV(ID1_ObjR,ID2_ObjR))",                  // mulDivReorder1
     "MUL(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR)",                  // mulDivReorder2
     "NEG(NEG(ID0_ObjR))",                                    // negationsimplify
     "ADD(NEG(ID0_ObjR),ID1_ObjR)",                           // sum2Sub
     "SUB(ID0_ObjR,NEG(ID1_ObjR))",                           // sub2Add
     "ADD(DIV(ID0_ObjR,ID1_ObjR),DIV(ID2_ObjR,ID3_ObjR))",    // expand1
     "SUB(DIV(ID0_ObjR,ID1_ObjR),DIV(ID2_ObjR,ID3_ObjR))",    // expand2
     "ADD(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR)",                  // expand3_1
     "ADD(ID0_ObjR,DIV(ID1_ObjR,ID2_ObjR))",                  // expand3_2
     "SUB(ID0_ObjR,DIV(ID1_ObjR,ID2_ObjR))",                  // expand3_3
     "SUB(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR)",                  // expand3_4
     "SQRT(9)",                                               // sqrtsimplify
     "DIV(10,5)",                                             // divisionimplify
     "MUL(2,5)",                                              // multiplicationsimplify
     "MUL(ID1_ObjR,1)",                                       // mulConstantFolding1
     "MUL(1,ID1_ObjR)",                                       // mulConstantFolding2
     "SUB(1,1)",                                              // subtractionsimplify
     "ADD(1,1)",                                              // additionsimplify
     "ADD(SUB(1,ID1_ObjR),ID1_ObjR)",                         // getAddtractor
     "ADD(ID1_ObjR,SUB(ID0_ObjR,ID1_ObjR))",                  // ADD1
     "SUB(ID1_ObjR,ID1_ObjR)",                                // SUB0
     "SUB(ADD(ID1_ObjR,ID0_ObjR),ID1_ObjR)",                  // SUB1_1
     "SUB(ADD(ADD(ID1_ObjR,ID0_ObjR),ID2_ObjR),ID1_ObjR)",    // SUB1_2
     "SUB(ADD(ID0_ObjR,ID1_ObjR),ID1_ObjR)",                  // SUB2_1
     "SUB(ADD(ID0_ObjR,ADD(ID2_ObjR,ID1_ObjR)),ID1_ObjR)",    // SUB2_2 TODO
     "SUB(SUB(ID1_ObjR,ID0_ObjR),ID1_ObjR)",                  // SUB3_1
     "SUB(SUB(SUB(ID1_ObjR,ID2_ObjR),ID0_ObjR),ID1_ObjR)",    // SUB3_2
     "SUB(NEG(ID1_ObjR),ID2_ObjR)",
     "ADD(ID110_ObjR,NEG(ID3_ObjR))"    // addNeg

    };
    std::vector< std::string > result = {
     "DIV(MUL(ID1_ObjR,ID2_ObjR),ID0_ObjR)", "DIV(ID0_ObjR,MUL(ID1_ObjR,ID2_ObjR))", "NEG(DIV(ID0_ObjR,ID1_ObjR))",
     "NEG(MUL(ID0_ObjR,ID1_ObjR))", "NEG(MUL(ID0_ObjR,ID1_ObjR))", "NEG(DIV(ID0_ObjR,ID1_ObjR))",
     "DIV(ADD(ID0_ObjR,ID2_ObjR),ID1_ObjR)", "DIV(MUL(ID0_ObjR,ID1_ObjR),ID2_ObjR)",
     "DIV(MUL(ID2_ObjR,ID0_ObjR),ID1_ObjR)", "ID0_ObjR", "SUB(ID1_ObjR,ID0_ObjR)", "ADD(ID0_ObjR,ID1_ObjR)",
     "DIV(ADD(MUL(ID0_ObjR,ID3_ObjR),MUL(ID1_ObjR,ID2_ObjR)),MUL(ID1_ObjR,ID3_ObjR))",
     "DIV(SUB(MUL(ID0_ObjR,ID3_ObjR),MUL(ID1_ObjR,ID2_ObjR)),MUL(ID1_ObjR,ID3_ObjR))",
     "DIV(ADD(ID0_ObjR,MUL(ID1_ObjR,ID2_ObjR)),ID1_ObjR)", "DIV(ADD(MUL(ID0_ObjR,ID2_ObjR),ID1_ObjR),ID2_ObjR)",
     "DIV(SUB(MUL(ID0_ObjR,ID2_ObjR),ID1_ObjR),ID2_ObjR)", "DIV(SUB(ID0_ObjR,MUL(ID2_ObjR,ID1_ObjR)),ID1_ObjR)", "3",
     "2", "10", "ID1_ObjR", "ID1_ObjR", "0", "2", "1", "ID0_ObjR", "0", "ID0_ObjR", "ADD(ID0_ObjR,ID2_ObjR)",
     "ID0_ObjR", "ADD(ID0_ObjR,ID2_ObjR)", "NEG(ID0_ObjR)", "SUB(NEG(ID2_ObjR),ID0_ObjR)",
     "NEG(ADD(ID1_ObjR,ID2_ObjR))",
     "SUB(ID110_ObjR,ID3_ObjR)"    // addNeg
    };

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

void TestSymbolicParser::TestSymbolicFinalOptimzation()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"MUL(NEG(1),ID1_ObjR)", "DIV(MUL(-1,ID1_ObjR),ID2_ObjR)"};
    std::vector< std::string > result = {"NEG(ID1_ObjR)", "NEG(DIV(ID1_ObjR,ID2_ObjR))"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicParser< iterator_type > sp;

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

        while ( str != emp )
        {
            str = emp;
            emp = "";
            iter = str.begin();
            end = str.end();

            r = phrase_parse( iter, end, g, space, emp );
            TS_ASSERT( r );
        }
        TS_ASSERT_EQUALS( result.at( i ), emp );
    }
}
