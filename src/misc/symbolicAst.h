/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolicAst.h
* Creation Date : 04-01-2017
* Last Modified : Di 20 Jun 2017 22:55:23 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLICAST_
#define _SYMBOLICAST_
//#define BOOST_SPIRIT_DEBUG
#ifndef _DS1006

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_hold.hpp>

#include <boost/phoenix/phoenix.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/bind/bind_function.hpp>

#include <boost/lexical_cast.hpp>
#include "symbolicParser.h"

#include <functional>
#include <cmath>
#include <algorithm>

namespace symbolic
{
namespace ast
{
namespace tag
{
struct Token
{
    bool mIsConstant = false;
    bool mIsObject = false;
    bool mIsAssociative = false;
    bool mIsCommutativ = false;
    bool mAdditionBarrier = false;
    bool mMultiplicationBarrier = false;
    bool mZeroSkipable = false;
    bool mOneSkipable = false;
};

struct Constant_T : public Token
{
    Constant_T()
        : Token()
    {
        mIsConstant = true;
    };
};
struct Object_T : public Token
{
    Object_T()
        : Token()
    {
        mIsObject = true;
    }
};

struct Function_T : public Token
{
    Function_T()
        : Token()
    {
    }
};

template < class X >
struct Convert_T
{
    double operator()( double& i, double& j ) { return X()( i, j ); }
    double operator()( double& i ) { return X()( i ); }

    template < typename T >
    std::vector< std::vector< T > >
    operator()( const std::vector< std::vector< T > >& l, const std::vector< std::vector< T > >& r )
    {
        std::vector< std::vector< T > > returnVec = {l.size(), std::vector< T >( l.at( 0 ).size() )};
        for ( size_t i = 0; i < l.size(); ++i )
        {

            auto& inner = l[i];
            std::transform( inner.begin(), inner.end(), r[i].begin(), returnVec[i].begin(), X() );
        }
        return returnVec;
    }

    template < typename T >
    std::string operator()( T& i, T& j )
    {
        const double i_x = boost::lexical_cast< double >( i.name );
        const double j_x = boost::lexical_cast< double >( j.name );
        return boost::lexical_cast< std::string >( X()( i_x, j_x ) );
    }

    template < typename T >
    std::string operator()( T& i )
    {
        const double i_x = boost::lexical_cast< double >( i.name );
        return boost::lexical_cast< std::string >( X()( i_x ) );
    }
    // typename Operator_T X;
};


struct Add_T : public Function_T
{
    Add_T()
        : Function_T()
    {
        mZeroSkipable = true;
        mIsAssociative = true;
        mIsCommutativ = true;
    }
    Convert_T< std::plus< double > > Converter;
};

struct Sub_T : public Function_T
{
    Sub_T()
        : Function_T()
    {
        mZeroSkipable = true;
    }
    Convert_T< std::minus< double > > Converter;
};

struct Mul_T : public Function_T
{
    Mul_T()
        : Function_T()
    {
        mOneSkipable = true;
        mAdditionBarrier = true;
        mIsAssociative = true;
        mIsCommutativ = true;
    }

    Convert_T< std::multiplies< double > > Converter;
};

struct Div_T : public Function_T
{
    Div_T()
        : Function_T()
    {
        mOneSkipable = true;
        mAdditionBarrier = true;
    }
    Convert_T< std::divides< double > > Converter;
};

template < class T >
struct DoNothing
{
    std::string operator()( const T&, const T& ) const { return ""; };
    std::string operator()( const T& ) const { return ""; };
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef std::string result_type;
};

struct Sqrt_Functor
{
    double operator()( double i ) { return sqrt( i ); }

    template < typename T >
    std::vector< std::vector< T > > operator()( const std::vector< std::vector< T > >& l )
    {
        std::vector< std::vector< T > > returnVec = {l.size(), std::vector< T >( l.at( 0 ).size() )};
        for ( size_t i = 0; i < l.size(); ++i )
        {
            for ( size_t j = 0; j < l[i].size(); ++j )
            {
                returnVec[i][j] = std::sqrt( l[i][j] );
            }
        }
        return returnVec;
    }

    template < typename T >
    std::vector< T > operator()( const std::vector< T >& l )
    {
        std::vector< T > returnVec( l.size() );
        for ( size_t i = 0; i < l.size(); ++i )
        {
            returnVec[i] = std::sqrt( l[i] );
        }
        return returnVec;
    }


    template < class X >
    std::string operator()( const X& i ) const
    {
        const double i_x = boost::lexical_cast< double >( i.name );
        result_type returnValue = boost::lexical_cast< std::string >( sqrt( i_x ) );
        return returnValue;
    }
    typedef std::string result_type;
};

struct Pow_Functor
{

    double operator()( double& i, double& j ) { return std::pow< double >( i, j ); }

    template < typename T >
    std::vector< std::vector< T > >
    operator()( const std::vector< std::vector< T > >& l, const std::vector< std::vector< T > >& r )
    {
        std::vector< std::vector< T > > returnVec = {l.size(), std::vector< T >( l.at( 0 ).size() )};
        for ( size_t i = 0; i < l.size(); ++i )
        {
            for ( size_t j = 0; j < l[i].size(); ++j )
            {
                returnVec[i][j] = std::pow< T >( l[i][j], r[i][j] );
            }
        }
        return returnVec;
    }

    double operator()( const double& i, const double& j ) { return std::pow< double >( i, j ); }


    template < class T >
    std::string operator()( T& i, T& j )
    {
        const double i_x = boost::lexical_cast< double >( i.name );
        const double j_x = boost::lexical_cast< double >( j.name );
        return boost::lexical_cast< std::string >( std::pow< double >( i_x, j_x ) );
    }
    typedef std::string result_type;
};

struct Tanh_Functor
{
    double operator()( double& i ) { return tanh( i ); }

    template < typename T >
    std::vector< std::vector< T > > operator()( const std::vector< std::vector< T > >& l )
    {
        std::vector< std::vector< T > > returnVec = {l.size(), std::vector< T >( l.at( 0 ).size() )};
        for ( size_t i = 0; i < l.size(); ++i )
        {
            for ( size_t j = 0; j < l[i].size(); ++j )
            {
                returnVec[i][j] = std::tanh( l[i][j] );
            }
        }
        return returnVec;
    }

    template < typename T >
    std::vector< T > operator()( const std::vector< T >& l )
    {
        std::vector< T > returnVec( l.size() );
        for ( size_t i = 0; i < l.size(); ++i )
        {
            returnVec[i] = std::tanh( l[i] );
        }
        return returnVec;
    }


    double operator()( const double& i ) { return std::tanh( i ); }


    template < class T >
    std::string operator()( T& i )
    {
        const double i_x = boost::lexical_cast< double >( i.name );
        return boost::lexical_cast< std::string >( tanh( i_x ) );
    }
    typedef std::string result_type;
};

struct Neg_Functor
{
    double operator()( double& i ) { return -i; }
    double operator()( const double& i ) { return -i; }

    template < typename T >
    std::vector< std::vector< T > > operator()( const std::vector< std::vector< T > >& l )
    {
        std::vector< std::vector< T > > returnVec = {l.size(), std::vector< T >( l.at( 0 ).size() )};
        for ( size_t i = 0; i < l.size(); ++i )
        {
            for ( size_t j = 0; j < l[i].size(); ++j )
            {
                returnVec[i][j] = -l[i][j];
            }
        }
        return returnVec;
    }


    template < typename T >
    std::vector< T > operator()( const std::vector< T >& l )
    {
        std::vector< T > returnVec( l.size() );
        for ( size_t i = 0; i < l.size(); ++i )
        {
            returnVec[i] = -l[i];
        }
        return returnVec;
    }


    template < class T >
    std::string operator()( T& i )
    {
        double i_x = boost::lexical_cast< double >( i.name );
        return boost::lexical_cast< std::string >( -i_x );
    }
    typedef std::string result_type;
};


struct Neg_T : public Function_T
{
    Neg_T()
        : Function_T()
    {
    }

    Neg_Functor Converter;
};

struct Sqrt_T : public Function_T
{
    Sqrt_T()
        : Function_T()
    {
    }
    Sqrt_Functor Converter;
};

struct Pow_T : public Function_T
{
    Pow_T()
        : Function_T()
    {
    }
    Pow_Functor Converter;    // std::pow< double >, std::placeholders::_1,
                              // std::placeholders::_2 )
                              // > Converter;
};

struct Tanh_T : public Function_T
{
    Tanh_T()
        : Function_T()
    {
    }
    Tanh_Functor Converter;
};
}


template < typename Tag >
struct Identifier
{
    typedef Tag value_T;
    std::string name;
};

using Constant = Identifier< ast::tag::Constant_T >;
using Variable = Identifier< ast::tag::Object_T >;
using Function = Identifier< ast::tag::Function_T >;

template < typename Optimizer >
struct BinaryFunctionCall;

template < typename Optimizer >
struct UnaryFunctionCall;

struct Nil
{
};

using Expression = boost::make_recursive_variant<
 Nil, Constant, Variable, boost::recursive_wrapper< BinaryFunctionCall< ast::tag::Add_T > >,
 boost::recursive_wrapper< BinaryFunctionCall< ast::tag::Sub_T > >, boost::recursive_wrapper< BinaryFunctionCall< ast::tag::Mul_T > >,
 boost::recursive_wrapper< BinaryFunctionCall< ast::tag::Div_T > >, boost::recursive_wrapper< UnaryFunctionCall< ast::tag::Tanh_T > >,
 boost::recursive_wrapper< UnaryFunctionCall< ast::tag::Neg_T > >, boost::recursive_wrapper< UnaryFunctionCall< ast::tag::Sqrt_T > >,
 boost::recursive_wrapper< BinaryFunctionCall< ast::tag::Pow_T > > >::type;

template < typename Optimizer >
struct BinaryFunctionCall
{
    Function funct;
    std::vector< Expression > params;
};

template < typename Optimizer >
struct UnaryFunctionCall
{
    Function funct;
    std::vector< Expression > params;
};
}
}

BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::Constant, ( std::string, name ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::Variable, ( std::string, name ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::Function, ( std::string, name ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::BinaryFunctionCall< symbolic::ast::tag::Add_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::BinaryFunctionCall< symbolic::ast::tag::Sub_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::BinaryFunctionCall< symbolic::ast::tag::Mul_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::BinaryFunctionCall< symbolic::ast::tag::Div_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::BinaryFunctionCall< symbolic::ast::tag::Pow_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );


BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::UnaryFunctionCall< symbolic::ast::tag::Sqrt_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::UnaryFunctionCall< symbolic::ast::tag::Tanh_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );
BOOST_FUSION_ADAPT_STRUCT( symbolic::ast::UnaryFunctionCall< symbolic::ast::tag::Neg_T >,
                           ( symbolic::ast::Function, funct )( std::vector< symbolic::ast::Expression >, params ) );


namespace symbolic
{
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template < typename Iterator >
struct RulezAst
{
    RulezAst()
    {
        using qi::string;
        using qi::double_;
        using boost::spirit::ascii::digit;
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

        zahl %= omit[double_[qi::_a = qi::_1]][phoenix::bind( &char2doubleFct, qi::_a, qi::_b )] >> attr( qi::_b );

        twoPortSuffix %= qi::as_string[string( "_LookupPhi2RFactorAussen" ) | string( "_LookupPhi2RFactorInnen" ) |
                                       string( "_LookupPhi2TauFactor" ) | string( "_ObjCT" ) | string( "_ObjR" ) |
                                       string( "_ObjC" ) | string( "_ObjV" ) | string( "_ObjTau" ) |
                                       string( "_ObjBase" ) | string( "_ObjMP" ) | string( "_Merge" )];

        twoPortID %= qi::as_string[( string( "ID" ) >> +digit ) >> twoPortSuffix];
        inputCurrent %= qi::as_string[string( "InputCurrent" )];    //[qi::_a = qi::_1] > attr( qi::_val + qi::_a );
        state %= qi::as_string[string( "State" ) > +digit];

        doubleRule %= addRule | subRule | mulRule | divRule | powRule;
        addRule = qi::as_string[string( "ADD" )] > '(' > ( group >> ',' >> group ) > ')';
        subRule = qi::as_string[string( "SUB" )] > '(' > ( group >> ',' >> group ) > ')';
        mulRule = qi::as_string[string( "MUL" )] > '(' > ( group >> ',' >> group ) > ')';
        divRule = qi::as_string[string( "DIV" )] > '(' > ( group >> ',' >> group ) > ')';
        powRule = qi::as_string[string( "POW" )] > '(' > ( group >> ',' >> group ) > ')';


        singleRule %= SqrtRule | NegRule | TanhRule;
        SqrtRule = qi::as_string[string( "SQRT" )] > '(' >> group > ')';
        NegRule = qi::as_string[string( "NEG" )] > '(' >> group > ')';
        TanhRule = qi::as_string[string( "TANH" )] > '(' >> group > ')';


        group %= state | inputCurrent | zahl | twoPortID | doubleRule | singleRule;

        BOOST_SPIRIT_DEBUG_NODES( ( group ) );
        BOOST_SPIRIT_DEBUG_NODES( ( doubleRule ) );
        BOOST_SPIRIT_DEBUG_NODES( ( twoPortSuffix ) );
        BOOST_SPIRIT_DEBUG_NODES( ( twoPortID ) );
        BOOST_SPIRIT_DEBUG_NODES( ( zahl ) );
    }

    qi::rule< Iterator, symbolic::ast::Expression(), ascii::space_type > doubleRule;
    qi::rule< Iterator, symbolic::ast::Expression(), ascii::space_type > singleRule;

    qi::rule< Iterator, symbolic::ast::UnaryFunctionCall< ast::tag::Sqrt_T >(), ascii::space_type > SqrtRule;
    qi::rule< Iterator, symbolic::ast::UnaryFunctionCall< ast::tag::Neg_T >(), ascii::space_type > NegRule;
    qi::rule< Iterator, symbolic::ast::UnaryFunctionCall< ast::tag::Tanh_T >(), ascii::space_type > TanhRule;

    qi::rule< Iterator, symbolic::ast::BinaryFunctionCall< ast::tag::Add_T >(), ascii::space_type > addRule;
    qi::rule< Iterator, symbolic::ast::BinaryFunctionCall< ast::tag::Sub_T >(), ascii::space_type > subRule;
    qi::rule< Iterator, symbolic::ast::BinaryFunctionCall< ast::tag::Mul_T >(), ascii::space_type > mulRule;
    qi::rule< Iterator, symbolic::ast::BinaryFunctionCall< ast::tag::Div_T >(), ascii::space_type > divRule;
    qi::rule< Iterator, symbolic::ast::BinaryFunctionCall< ast::tag::Pow_T >(), ascii::space_type > powRule;

    qi::rule< Iterator, std::string(), ascii::space_type > AddSubOperator;
    qi::rule< Iterator, std::string(), ascii::space_type > MulDivOperator;
    qi::rule< Iterator, symbolic::ast::Constant(), ascii::space_type, qi::locals< double, std::string > > zahl;
    qi::rule< Iterator, std::string(), ascii::space_type > twoPortSuffix;
    qi::rule< Iterator, symbolic::ast::Variable(), ascii::space_type > twoPortID;
    qi::rule< Iterator, symbolic::ast::Variable(), ascii::space_type, qi::locals< std::string > > inputCurrent;
    qi::rule< Iterator, symbolic::ast::Variable(), ascii::space_type > state;
    qi::rule< Iterator, symbolic::ast::Expression(), ascii::space_type > group;
};

/// A symbolic Parser for the intermediate language which is used by different output formats
template < typename Iterator >
struct SymbolicAst : qi::grammar< Iterator, symbolic::ast::Expression(), ascii::space_type >
{

    SymbolicAst()
        : SymbolicAst::base_type( start )
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
    RulezAst< Iterator > r;
    qi::rule< Iterator, symbolic::ast::Expression(), ascii::space_type > start;
};
}
#endif /* _DS1006 */
#endif /* _SYMBOLICAST_ */
