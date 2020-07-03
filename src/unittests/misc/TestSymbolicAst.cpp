/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSymbolicAst.cpp
* Creation Date : 22-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestSymbolicAst.h"
#include "../../misc/symbolicAst.h"
#include "../../misc/ast_visitor/symbolicVisitor.h"
#include "../../misc/ast_visitor/optimizationVisitor.h"
#include "../../misc/ast_visitor/uniqueTokenizerVisitor.h"
#include "../../state/soc.h"
#include "../../state/thermal_state.h"
#include "../object/TestLookupObj.h"

void FinalizeOptimization( std::vector< std::string > &input, std::vector< std::string > &result, std::vector< size_t > &counts )
{
    using namespace symbolic;
    using boost::spirit::ascii::space;

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicAst< iterator_type > sp;

    TS_ASSERT_EQUALS( input.size(), result.size() );
    for ( size_t i = 0; i < input.size(); ++i )
    {
        sp g;

        std::string str = input.at( i );

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        symbolic::ast::Expression emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        TS_ASSERT( r );
        {

            std::stringstream firstRun;
            symbolic::ast::SymbolicVisitor str( &firstRun );
            boost::apply_visitor( str, emp );

            std::string lastValue = "";
            std::string currentValue = firstRun.str();

            symbolic::ast::Expression exp_memory = emp;
            size_t counter = 0;
            while ( lastValue != currentValue )
            {
                ++counter;
                lastValue = currentValue;
                symbolic::ast::OptimizationVisitor x;
                exp_memory = boost::apply_visitor( x, exp_memory );

                std::stringstream currentRun;
                symbolic::ast::SymbolicVisitor cur_V( &currentRun );
                boost::apply_visitor( cur_V, exp_memory );
                currentValue = currentRun.str();
            }
            TS_ASSERT_EQUALS( result.at( i ), currentValue );
            if ( result.at( i ) != currentValue )
            {
                std::cout << "input: " << input.at( i ) << std::endl;
                std::cout << "result: " << result.at( i ) << " current Value: " << currentValue << std::endl;
            }
            if ( !counts.empty() )
            {
                TS_ASSERT_EQUALS( counts.at( i ), counter );
                if ( counts.at( i ) != counter )
                {

                    std::cout << "input: " << input.at( i ) << std::endl;
                    std::cout << "result: " << result.at( i ) << " current Value: " << currentValue << std::endl;
                }
            }
        }
    }
}

void FinalizeOptimizationAdvanced( std::vector< std::string > &input, std::vector< std::string > &result,
                                   std::vector< size_t > &counts, typename symbolic::ast::ID_Map &map )
{
    using namespace symbolic;
    using boost::spirit::ascii::space;

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicAst< iterator_type > sp;

    TS_ASSERT_EQUALS( input.size(), result.size() );
    for ( size_t i = 0; i < input.size(); ++i )
    {
        sp g;

        std::string str = input.at( i );

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        symbolic::ast::Expression emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        TS_ASSERT( r );
        {

            std::stringstream firstRun;
            symbolic::ast::SymbolicVisitor str( &firstRun );
            boost::apply_visitor( str, emp );

            std::string lastValue = "";
            std::string currentValue = firstRun.str();

            symbolic::ast::Expression exp_memory = emp;
            size_t counter = 0;
            while ( lastValue != currentValue )
            {
                ++counter;
                lastValue = currentValue;
                symbolic::ast::OptimizationVisitor x( &map );
                exp_memory = boost::apply_visitor( x, exp_memory );

                std::stringstream currentRun;
                symbolic::ast::SymbolicVisitor cur_V( &currentRun );
                boost::apply_visitor( cur_V, exp_memory );
                currentValue = currentRun.str();
            }
            TS_ASSERT_EQUALS( result.at( i ), currentValue );
            if ( result.at( i ) != currentValue )
            {
                std::cout << "input: " << input.at( i ) << std::endl;
                std::cout << "result: " << result.at( i ) << " current Value: " << currentValue << std::endl;
            }
            if ( !counts.empty() )
            {
                TS_ASSERT_EQUALS( counts.at( i ), counter );
                if ( counts.at( i ) != counter )
                {

                    std::cout << "input: " << input.at( i ) << std::endl;
                    std::cout << "result: " << result.at( i ) << " current Value: " << currentValue << std::endl;
                }
            }
        }
    }
}


void TestSymbolicAst::TestSymbolicAstNew()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"SQRT(1)",                       "NEG(1)",       "TANH(1)",
                                        "NEG(ID10_LookupPhi2TauFactor)", "InputCurrent", "State1",
                                        "NEG(NEG(ID999_ObjMP))",         "ADD(1,1)",     "ADD(1,SUB(1,1))"};

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicAst< iterator_type > sp;

    //    TS_ASSERT_EQUALS( input.size(), result.size() );
    for ( size_t i = 0; i < input.size(); ++i )
    {
        sp g;


        std::string str = input.at( i );

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        symbolic::ast::Expression emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        TS_ASSERT( r );
    }
}

void TestSymbolicAst::TestSymbolicVisitor()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;
    std::vector< std::string > input = {"1",               "SQRT(1)",                       "NEG(1)",
                                        "TANH(1)",         "NEG(ID10_LookupPhi2TauFactor)", "InputCurrent",
                                        "State1",          "NEG(NEG(ID999_ObjMP))",         "ADD(1,1)",
                                        "ADD(1,SUB(1,1))", "SUB(11,SUB(12,13))",            "0.0001",
                                        "1e8"};
    std::vector< std::string > result = input;
    result.back() = "100000000";

    typedef std::string::const_iterator iterator_type;
    typedef SymbolicAst< iterator_type > sp;

    TS_ASSERT_EQUALS( input.size(), result.size() );
    for ( size_t i = 0; i < input.size(); ++i )
    {
        sp g;


        std::string str = input.at( i );

        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        symbolic::ast::Expression emp;
        bool r = phrase_parse( iter, end, g, space, emp );
        TS_ASSERT( r );
        {
            std::stringstream firstRun;
            symbolic::ast::SymbolicVisitor str( &firstRun );
            boost::apply_visitor( str, emp );
            /*
            std::cout << std::endl;
            std::cout << input.at( i ) << std::endl;
            */
            TS_ASSERT_EQUALS( result.at( i ), firstRun.str() );
        }
        // std::cout << emp.name << std::endl;
    }
}

void TestSymbolicAst::TestConstantFolding()
{
    std::vector< std::string > input = {
     "ADD(ADD(1,1),ADD(1,1))", "MUL(MUL(1,1),MUL(1,1))", "DIV(10,5)",             "SUB(10,5)", "SQRT(9)",
     "TANH(0)",                "SQRT(1)",                "POW(ADD(1,1),ADD(1,1))"

     /* "1",               "SQRT(1)",                       "NEG(1)",
                                    "TANH(1)",         "NEG(ID10_LookupPhi2TauFactor)", "InputCurrent",
                                    "State1",          "NEG(NEG(ID999_ObjMP))",         "ADD(1,1)",
                                    "ADD(1,SUB(1,1))", "SUB(11,SUB(12,13))",            "0.0001",
                                    "1e8"*/
    };
    std::vector< size_t > counts = {2, 2, 2, 2, 2, 2, 2, 2};
    std::vector< std::string > result = {"4", "1", "2", "5", "3", "0", "1", "4"};
    FinalizeOptimization( input, result, counts );
}


void TestSymbolicAst::TestRestructuringRules()
{
    using namespace symbolic;
    using boost::spirit::ascii::space;

    std::vector< std::string > label = {"R1",  "R2",  "R3",  "R4",  "R5",  "R6",  "R7",  "R8",  "R9",  "R10",
                                        "R11", "R12", "R13", "R14", "R15", "R16", "R17", "R18", "R19", "R20"};
    std::vector< std::string > input = {
     "ADD(1,1)",                                                  // R1
     "ADD(ID0_ObjR,1)",                                           // R2
     "MUL(1,1)",                                                  // R3
     "MUL(ID0_ObjR,2)",                                           // R4
     "SUB(1,1)",                                                  // R5
     "SUB(ID0_ObjR,1)",                                           // R6
     "ADD(TANH(ID0_ObjR),ADD(TANH(ID1_ObjR),TANH(ID2_ObjR)))",    // R7 <-- vielleicht überarbeiten, abweichung vom Buch
     "MUL(TANH(ID0_ObjR),MUL(TANH(ID1_ObjR),TANH(ID2_ObjR)))",    // R8 <-- vielleicht überarbeiten, abweichung vom Buch
     "ADD(ADD(1,ID0_ObjR),2)",                                    // R9
     "MUL(MUL(3,ID0_ObjR),2)",                                    // R10
     "MUL(ADD(3,ID0_ObjR),2)",                                    // R11
     "MUL(2,ADD(3,ID0_ObjR))",                                    // R12
     "MUL(ADD(ID0_ObjR,ID1_ObjR),2)",                             // R13
     "MUL(2,ADD(ID0_ObjR,ID1_ObjR))",                             // R14
     "MUL(SUB(ID0_ObjR,ID1_ObjR),2)",                             // R15
     "MUL(2,SUB(ID0_ObjR,ID1_ObjR))",                             // R16
     "MUL(ID0_ObjR,ADD(ID1_ObjR,ID2_ObjR))",                      // R17
     "MUL(ADD(ID1_ObjR,ID2_ObjR),ID0_ObjR)",                      // R18
     "MUL(ID0_ObjR,SUB(ID1_ObjR,ID2_ObjR))",                      // R19
     "MUL(SUB(ID1_ObjR,ID2_ObjR),ID0_ObjR)",                      // R20
     "DIV(10,5)",                                                 // RA1
     "ADD(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR)",                      // RA2
     "ADD(ID2_ObjR,DIV(ID0_ObjR,ID1_ObjR))",                      // RA3
     "SUB(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR)",                      // RA4
     "SUB(ID2_ObjR,DIV(ID0_ObjR,ID1_ObjR))",                      // RA5
     "DIV(ID0_ObjR,DIV(1,ID0_ObjR))",                             // RA6
     "DIV(ID0_ObjR,DIV(2,ID0_ObjR))",                             // RA7
     "ADD(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID4_ObjR))",        // RA8
     "SUB(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID4_ObjR))",        // RA9
     "ADD(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID1_ObjR))",        // RA10
     "SUB(DIV(ID0_ObjR,ID1_ObjR),DIV(ID3_ObjR,ID1_ObjR))",        // RA11

    };

    std::vector< std::string > result = {
     "2",                                                         // R1
     "ADD(1,ID0_ObjR)",                                           // R2
     "1",                                                         // R3
     "MUL(2,ID0_ObjR)",                                           // R4
     "0",                                                         // R5
     "ADD(-1,ID0_ObjR)",                                          // R6
     "ADD(ADD(TANH(ID0_ObjR),TANH(ID1_ObjR)),TANH(ID2_ObjR))",    // R7 <-- vielleicht überarbeiten, abweichung vom Buch
     "MUL(MUL(TANH(ID0_ObjR),TANH(ID1_ObjR)),TANH(ID2_ObjR))",    // R8 <-- vielleicht überarbeiten, abweichung vom Buch
     "ADD(3,ID0_ObjR)",                                           // R9
     "MUL(6,ID0_ObjR)",                                           // R10
     "ADD(6,MUL(2,ID0_ObjR))",                                    // R11
     "ADD(6,MUL(2,ID0_ObjR))",                                    // R12
     "ADD(MUL(2,ID0_ObjR),MUL(2,ID1_ObjR))",                      // R13
     "ADD(MUL(2,ID0_ObjR),MUL(2,ID1_ObjR))",                      // R14
     "SUB(MUL(2,ID0_ObjR),MUL(2,ID1_ObjR))",                      // R15
     "SUB(MUL(2,ID0_ObjR),MUL(2,ID1_ObjR))",                      // R16
     "ADD(MUL(ID0_ObjR,ID1_ObjR),MUL(ID0_ObjR,ID2_ObjR))",        // R17
     "ADD(MUL(ID0_ObjR,ID1_ObjR),MUL(ID0_ObjR,ID2_ObjR))",        // R18
     "SUB(MUL(ID0_ObjR,ID1_ObjR),MUL(ID0_ObjR,ID2_ObjR))",        // R19
     "SUB(MUL(ID0_ObjR,ID1_ObjR),MUL(ID0_ObjR,ID2_ObjR))",        // R20
     "2",                                                         // RA1
     "DIV(ADD(ID0_ObjR,MUL(ID2_ObjR,ID1_ObjR)),ID1_ObjR)",        // RA2
     "DIV(ADD(ID0_ObjR,MUL(ID2_ObjR,ID1_ObjR)),ID1_ObjR)",        // RA3
     "DIV(SUB(ID0_ObjR,MUL(ID2_ObjR,ID1_ObjR)),ID1_ObjR)",        // RA4
     "DIV(SUB(MUL(ID2_ObjR,ID1_ObjR),ID0_ObjR),ID1_ObjR)",        // RA5
     "MUL(ID0_ObjR,ID0_ObjR)",                                    // RA6
     "MUL(0.5,MUL(ID0_ObjR,ID0_ObjR))",                           // RA7
     "DIV(ADD(MUL(ID0_ObjR,ID4_ObjR),MUL(ID1_ObjR,ID3_ObjR)),MUL(ID1_ObjR,ID4_ObjR))",    // RA8
     "DIV(SUB(MUL(ID0_ObjR,ID4_ObjR),MUL(ID1_ObjR,ID3_ObjR)),MUL(ID1_ObjR,ID4_ObjR))",    // RA9
     "DIV(ADD(ID0_ObjR,ID3_ObjR),ID1_ObjR)",                                              // RA10
     "DIV(SUB(ID0_ObjR,ID3_ObjR),ID1_ObjR)",                                              // RA11

    };
    auto x = std::vector< size_t >();
    FinalizeOptimization( input, result, x );
}

void TestSymbolicAst::TestRestructuringSampleTree()
{
    std::vector< std::string > input = {
     // ADD(BASE_A,MUL(ADD(MUL(SUB(i,lo1),ADD(SUB(hi2,lo2),1)),SUB(j,lo2)),w))
     "ADD(3,MUL(ADD(MUL(SUB(29,7),ADD(SUB(23,13),1)),SUB(ID999_ObjMP,13)),ID1_ObjR))"};

    std::vector< std::string > result = {"ADD(3,ADD(MUL(229,ID1_ObjR),MUL(ID1_ObjR,ID999_ObjMP)))"};

    std::vector< size_t > counts;
    FinalizeOptimization( input, result, counts );
}

template < typename Op >
void CompareResultsBinary( const symbolic::ast::ID_Map &map, const std::string name, const std::vector< std::vector< double > > &Cd )
{
    auto it = map.find( name );
    TS_ASSERT( it != map.end() )

    auto lookup = dynamic_cast< object::LookupObj2dWithState< double > * >( it->second.get() )->GetLookup().GetLookup();

    for ( size_t i = 0; i < Cd.size(); ++i )
    {
        for ( size_t j = 0; j < Cd[i].size(); ++j )
        {
            TS_ASSERT_DELTA( Op()( Cd[i][j], Cd[i][j] ), lookup[i][j], 0.0001 );
        }
    }
}

template < typename Op >
void CompareResultsUnary( const symbolic::ast::ID_Map &map, const std::string name, const std::vector< std::vector< double > > &Cd )
{
    auto it = map.find( name );
    TS_ASSERT( it != map.end() )

    auto lookup = dynamic_cast< object::LookupObj2dWithState< double > * >( it->second.get() )->GetLookup().GetLookup();

    for ( size_t i = 0; i < Cd.size(); ++i )
    {
        for ( size_t j = 0; j < Cd[i].size(); ++j )
        {
            TS_ASSERT_DELTA( Op()( Cd[i][j] ), lookup[i][j], 0.0001 );
        }
    }
}

template < typename Op >
void CompareResultsUnary1d( const symbolic::ast::ID_Map &map, const std::string name, const std::vector< double > &valueVector )
{
    auto it = map.find( name );
    TS_ASSERT( it != map.end() );
    auto lookup = dynamic_cast< object::LookupObj1dWithState< double > * >( it->second.get() )->GetLookup().GetLookup();

    for ( size_t i = 0; i < valueVector.size(); ++i )
    {
        TS_ASSERT_DELTA( Op()( valueVector[i] ), lookup[i], 0.0001 );
    }
}


void TestSymbolicAst::TestRestructuringAstVariables()
{
    std::vector< std::string > input = {"ADD(ID0_ObjR,ID1_ObjR)", "ADD(ID2_ObjR,ID3_ObjR)", "SUB(ID2_ObjR,ID3_ObjR)",
                                        "TANH(ID0_ObjR)",         "SQRT(ID1_ObjR)",         "NEG(ID1_ObjR)",
                                        "NEG(ID2_ObjR)",          "NEG(ID4_ObjR)",          "TANH(ID2_ObjR)",
                                        "SQRT(ID2_ObjR)",         "TANH(ID4_ObjR)",         "SQRT(ID4_ObjR)"};

    std::vector< std::string > result = {"3",                   "ID5_Merge",          "ID6_Merge",
                                         "0.76159415595576485", "1.4142135623730951", "-2",
                                         "ID7_Merge",           "ID8_Merge",          "TANH(ID2_ObjR)",
                                         "SQRT(ID2_ObjR)",      "TANH(ID4_ObjR)",     "SQRT(ID4_ObjR)"};
    auto x = std::vector< size_t >();
    typename symbolic::ast::ID_Map map;

    map["ID0_ObjR"] = boost::make_shared< object::ConstObj< double > >( 1.0 );
    map["ID1_ObjR"] = boost::make_shared< object::ConstObj< double > >( 2.0 );

    double capacity = 20.0;
    double status = 20.0;

    std::vector< double > Temp = CreateTemp();
    std::vector< double > SOC = CreateSOC();
    boost::shared_ptr< state::Soc > testSoc( new state::Soc( capacity /* [Ah] */, capacity, status /*[%]*/ ) );
    boost::shared_ptr< state::ThermalState< double > > testTemp( new state::ThermalState< double >( 23.0 ) );

    std::vector< double > m1 = Createm1();
    std::vector< double > f1_m = Createf1();
    std::vector< std::vector< double > > Cd = CreateCd();

    map["ID2_ObjR"] = boost::make_shared< object::LookupObj2dWithState< double > >( Cd, Temp, SOC, testTemp, testSoc );
    map["ID3_ObjR"] = boost::make_shared< object::LookupObj2dWithState< double > >( Cd, Temp, SOC, testTemp, testSoc );
    map["ID4_ObjR"] = boost::make_shared< object::LookupObj1dWithState< double > >( f1_m, m1, testSoc );


    FinalizeOptimizationAdvanced( input, result, x, map );
    CompareResultsBinary< std::plus< double > >( map, "ID5_Merge", Cd );
    CompareResultsBinary< std::minus< double > >( map, "ID6_Merge", Cd );
    CompareResultsUnary< symbolic::ast::tag::Neg_Functor >( map, "ID7_Merge", Cd );
    CompareResultsUnary1d< symbolic::ast::tag::Neg_Functor >( map, "ID8_Merge", f1_m );
}
