#ifndef _SYMBOLIC_CLASS_H_
#define _SYMBOLIC_CLASS_H_

// STD
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>
// BOOST
#include <boost/spirit/include/qi.hpp>

// MISC
#include "../exceptions/error_proto.h"
#include "StrCont.h"
#include "symbolicParser.h"


class TestSymbolic;

namespace symbolic
{
// template struct SymbolicParser< std::string::const_iterator >;
using misc::StrCont;
using std::vector;
typedef double OutType;

template < typename OutputType = OutType >
class Symbolic
{
    public:
    typedef const char *value_type;
    bool empty() const { return IsEmpty(); };
    Symbolic();
    explicit Symbolic( const char *content );
    explicit Symbolic( const size_t content );
    explicit Symbolic( const int content );
    explicit Symbolic( const std::ptrdiff_t content );

    Symbolic( const double content );
    ~Symbolic();
    bool IsEmpty() const;
    bool IsOne() const;
    size_t size() const { return mContent.size(); };
    Symbolic< OutputType > &operator=( const char *content );
    const Symbolic< OutputType > operator-() const;
    const Symbolic< OutputType > &operator+=( const Symbolic< OutputType > &rhs );
    const Symbolic< OutputType > &operator-=( const Symbolic< OutputType > &rhs );
    const Symbolic< OutputType > &operator*=( const Symbolic< OutputType > &rhs );
    const Symbolic< OutputType > &operator/=( const Symbolic< OutputType > &rhs );
    bool operator==( const Symbolic< OutputType > &rhs ) const;
    bool operator!=( const Symbolic< OutputType > &rhs ) const;
    inline const misc::StrCont &GetString() const { return mContent; }
    void Simplyfy();

    private:
    bool ContentIsZero( const size_t &content ) const;
    bool ContentIsZero( const std::ptrdiff_t &content ) const;
    bool ContentIsZero( const int &content ) const;
    bool ContentIsZero( const double &content ) const;
    void DoNothingButAvoidCompilerWarning() const;
    StrCont mContent;
};

template < typename OutputType >
void Symbolic< OutputType >::Simplyfy()
{
    /*
    if ( empty() )
        return;
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    std::string input;
    std::string tmpString = static_cast< const char * >( this->GetString() );

    size_t i = 0;
    while ( true )
    {
        SymbolicParser< iterator_type > g;
        std::string str = input;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        std::string emp;

        bool r = boost::spirit::qi::phrase_parse( iter, end, g, space, emp );
        if ( !r || iter != end )
        {
            std::cerr << "Error parsing string" << std::endl;
            std::cerr << *iter;
            //            std::exit( EXIT_FAILURE );
        }
        if ( str == emp )
        {
            input = emp;
            break;
        }
        else
        {
            input = emp;
        }

        ++i;
    }
    this->mContent = StrCont( input.c_str() );
    */

    /*
    typedef std::string::const_iterator iterator_type;
    typedef ::symbolic::SymbolicParser< iterator_type > sp;
    using boost::spirit::ascii::space;
    std::string result = "";

    std::string input( this->GetString() );
    for ( size_t i = 0; i < 5; ++i )
    {
        result = "";
        sp g;
        std::string str = input;
        std::string::const_iterator iter = str.begin();
        std::string::const_iterator end = str.end();
        bool r = phrase_parse( iter, end, g, space, result );
        if ( !r || iter != end )
        {
            std::cerr << "Error parsing string: " << std::endl;
            for ( auto b = iter; b != end; ++b )
            {
                std::cout << *b;
            }
            std::cout << std::endl;

            std::exit( EXIT_FAILURE );
        }
        std::cout << "Round: " << i << std::endl << "Result: \n" << result << std::endl;
        if ( input == result )
            break;
        else
        {
            input = result;
        }
    }
    this->mContent = StrCont( input.c_str() );
    */
}


template < typename OutputType >
std::ostream &operator<<( std::ostream &os, const Symbolic< OutputType > &sym )
{
    if ( sym.IsEmpty() )
        os << "0";
    else
        os << sym.GetString();
    return os;
}


template < typename OutputType >
bool Symbolic< OutputType >::operator==( const Symbolic< OutputType > &rhs ) const
{
    return strcmp( mContent, rhs.mContent ) == 0;
}

template < typename OutputType >
bool Symbolic< OutputType >::operator!=( const Symbolic< OutputType > &rhs ) const
{
    return strcmp( mContent, rhs.mContent ) != 0;
}

template < typename OutputType >
Symbolic< OutputType >::Symbolic( const char *content )
    : mContent( content )
{
}

template < typename OutputType >
Symbolic< OutputType > &Symbolic< OutputType >::operator=( const char *content )
{
    if ( content )
        this->mContent = StrCont( content );
    else
        this->mContent = StrCont();
    this->Simplyfy();
    return *this;
}

template < typename OutputType >
Symbolic< OutputType >::Symbolic( const size_t content )
    : mContent( ContentIsZero( content ) ? StrCont() : StrCont( content ) )
{
}

template < typename OutputType >
Symbolic< OutputType >::Symbolic( const std::ptrdiff_t content )
    : mContent( ContentIsZero( content ) ? StrCont() : StrCont( content ) )
{
}

template < typename OutputType >
Symbolic< OutputType >::Symbolic( const int content )
    : mContent( ContentIsZero( content ) ? StrCont() : StrCont( content ) )
{
}

template < typename OutputType >
Symbolic< OutputType >::Symbolic( const double content )
    : mContent( ContentIsZero( content ) ? StrCont() : StrCont( content ) )
{
}

template < typename OutputType >
Symbolic< OutputType >::Symbolic()
    : mContent( StrCont() )
{
}

template < typename OutputType >
Symbolic< OutputType >::~Symbolic()
{
}

template < typename OutputType >
bool Symbolic< OutputType >::IsEmpty() const
{
    return strlen( mContent ) == 0;
}

template < typename OutputType >
bool Symbolic< OutputType >::IsOne() const
{
    double value = atof( mContent );
    return value < 1.000001 && value > 0.999999;
}

template < typename OutputType >
const Symbolic< OutputType > Symbolic< OutputType >::operator-() const
{
    StrCont tmp;

    if ( !IsEmpty() )
        tmp = StrCont( "NEG(" ) + mContent + StrCont( ")" );

    return Symbolic< OutputType >( tmp );
}

template < typename OutputType >
bool Symbolic< OutputType >::ContentIsZero( const size_t &content ) const
{
    return content == 0;
}

template < typename OutputType >
bool Symbolic< OutputType >::ContentIsZero( const std::ptrdiff_t &content ) const
{
    return content == 0;
}

template < typename OutputType >
bool Symbolic< OutputType >::ContentIsZero( const int &content ) const
{
    return content == 0;
}

template < typename OutputType >
bool Symbolic< OutputType >::ContentIsZero( const double &content ) const
{
    return content == 0.0;    // return content < 0.000001 && content > - 0.000001;
}

template < typename OutputType >
const Symbolic< OutputType > &Symbolic< OutputType >::operator+=( const Symbolic< OutputType > &rhs )
{
    if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "ADD(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else if ( IsEmpty() )
        mContent = rhs.mContent;
    else if ( rhs.IsEmpty() )
        DoNothingButAvoidCompilerWarning();
    else
        mContent = misc::StrCont();

    return *this;
}

template < typename OutputType >
const Symbolic< OutputType > &Symbolic< OutputType >::operator-=( const Symbolic< OutputType > &rhs )
{
    if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "SUB(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else if ( IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "NEG(" ) + rhs.mContent + StrCont( ")" );
    else if ( !IsEmpty() && rhs.IsEmpty() )
        DoNothingButAvoidCompilerWarning();
    else
        mContent = misc::StrCont();

    return *this;
}

template < typename OutputType >
const Symbolic< OutputType > &Symbolic< OutputType >::operator*=( const Symbolic< OutputType > &rhs )
{
    if ( IsOne() )
        mContent = rhs.mContent;
    else if ( rhs.IsOne() )
        DoNothingButAvoidCompilerWarning();
    else if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "MUL(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else
        mContent = misc::StrCont();

    return *this;
}

template < typename OutputType >
const Symbolic< OutputType > &Symbolic< OutputType >::operator/=( const Symbolic< OutputType > &rhs )
{
    if ( rhs.IsEmpty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                             "Empty symbolic instance is considered a zero, and division by zero is "
                                             "not valid" );

    if ( rhs.IsOne() )
        DoNothingButAvoidCompilerWarning();
    else if ( !IsEmpty() && !rhs.IsEmpty() )
        mContent = StrCont( "DIV(" ) + mContent + StrCont( "," ) + rhs.mContent + StrCont( ")" );
    else
        mContent = misc::StrCont();

    return *this;
}

template < typename OutputType >
void Symbolic< OutputType >::DoNothingButAvoidCompilerWarning() const
{
}


}    // namespace symbolic

/*
template < typename OutputType >
bool operator==( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    size_t n = std::max( strlen( lhs.GetString() ), strlen( rhs.GetString() ) );
    return strncmp( lhs.GetString(), rhs.GetString(), n ) == 0;
}

template < typename OutputType >
const symbolic::Symbolic< OutputType >
operator+( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    symbolic::Symbolic< OutputType > ret = lhs;
    ret += rhs;
    return ret;
}

template < typename OutputType >
const symbolic::Symbolic< OutputType >
operator-( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    symbolic::Symbolic< OutputType > ret = lhs;
    ret -= rhs;
    return ret;
}

template < typename OutputType >
const symbolic::Symbolic< OutputType >
operator*( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    symbolic::Symbolic< OutputType > ret = lhs;
    ret *= rhs;
    return ret;
}

template < typename OutputType >
const symbolic::Symbolic< OutputType >
operator/( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    symbolic::Symbolic< OutputType > ret = lhs;
    ret /= rhs;
    return ret;
}
*/


namespace std
{
template < typename OutputType >
symbolic::Symbolic< OutputType > pow( const symbolic::Symbolic< OutputType > &lhs, const double &rhs )
{
    if ( rhs < 1.999 || rhs > 2.001 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "PowOverload" );

    misc::StrCont tmp;
    if ( !lhs.IsEmpty() )
        tmp = misc::StrCont( "SQU(" ) + lhs.GetString() + misc::StrCont( ")" );
    return symbolic::Symbolic< OutputType >( tmp );
}

template < typename OutputType >
symbolic::Symbolic< OutputType > sqrt( const symbolic::Symbolic< OutputType > &rhs )
{
    misc::StrCont tmp;
    tmp = misc::StrCont( "SQRT(" ) + rhs.GetString() + misc::StrCont( ")" );
    return symbolic::Symbolic< OutputType >( tmp );
}

template < typename OutputType >
symbolic::Symbolic< OutputType > log10( const symbolic::Symbolic< OutputType > &rhs )
{
    misc::StrCont tmp;
    tmp = misc::StrCont( "LOG10(" ) + rhs.GetString() + misc::StrCont( ")" );
    return symbolic::Symbolic< OutputType >( tmp );
}

template < typename OutputType >
symbolic::Symbolic< OutputType > ceil( const symbolic::Symbolic< OutputType > &rhs )
{
    return symbolic::Symbolic< OutputType >( rhs );
}
}    // namespace std

symbolic::Symbolic< double > sqrt( symbolic::Symbolic< double > rhs );
symbolic::Symbolic< double > log10( symbolic::Symbolic< double > rhs );


template < typename OutputType >
symbolic::Symbolic< OutputType > sqrt( symbolic::Symbolic< OutputType > &rhs )
{
    misc::StrCont tmp;
    tmp = misc::StrCont( "SQRT(" ) + rhs.GetString() + misc::StrCont( ")" );
    return tmp;
}

template < typename OutputType >
symbolic::Symbolic< OutputType > sqrt( const symbolic::Symbolic< OutputType > &rhs )
{
    misc::StrCont tmp;
    tmp = misc::StrCont( "SQRT(" ) + rhs.GetString() + misc::StrCont( ")" );
    return tmp;
}

template < typename OutputType >
symbolic::Symbolic< OutputType > sqrt( symbolic::Symbolic< OutputType > rhs )
{
    misc::StrCont tmp;
    tmp = misc::StrCont( "SQRT(" ) + rhs.GetString() + misc::StrCont( ")" );
    return tmp;
}

template < typename OutputType >
symbolic::Symbolic< OutputType > log10( const symbolic::Symbolic< OutputType > &rhs )
{
    misc::StrCont tmp;
    tmp = misc::StrCont( "LOG10(" ) + rhs.GetString() + misc::StrCont( ")" );
    return symbolic::Symbolic< OutputType >( tmp );
}

template < typename OutputType >
symbolic::Symbolic< OutputType > ceil( const symbolic::Symbolic< OutputType > &rhs )
{
    return symbolic::Symbolic< OutputType >( rhs );
}


namespace symbolic
{
template < typename OutputType >
symbolic::Symbolic< OutputType > operator+( symbolic::Symbolic< OutputType > lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    lhs += rhs;
    return lhs;
}

template < typename OutputType >
symbolic::Symbolic< OutputType > operator-( symbolic::Symbolic< OutputType > lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    lhs -= rhs;
    return lhs;
}


template < typename OutputType >
symbolic::Symbolic< OutputType > operator*( const int rhs, const symbolic::Symbolic< OutputType > &lhs )
{
    symbolic::Symbolic< OutputType > returntype( lhs );
    returntype *= rhs;
    return returntype;
}


template < typename OutputType >
symbolic::Symbolic< OutputType > operator*( const symbolic::Symbolic< OutputType > &lhs, const int rhs )
{
    symbolic::Symbolic< OutputType > returntype( lhs );
    returntype *= rhs;
    return returntype;
}

template < typename OutputType >
symbolic::Symbolic< OutputType > operator*( const double rhs, const symbolic::Symbolic< OutputType > &lhs )
{
    symbolic::Symbolic< OutputType > returntype( lhs );
    returntype *= rhs;
    return returntype;
}


template < typename OutputType >
symbolic::Symbolic< OutputType > operator*( const symbolic::Symbolic< OutputType > &lhs, const double rhs )
{
    symbolic::Symbolic< OutputType > returntype( lhs );
    returntype *= rhs;
    return returntype;
}

template < typename OutputType >
symbolic::Symbolic< OutputType >
operator*( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    symbolic::Symbolic< OutputType > returntype( lhs );
    returntype *= rhs;
    return returntype;
}

template < typename OutputType >
symbolic::Symbolic< OutputType >
operator/( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    symbolic::Symbolic< OutputType > returntype( lhs );
    returntype /= rhs;
    return returntype;
}

template < typename OutputType >
symbolic::Symbolic< OutputType > abs( const symbolic::Symbolic< OutputType > &rhs )
{
    misc::StrCont tmp;
    if ( !rhs.IsEmpty() )
        tmp = misc::StrCont( "ABS(" ) + rhs.GetString() + misc::StrCont( ")" );
    return symbolic::Symbolic< OutputType >( tmp );
}
#ifndef __clang__
template < typename OutputType >
bool operator==( const Symbolic< OutputType > &lhs, const Symbolic< OutputType > &rhs )
{
    return strcmp( lhs.mContent, rhs.mContent ) == 0;
}

template < typename OutputType >
bool operator<=( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() <= rhs.size();
}

template < typename OutputType >
bool operator<( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() < rhs.size();
}

template < typename OutputType >
bool operator>( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() > rhs.size();
}

template < typename OutputType >
bool operator>=( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() >= rhs.size();
}
#endif /*__clang*/
}    // namespace symbolic

#ifdef __clang__
template < typename OutputType >
bool operator<=( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() <= rhs.size();
}

template < typename OutputType >
bool operator<( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() < rhs.size();
}

template < typename OutputType >
bool operator>( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() > rhs.size();
}

template < typename OutputType >
bool operator>=( const symbolic::Symbolic< OutputType > &lhs, const symbolic::Symbolic< OutputType > &rhs )
{
    return lhs.size() >= rhs.size();
}
#endif /* __clang__ */

#endif    // _SYMBOLIC_CLASS_H_
