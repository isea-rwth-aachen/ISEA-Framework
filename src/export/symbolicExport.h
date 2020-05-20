/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolicExport.h
* Creation Date : 10-01-2017
* Last Modified : Di 20 Jun 2017 22:19:34 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLICEXPORT_
#define _SYMBOLICEXPORT_
#ifndef _DS1006

//#define BOOST_SPIRIT_DEBUG
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_hold.hpp>

#include <boost/phoenix/phoenix.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/lambda/lambda.hpp>


#include <boost/spirit/include/qi_eps.hpp>
#include <boost/lexical_cast.hpp>

#include <functional>
#include <cmath>

#include "../misc/symbolic.h"

namespace symbolic
{
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/// Standard Export, don't do anything
struct StandardExportStruct
{
    // Exportfunctions for exporting to latex
    public:
    bool FallThrough() const { return true; };
    const std::string newline{"\n"};
    const std::string spacer{" , "};
    std::string PreTextOutput( const std::string name ) const { return name + " = "; };
    std::string PostTextOutput( const std::string /*name*/ ) const { return newline; };

    void OutputPhiAussen( std::string & /*input*/ ) const {};
    void OutputPhiInnen( std::string & /*input*/ ) const {};
    void TauFactor( std::string & /*input*/ ) const {};
    void OhmicResistance( std::string & /*input*/ ) const {};
    void VoltageSource( std::string & /*input*/ ) const {};
    void Capacity( std::string & /*input*/ ) const {};
    void Tau( std::string & /*input*/ ) const {};
    void MP( std::string & /*input*/ ) const {};
    void CT( std::string & /*input*/ ) const {};
    void Obj( std::string & /*input*/ ) const {};
    void TwoPort( std::string & /*input*/ ) const {};
    void Current( std::string & /*input*/ ) const {};
    void RealName( std::string & /*ID_Number*/, std::string & /*TypeName*/ ) const {}

    void AddOperator( std::string & /*add*/ ) const {};
    void SubOperator( std::string & /*sub*/ ) const {};
    void MulOperator( std::string & /*mul*/ ) const {};
    void DivOperator( std::string & /*operand1*/, std::string & /*operand2*/ ) const {};
    void PowOperator( std::string & /*operand1*/, std::string & /*operand2*/ ) const {};
    void DoubleRule( std::string & /*op*/, std::string & /*operand1*/, std::string & /*operand2*/ ) const {}
    void SqrtRule( std::string & /*operand*/ ) const {}
    void TanhRule( std::string & /*operand*/ ) const {}
    void NegRule( std::string & /*operand*/ ) const {}
    void State( std::string &returnString ) const { returnString = "State" + returnString; }
};

template < typename Iterator, typename GeneratorType >
struct ExportRulez
{

    ExportRulez()
        : mExport()
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
        using boost::spirit::eps;

        // operator ::= add | mul | prod | div | sqrt | pow
        // zahl    ::= int | double
        // operand ::= zahl | operator
        // group ::=  operator '(' (operand, operand) | operand ')'

        // operator = add | mul | prod | div | sqrt | pow;
        // zahl    = int | double;
        // operand = zahl | group;
        // group =  operator'(' (operand ',' operand) | operand ')';

        Add %= "ADD" >> eps[phoenix::bind( &GeneratorType::AddOperator, mExport, qi::_a )] >> attr( qi::_a );
        Sub %= "SUB" >> eps[phoenix::bind( &GeneratorType::SubOperator, mExport, qi::_a )] >> attr( qi::_a );
        AddSubOperator %= hold[Add] | hold[Sub];
        MulDivOperator %= "MUL" >> eps[phoenix::bind( &GeneratorType::MulOperator, mExport, qi::_a )] >> attr( qi::_a );
        doubleOperator %= AddSubOperator | MulDivOperator;

        zahl %= lexeme[-char_( '-' ) >> ( +digit >> -( char_( '.' ) >> *digit ) )];
        inputCurrent %= omit[string( "InputCurrent" )] >> attr( qi::_val + "I" );
        state %= omit[string( "State" )[qi::_a = ""]] >> omit[+digit[qi::_a += qi::_1]] >>
                 eps[phoenix::bind( &GeneratorType::State, mExport, qi::_a )] >> attr( qi::_val + qi::_a );

        /* Elemental Output */
        phi2RFactorAussen %= omit[char_( '_' )] >> string( "LookupPhi2RFactorAussen" );
        phi2RFactorInnen %= omit[char_( '_' )] >> string( "LookupPhi2RFactorInnen" );
        phi2TauFactor %= omit[char_( '_' )] >> string( "LookupPhi2TauFactor" );
        objR %= omit[string( "_ObjR" )] >> attr( qi::_val + "R" );
        objC %= omit[string( "_ObjC" )] >> attr( qi::_val + "C" );
        objV %= omit[string( "_ObjV" )] >> attr( qi::_val + "V" );
        objTau %= omit[string( "_ObjTau" )] >> attr( qi::_val + "Tau" );
        objMP %= omit[string( "_ObjMP" )] >> attr( qi::_val + "MP" );
        objCT %= omit[string( "_ObjCT" )] >> attr( qi::_val + "CT" );
        objBase %= omit[string( "_ObjBase" )] >> attr( qi::_val + "Base" );

        twoPortSuffix %= ( phi2RFactorAussen[phoenix::bind( &GeneratorType::OutputPhiAussen, mExport, qi::_1 )] |
                           phi2RFactorInnen[( phoenix::bind( &GeneratorType::OutputPhiInnen, mExport, qi::_1 ) )] |
                           phi2TauFactor[phoenix::bind( &GeneratorType::TauFactor, mExport, qi::_1 )] |
                           objR[phoenix::bind( &GeneratorType::OhmicResistance, mExport, qi::_1 )] |
                           objV[phoenix::bind( &GeneratorType::VoltageSource, mExport, qi::_1 )] |
                           objTau[phoenix::bind( &GeneratorType::Tau, mExport, qi::_1 )] |
                           objMP[phoenix::bind( &GeneratorType::MP, mExport, qi::_1 )] |
                           objCT[phoenix::bind( &GeneratorType::CT, mExport, qi::_1 )] |
                           objC[phoenix::bind( &GeneratorType::Capacity, mExport, qi::_1 )] |
                           objBase[phoenix::bind( &GeneratorType::Obj, mExport, qi::_1 )] );

        realName %= eps[phoenix::bind( &GeneratorType::RealName, mExport, qi::_r1, qi::_r2 )] >> attr( qi::_val + qi::_r2 );

        twoPortID %= omit[string( "ID" )[qi::_a = ""] >> +digit[qi::_a += qi::_1] >> twoPortSuffix[qi::_b = qi::_1]] >>
                     realName( qi::_a, qi::_b );


        operandWithoutGroup %= ( hold[zahl] | hold[twoPortID][phoenix::bind( &GeneratorType::TwoPort, mExport, qi::_1 )] |
                                 hold[inputCurrent][phoenix::bind( &GeneratorType::Current, mExport, qi::_1 )] | hold[state] );
        operand %= ( hold[group] | hold[zahl] | hold[twoPortID][phoenix::bind( &GeneratorType::TwoPort, mExport, qi::_1 )] |
                     hold[inputCurrent][phoenix::bind( &GeneratorType::Current, mExport, qi::_1 )] | hold[state] );

        divPeak %= ( omit["DIV(" >> ( operand[qi::_a = qi::_1] >> ( ',' ) >> operand[qi::_b = qi::_1] ) >> ')'] ) >>
                   eps[phoenix::bind( &GeneratorType::DivOperator, mExport, qi::_a, qi::_b )] >> attr( qi::_a );

        powPeak %= ( omit["POW(" >> ( operand[qi::_a = qi::_1] >> ( ',' ) >> operand[qi::_b = qi::_1] ) >> ')'] ) >>
                   eps[phoenix::bind( &GeneratorType::PowOperator, mExport, qi::_a, qi::_b )] >> attr( qi::_a );

        doubleRule %=
         hold[divPeak] | hold[powPeak] |
         ( omit[doubleOperator[qi::_a = qi::_1] >> '(' >> ( operand[qi::_b = qi::_1] >> ( ',' ) >> operand[qi::_c = qi::_1] ) >> ')'] >>
           eps[phoenix::bind( &GeneratorType::DoubleRule, mExport, qi::_a, qi::_b, qi::_c )] >> attr( qi::_a ) );

        singleRule %=
         ( omit["SQRT(" >> operand[qi::_a = qi::_1] >> ")"] )[phoenix::bind( &GeneratorType::SqrtRule, mExport, qi::_a )] >>
          attr( qi::_a ) |
         ( omit["TANH(" >> operand[qi::_a = qi::_1] >> ")"] )[phoenix::bind( &GeneratorType::TanhRule, mExport, qi::_a )] >>
          attr( qi::_a ) |
         ( omit["NEG(" >> operandWithoutGroup[qi::_a = qi::_1] >> ")"] ) >> attr( "-" + qi::_a ) |
         ( omit["NEG(" >> operand[qi::_a = qi::_1] >> ")"] )[phoenix::bind( &GeneratorType::NegRule, mExport, qi::_a )] >>
          attr( qi::_a );

        group %= ( hold[operandWithoutGroup] | hold[doubleRule] | hold[singleRule] );

        /*
        BOOST_SPIRIT_DEBUG_NODES( ( objCT ) );
        BOOST_SPIRIT_DEBUG_NODES( ( objMP ) );
        BOOST_SPIRIT_DEBUG_NODES( ( singleRule ) );
        BOOST_SPIRIT_DEBUG_NODES( ( group ) );
        BOOST_SPIRIT_DEBUG_NODES( ( doubleOperator ) );
        BOOST_SPIRIT_DEBUG_NODES( ( doubleRule ) );
        BOOST_SPIRIT_DEBUG_NODES( ( singleRule ) );

        BOOST_SPIRIT_DEBUG_NODES( ( twoPortSuffix ) );
        BOOST_SPIRIT_DEBUG_NODES( ( twoPortID ) );
        BOOST_SPIRIT_DEBUG_NODES( ( zahl ) );
        BOOST_SPIRIT_DEBUG_NODES( ( operand ) );
        */
    }

    GeneratorType const mExport;

    qi::rule< Iterator, std::string(), ascii::space_type > phi2RFactorAussen;
    qi::rule< Iterator, std::string(), ascii::space_type > phi2RFactorInnen;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string, std::string, std::string > > doubleRule;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string, std::string > > phi2TauFactor;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string > > singleRule;
    qi::rule< Iterator, std::string(), ascii::space_type > objR;
    qi::rule< Iterator, std::string(), ascii::space_type > objV;
    qi::rule< Iterator, std::string(), ascii::space_type > objC;
    qi::rule< Iterator, std::string(), ascii::space_type > objTau;
    qi::rule< Iterator, std::string(), ascii::space_type > objMP;
    qi::rule< Iterator, std::string(), ascii::space_type > objCT;
    qi::rule< Iterator, std::string(), ascii::space_type > objBase;
    qi::rule< Iterator, std::string( std::string, std::string ), ascii::space_type > realName;


    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string, std::string > > powPeak;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string, std::string > > divPeak;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string > > Sub;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string > > Add;
    qi::rule< Iterator, std::string(), ascii::space_type > AddSubOperator;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string > > MulDivOperator;
    qi::rule< Iterator, std::string(), ascii::space_type > doubleOperator;
    qi::rule< Iterator, std::string(), ascii::space_type > zahl;
    qi::rule< Iterator, std::string(), ascii::space_type > inputCurrent;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string > > state;
    qi::rule< Iterator, std::string(), ascii::space_type > twoPortSuffix;
    qi::rule< Iterator, std::string(), ascii::space_type, boost::spirit::locals< std::string, std::string > > twoPortID;
    qi::rule< Iterator, std::string(), ascii::space_type > operand;
    qi::rule< Iterator, std::string(), ascii::space_type > operandWithoutGroup;
    qi::rule< Iterator, std::string(), ascii::space_type > group;
};


template < typename Iterator, typename GeneratorType = StandardExportStruct >
class SymbolicExport : public qi::grammar< Iterator, std::string(), ascii::space_type >
{
    public:
    typedef GeneratorType Generator_T;
    SymbolicExport()
        : SymbolicExport::base_type( start )
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
    };
    ~SymbolicExport(){};

    private:
    ExportRulez< Iterator, GeneratorType > r;
    qi::rule< Iterator, std::string(), ascii::space_type > start;
};
}
#endif /* _DS1006 */
#endif /* _SYMBOLICEXPORT_ */
