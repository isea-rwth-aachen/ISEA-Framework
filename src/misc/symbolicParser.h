/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolicParser.h
* Creation Date : 04-01-2017
* Last Modified : Di 20 Jun 2017 22:55:23 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLICPARSER_
#define _SYMBOLICPARSER_
//#define BOOST_SPIRIT_DEBUG
#ifndef _DS1006

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_hold.hpp>

#include <boost/phoenix/phoenix.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <cmath>

namespace symbolic
{
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;


void char2doubleFct( const double j, std::string &returnvalue );
void MulConstantFolding2( const double j, std::string &operand, std::string &returnvalue );
void MulConstantFolding( std::string &operand, const double j, std::string &returnvalue );

template < class Arithmetictype >
void Arithmetic( const double i, const double j, std::string &returnvalue )
{
    returnvalue = boost::lexical_cast< std::string >( Arithmetictype()( i, j ) );
}

template <>
void Arithmetic< std::multiplies< const double > >( const double i, const double j, std::string &returnvalue );

void Sqrt( const double i, std::string &returnvalue );

template < typename Iterator >
struct Rulez
{
    Rulez()
    {
        using qi::string;
        using qi::double_;
        using qi::int_;
        using qi::lexeme;
        using ascii::char_;
        using boost::spirit::ascii::digit;
        using boost::spirit::hold;
        using boost::spirit::omit;
        using boost::spirit::attr;

        // operator ::= add | mul | prod | div | sqrt | pow
        // zahl    ::= int | double
        // operand ::= zahl | operator
        // group ::=  operator '(' (operand, operand) | operand ')'

        // operator = add | mul | prod | div | sqrt | pow;
        // zahl    = int | double;
        // operand = zahl | group;
        // group =  operator'(' (operand ',' operand) | operand ')';
        AddSubOperator %= string( "ADD" ) | string( "SUB" );
        MulDivOperator %= string( "MUL" ) | string( "DIV" );
        doubleOperator %= AddSubOperator | MulDivOperator | string( "POW" );
        singleOperator %= string( "SQRT" ) | string( "NEG" ) | string( "TANH" );

        zahl %= omit[double_[qi::_a = qi::_1]][phoenix::bind( &char2doubleFct, qi::_a, qi::_b )] >> attr( qi::_val + qi::_b );

        twoPortSuffix %= string( "_LookupPhi2RFactorAussen" ) | string( "_LookupPhi2RFactorInnen" ) |
                         string( "_LookupPhi2TauFactor" ) | string( "_ObjCT" ) | string( "_ObjR" ) | string( "_ObjC" ) |
                         string( "_ObjV" ) | string( "_ObjTau" ) | string( "_ObjBase" ) | string( "_ObjMP" );
        twoPortID %= ( string( "ID" ) >> +digit ) > twoPortSuffix;
        inputCurrent %= string( "InputCurrent" );
        state %= string( "State" ) > +digit;

        // A + (-B) = A - B
        addNeg %= omit["ADD(" >> group[qi::_a = qi::_1] >> ",NEG(" > group[qi::_b = qi::_1] > "))"] >
                  attr( qi::_val + "SUB(" + qi::_a + "," + qi::_b + ")" );
        // ~~~~~ _a1   ~~~  a2  ~  _a1
        // ADD ( OP_A, SUB( OP_B, OP_A))  //

        // inlcuded by getSubtractor
        // -33  B-(C-D) -B = -C-D
        SUB3_2 %= hold[string( "SUB(" ) >> SUB3_1( qi::_r1 ) >> string( "," ) >> group >> string( ")" )];

        // inlcuded by getSubtractor
        // -32  B-C -B = -C
        SUB3_1 %= hold["SUB(" >> omit[string( qi::_r1 )] >> "," >> attr( qi::_val + "NEG(" ) >> group >> string( ")" )] |
                  hold[SUB3_2( qi::_r1 )] /*| hold[SUB1_1( qi::_r1 )]*/;    // >> string(  ) >>

        // inlcuded by getSubtractor
        SUB3 %= omit["SUB("] >> SUB3_1( qi::_r1 ) >> omit["," >> group >> ")"];


        // inlcuded by getSubtractor
        // -31 A + B + X -B = A
        SUB2_2 %= hold[string( "ADD(" ) >> group >> string( "," ) >> SUB2_1( qi::_r1 ) >> string( ")" )];
        // inlcuded by getSubtractor
        // -30 A + B -B = A
        SUB2_1 %= hold["ADD(" >> group >> "," >> omit[string( qi::_r1 )] >> ")"] |
                  hold[SUB2_2( qi::_r1 )] /*| hold[SUB1_1( qi::_r1 )]*/;    // >> string(  ) >>

        // inlcuded by getSubtractor
        SUB2 %= omit["SUB("] >> SUB2_1( qi::_r1 ) >> omit["," >> group >> ")"];


        // inlcuded by getSubtractor
        subtractionRule1 %= attr( qi::_val + "0" );

        //-29  A + X + Y - A = X
        // inlcuded by getSubtractor
        SUB1_2 %= hold[string( "ADD(" ) >> SUB1_1( qi::_r1 ) >> string( "," ) >> group >> string( ")" )];

        //-29  A+X-A = X
        SUB1_1 %= hold["ADD(" >> omit[string( qi::_r1 )] >> "," >> group >> ")"] | hold[SUB1_2( qi::_r1 )] |
                  hold[SUB2_1( qi::_r1 )];    // >> string(  ) >>


        SUB1 %= omit["SUB("] >> SUB1_1( qi::_r1 ) >> omit["," >> group >> ")"];

        // inlcuded by getSubtractor
        //- 28 A-A
        SUB0 %= omit["SUB(" >> omit[string( qi::_r1 )] >> "," >> omit[string( qi::_r1 )] >> subtractionRule1 >> ")"];

        //- A-B
        getSubtractor %= &( omit["SUB("] >> group >> omit["," >> group[qi::_a = qi::_1] >> ")"] ) >>
                         ( hold[SUB1( qi::_a )] | hold[SUB2( qi::_a )] | hold[SUB0( qi::_a )] | hold[SUB3( qi::_a )] );


        /*        getMultiplier %= &( omit["MUL("] >> group >> omit["," >> group[qi::_a = qi::_1] >> ")"] ) >>
                                 ( hold[SUB1( qi::_a )] | hold[SUB2( qi::_a )] | hold[SUB0( qi::_a )] | hold[SUB3(
           qi::_a )] );

                                 */

        // omit["SUB("] >> group >> "," >> omit[string( qi::_r1 ) | omit[string( qi::_r2 )] >>
        // subtractionRule1 >> ")"];

        // SUB( A, B)
        // ADD(A,SUB( B, A)) --> B
        // r1 := A, r2 := SUB(B,A)

        // ADD(1,SUB( ID10_ObjR, 1))
        //"ADD( SUB(A, B), B)" | ADD(B, SUB(A,B))
        //"ADD( ADD(SUB(A, B),C), B)" | ADD(B, ADD(SUB(A,B), C))


        // inlcuded by getAddtractor
        //- 26 A+B-A = B
        ADD1 %= "ADD(" >> omit[group[qi::_a = qi::_1]] >> ",SUB(" >> group >> "," >> omit[string( qi::_a )] >> "))";

        //- 25 A-B+B = A
        getAddtractor %=
         hold["ADD(SUB(" >> group >> "," >> omit[group[qi::_a = qi::_1]] >> ")," >> omit[string( qi::_a )] >> ")"] | hold[ADD1];

        subtractionEnd %= /*hold[subtractionRule1] |*/ hold[getSubtractor] | hold[getAddtractor];

        //- 24 1+1 = 2
        additionsimplify %=
         ( ( omit[string( "ADD" ) >> '(' >> omit[double_[qi::_a = qi::_1]] >> ',' >> omit[double_[qi::_b = qi::_1]] >>
                  ')'] )[phoenix::bind( &Arithmetic< std::plus< const double > >, qi::_a, qi::_b, qi::_c )] >>
           attr( qi::_val + qi::_c ) );

        //- 23 1-1 = 0
        subtractionsimplify %=
         ( omit[( "SUB(" >> double_[qi::_a = qi::_1] >> ',' >> double_[qi::_b = qi::_1] >>
                  ')' )] )[phoenix::bind( &Arithmetic< std::minus< const double > >, qi::_a, qi::_b, qi::_c )] >>
         attr( qi::_val + qi::_c );

        //- 22 1*A = A
        mulConstantFolding2 %= omit["MUL(" >> double_[qi::_a = qi::_1] >> ',' >> group[qi::_b = qi::_1] >>
                                    ')'][phoenix::bind( &MulConstantFolding2, qi::_a, qi::_b, qi::_c )] >>
                               attr( qi::_val + qi::_c );

        //- 21 A*1 = A
        mulConstantFolding1 %= ( ( "MUL(" >> omit[group[qi::_a = qi::_1]] >> "," >> omit[double_[qi::_b = qi::_1]] >>
                                   ")" )[phoenix::bind( &MulConstantFolding, qi::_a, qi::_b, qi::_c )] >>
                                 attr( qi::_val + qi::_c ) );

        //- 20 2 * 5 = 10
        multiplicationsimplify %=
         ( "MUL(" >> omit[double_[qi::_a = qi::_1]] >> "," >> omit[double_[qi::_b = qi::_1]] >>
           ")" )[phoenix::bind( &Arithmetic< std::multiplies< const double > >, qi::_a, qi::_b, qi::_c )] >>
         attr( qi::_val + qi::_c );

        //- 19 10 / 5 = 2
        divisionimplify %=
         ( ( omit["DIV(" >> omit[double_[qi::_a = qi::_1]] >> "," >> omit[double_[qi::_b = qi::_1]] >>
                  ")"] )[phoenix::bind( &Arithmetic< std::divides< const double > >, qi::_a, qi::_b, qi::_c )] >>
           attr( qi::_val + qi::_c ) );


        //- no need to peak
        //- 18 SQRT(9) = 3
        sqrtsimplify %= omit[( "SQRT(" >> omit[double_[qi::_a = qi::_1]] > ')' )][phoenix::bind( &Sqrt, qi::_a, qi::_b )] >
                        attr( qi::_val + qi::_b );


        /// SUB(DIV(A,B), C)  --> DIV(SUB(A, MUL(C,B)),B)
        // - 17 A/B - C = (A - B * C) / C
        expand3_4 %= omit["SUB(DIV(" >> group[qi::_a = qi::_1] >> "," >> group[qi::_b = qi::_1] >> ")," >>
                          group[qi::_c = qi::_1] >> ")"] >>
                     attr( qi::_val + "DIV(SUB(" + qi::_a + ",MUL(" + qi::_c + "," + qi::_b + "))," + qi::_b + ")" );


        //- 16 A - B / C = (A * C - B) / C
        expand3_3 %= omit["SUB(" >> group[qi::_a = qi::_1] >> ",DIV(" >> group[qi::_b = qi::_1] >> "," >>
                          group[qi::_c = qi::_1] >> "))"] >>
                     attr( qi::_val + "DIV(SUB(MUL(" + qi::_a + "," + qi::_c + ")," + qi::_b + ")," + qi::_c + ")" );

        //- 15 A + B/C = (A * C + B ) / C
        expand3_2 %= omit["ADD(" >> group[qi::_a = qi::_1] >> ",DIV(" >> group[qi::_b = qi::_1] >> "," >>
                          group[qi::_c = qi::_1] >> "))"] >>
                     attr( qi::_val + "DIV(ADD(MUL(" + qi::_a + "," + qi::_c + ")," + qi::_b + ")," + qi::_c + ")" );


        //- 14 (A/B) + C = (A +B*C) / C
        expand3_1 %= omit["ADD(DIV(" >> group[qi::_a = qi::_1] >> "," >> group[qi::_b = qi::_1] >> ")," >>
                          group[qi::_c = qi::_1] >> ")"] >>
                     attr( qi::_val + "DIV(ADD(" + qi::_a + ",MUL(" + qi::_b + "," + qi::_c + "))," + qi::_b + ")" );


        //- 13 A/B - C/D = (A*D - C*B) / (B*D)
        expand2 %= omit["SUB(DIV(" >> group[qi::_a = qi::_1] >> "," >> group[qi::_b = qi::_1] >> ")," >> "DIV(" >>
                        group[qi::_c = qi::_1] >> "," >> group[qi::_d = qi::_1] >> "))"] >>
                   attr( qi::_val + "DIV(SUB(MUL(" + qi::_a + "," + qi::_d + "),MUL(" + qi::_b + "," + qi::_c +
                         ")),MUL(" + qi::_b + "," + qi::_d + "))" );

        //- 12 (A/B + C/D) = (A*D + C*B) / (B * D)
        expand1 %= omit["ADD(DIV(" >> group[qi::_a = qi::_1] >> "," >> group[qi::_b = qi::_1] >> ")," >> "DIV(" >>
                        group[qi::_c = qi::_1] >> "," >> group[qi::_d = qi::_1] >> "))"] >>
                   attr( qi::_val + "DIV(ADD(MUL(" + qi::_a + "," + qi::_d + "),MUL(" + qi::_b + "," + qi::_c +
                         ")),MUL(" + qi::_b + "," + qi::_d + "))" );

        //- 27_a -A -(B) = -( A + B)
        sub2Add_a %= omit["SUB(NEG(" >> group[qi::_a = qi::_1] >> ")," >> group[qi::_b = qi::_1] >> ")"] >>
                     attr( qi::_val + "NEG(ADD(" + qi::_a + "," + qi::_b + "))" );

        //- 11 A - -(B) = A + B
        sub2Add %= omit["SUB(" >> group[qi::_a = qi::_1] >> "," >> "NEG(" >> group[qi::_b = qi::_1] >> "))"] >>
                   attr( qi::_val + "ADD(" + qi::_a + "," + qi::_b + ")" );


        //- 10 -A + B = B - A
        sum2Sub %= omit["ADD(NEG(" >> group[qi::_a = qi::_1] >> ")," >> group[qi::_b = qi::_1] >> ")"] >>
                   attr( qi::_val + "SUB(" + qi::_b + "," + qi::_a + ")" );


        //- 9 -(-A) = A // no need to peak
        negationsimplify %= omit["NEG(NEG("] > group >> omit["))"];

        //- 8 (B/C) * A = (A*B)/C
        mulDivReorder2 %=
         omit["MUL(DIV(" >> group[qi::_b = qi::_1] >> ',' >> group[qi::_c = qi::_1] >> ")," >> group[qi::_a = qi::_1] >> ')'] >>
         attr( qi::_val + "DIV(MUL(" + qi::_a + "," + qi::_b + ")," + qi::_c + ")" );


        //- 7 A* (B/C) = (A*B)/C
        mulDivReorder1 %= omit["MUL(" >> group[qi::_a = qi::_1] >> ",DIV(" >> group[qi::_b = qi::_1] >> ',' >>
                               group[qi::_c = qi::_1] >> "))"] >>
                          attr( qi::_val + "DIV(MUL(" + qi::_a + "," + qi::_b + ")," + qi::_c + ")" );

        //- 6 (A/B) + (C/B) = (A+C) / C
        addDivDiv %= omit["ADD(DIV(" >> group[qi::_a = qi::_1] >> "," >> group[qi::_b = qi::_1] >> "),DIV(" >>
                          group[qi::_c = qi::_1] >> "," >> string( qi::_b ) >> "))"] >>
                     attr( qi::_val + "DIV(ADD(" + qi::_a + "," + qi::_c + ")," + qi::_b + ")" );

        //- 5 A/ -(B) = -(A/B)
        mulNeg4 %= omit["DIV(" >> group[qi::_a = qi::_1] >> ",NEG(" >> group[qi::_b = qi::_1] >> "))"] >>
                   attr( qi::_val + "NEG(DIV(" + qi::_a + "," + qi::_b + "))" );

        //- 4 A* -(B) = -(A*B)
        mulNeg3 %= omit["MUL(" >> group[qi::_a = qi::_1] >> ",NEG(" >> group[qi::_b = qi::_1] >> "))"] >>
                   attr( qi::_val + "NEG(MUL(" + qi::_a + "," + qi::_b + "))" );

        //- 3 (-A) * B = -(A * B)
        mulNeg2 %= omit["MUL(NEG(" >> group[qi::_a = qi::_1] >> ")," >> group[qi::_b = qi::_1] >> ")"] >>
                   attr( qi::_val + "NEG(MUL(" + qi::_a + "," + qi::_b + "))" );

        //- 2 (-A)/B  = -(A/B)
        mulNeg1 %= omit["DIV(NEG(" >> group[qi::_a = qi::_1] >> ")," >> group[qi::_b = qi::_1] >> ")"] >>
                   attr( qi::_val + "NEG(DIV(" + qi::_a + "," + qi::_b + "))" );


        //- 1_1  (A/B) / C = A / ( B*C)
        division2multiplication_2 %= omit["DIV(DIV(" >> group[qi::_a = qi::_1] >> "," >> group[qi::_b = qi::_1] >>
                                          ")," >> group[qi::_c = qi::_1] >> ")"] >>
                                     attr( qi::_val + "DIV(" + qi::_a + ",MUL(" + qi::_b + "," + qi::_c + "))" );

        //- 1 (A/ ( B/C)) = (A*C) / B
        division2multiplication_1 %= omit["DIV(" >> group[qi::_a = qi::_1] >> ",DIV(" >> group[qi::_b = qi::_1] >>
                                          ',' >> group[qi::_c = qi::_1] >> "))"] >>
                                     attr( qi::_val + "DIV(MUL(" + qi::_a + "," + qi::_c + ")," + qi::_b + ")" );

        doubleRule %= doubleOperator > char_( '(' ) > ( group > ( char_( ',' ) > group ) ) > char_( ')' );
        singleRule %= singleOperator > char_( '(' ) > group > char_( ')' );

        DIV_PREFIX %= &( string( "DIV" ) ) >> ( hold[mulNeg1] | hold[mulNeg4] | hold[divisionimplify] |
                                                hold[division2multiplication_1] | hold[division2multiplication_2] );    // 5
        MUL_PREFIX %= &( string( "MUL" ) ) >>
                      ( hold[mulNeg2] | hold[mulNeg3] | hold[mulDivReorder1] | hold[mulDivReorder2] |
                        hold[multiplicationsimplify] | hold[mulConstantFolding1] | hold[mulConstantFolding2] );    // 7
        ADD_PREFIX %= &( string( "ADD" ) ) >> ( hold[addDivDiv] | hold[sum2Sub] | hold[expand1] | hold[expand3_1] | hold[addNeg] |
                                                hold[expand3_2] | hold[additionsimplify] | hold[getAddtractor] );    // 7
        SUB_PREFIX %= &( string( "SUB" ) ) >> ( hold[sub2Add] | hold[sub2Add_a] | hold[expand2] | hold[expand3_3] |
                                                hold[expand3_4] | hold[subtractionsimplify] | hold[getSubtractor] );    // 6

        simplification %= ( MUL_PREFIX | ADD_PREFIX | SUB_PREFIX | DIV_PREFIX | hold[negationsimplify] | hold[sqrtsimplify] );

        group %= ( hold[state] | hold[inputCurrent] | hold[zahl] | hold[twoPortID] | hold[subtractionEnd] |
                   hold[simplification] | hold[doubleRule] | hold[singleRule] );


        // BOOST_SPIRIT_DEBUG_NODES( ( localRule ) );

        BOOST_SPIRIT_DEBUG_NODES( ( SUB2_2 ) );
        BOOST_SPIRIT_DEBUG_NODES( ( SUB2_1 ) );
        BOOST_SPIRIT_DEBUG_NODES( ( SUB1_2 ) );
        BOOST_SPIRIT_DEBUG_NODES( ( SUB1_1 ) );

        BOOST_SPIRIT_DEBUG_NODES( ( ADD1 ) );

        BOOST_SPIRIT_DEBUG_NODES( ( SUB0 ) );
        BOOST_SPIRIT_DEBUG_NODES( ( SUB1 ) );
        BOOST_SPIRIT_DEBUG_NODES( ( SUB2 ) );

        BOOST_SPIRIT_DEBUG_NODES( ( getSubtractor ) );
        BOOST_SPIRIT_DEBUG_NODES( ( getAddtractor ) );

        BOOST_SPIRIT_DEBUG_NODES( ( duplicationFinderSecondArgument ) );
        BOOST_SPIRIT_DEBUG_NODES( ( duplicationFinderFirstArgument ) );
        BOOST_SPIRIT_DEBUG_NODES( ( duplicationFinder ) );
        BOOST_SPIRIT_DEBUG_NODES( ( subtractionEnd ) );

        BOOST_SPIRIT_DEBUG_NODES( ( additionsimplify ) );
        BOOST_SPIRIT_DEBUG_NODES( ( subtractionsimplify ) );
        BOOST_SPIRIT_DEBUG_NODES( ( multiplicationsimplify ) );
        BOOST_SPIRIT_DEBUG_NODES( ( divisionimplify ) );

        BOOST_SPIRIT_DEBUG_NODES( ( division2multiplication_1 ) );
        BOOST_SPIRIT_DEBUG_NODES( ( division2multiplication_2 ) );
        BOOST_SPIRIT_DEBUG_NODES( ( negationsimplify ) );
        BOOST_SPIRIT_DEBUG_NODES( ( sqrtsimplify ) );
        BOOST_SPIRIT_DEBUG_NODES( ( group ) );
        BOOST_SPIRIT_DEBUG_NODES( ( singleOperator ) );
        BOOST_SPIRIT_DEBUG_NODES( ( doubleOperator ) );
        BOOST_SPIRIT_DEBUG_NODES( ( doubleRule ) );
        BOOST_SPIRIT_DEBUG_NODES( ( singleRule ) );

        BOOST_SPIRIT_DEBUG_NODES( ( twoPortSuffix ) );
        BOOST_SPIRIT_DEBUG_NODES( ( twoPortID ) );
        BOOST_SPIRIT_DEBUG_NODES( ( zahl ) );
        BOOST_SPIRIT_DEBUG_NODES( ( operand ) );
    }

    // qi::rule< Iterator, std::string(), ascii::space_type > localRule;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB3_2;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB3_1;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB3;

    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB2_2;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB2_1;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB2;

    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB1_2;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB1_1;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB1;

    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string > > ADD1;

    qi::rule< Iterator, std::string( std::string ), ascii::space_type > SUB0;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type, qi::locals< std::string > > duplicationFinderSecondArgument;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type, qi::locals< std::string > > duplicationFinderFirstArgument;
    qi::rule< Iterator, std::string( std::string ), ascii::space_type, qi::locals< std::string > > duplicationFinder;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > getSubtractor;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > getAddtractor;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string > > subtractionRule1;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string > > subtractionEnd;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string > > subtractionStart;


    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > mulDivReorder2;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > mulDivReorder1;


    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > expand3_4;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > expand3_3;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > expand3_2;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > expand3_1;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string, std::string > > expand2;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string, std::string > > expand1;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > sub2Add_a;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > sub2Add;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > addNeg;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > sum2Sub;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > mulNeg4;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > mulNeg3;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > mulNeg2;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string > > mulNeg1;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > addDivDiv;

    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > division2multiplication_2;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, std::string, std::string > > division2multiplication_1;
    qi::rule< Iterator, std::string(), ascii::space_type > negationsimplify;
    qi::rule< Iterator, std::string(), ascii::space_type > simplification;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, std::string > > sqrtsimplify;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< std::string, double, std::string > > mulConstantFolding1;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, std::string, std::string > > mulConstantFolding2;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, double, std::string > > multiplicationsimplify;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, double, std::string > > divisionimplify;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, double, std::string > > subtractionsimplify;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, double, std::string > > additionsimplify;
    //    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, std::string > > char2double;

    qi::rule< Iterator, std::string(), ascii::space_type > DIV_PREFIX;
    qi::rule< Iterator, std::string(), ascii::space_type > ADD_PREFIX;
    qi::rule< Iterator, std::string(), ascii::space_type > SUB_PREFIX;
    qi::rule< Iterator, std::string(), ascii::space_type > MUL_PREFIX;
    qi::rule< Iterator, std::string(), ascii::space_type > doubleRule;
    qi::rule< Iterator, std::string(), ascii::space_type > singleRule;
    qi::rule< Iterator, std::string(), ascii::space_type > singleOperator;

    qi::rule< Iterator, std::string(), ascii::space_type > AddSubOperator;
    qi::rule< Iterator, std::string(), ascii::space_type > MulDivOperator;
    qi::rule< Iterator, std::string(), ascii::space_type > doubleOperator;
    qi::rule< Iterator, std::string(), ascii::space_type, qi::locals< double, std::string > > zahl;
    qi::rule< Iterator, std::string(), ascii::space_type > twoPortSuffix;
    qi::rule< Iterator, std::string(), ascii::space_type > twoPortID;
    qi::rule< Iterator, std::string(), ascii::space_type > inputCurrent;
    qi::rule< Iterator, std::string(), ascii::space_type > state;
    qi::rule< Iterator, std::string(), ascii::space_type > operand;
    qi::rule< Iterator, std::string(), ascii::space_type > group;
};

/// A symbolic Parser for the intermediate language which is used by different output formats
template < typename Iterator >
struct SymbolicParser : qi::grammar< Iterator, std::string(), ascii::space_type >
{

    SymbolicParser()
        : SymbolicParser::base_type( start )
    {
        using qi::on_error;
        using qi::fail;
        using boost::phoenix::construct;
        using boost::phoenix::val;


        start %= r.group;
        on_error< fail >( start, std::cout
                                  << val( "Error! Expecting " ) << qi::_4    // what failed?
                                  << val( " here: \"" )
                                  << construct< std::string >( qi::_3, qi::_2 )    // iterators to error-pos, end
                                  << val( "\"" ) << std::endl );
    }

    private:
    Rulez< Iterator > r;
    qi::rule< Iterator, std::string(), ascii::space_type > start;
};
}
#endif /* _DS1006 */
#endif /* _SYMBOLICPARSER_ */
