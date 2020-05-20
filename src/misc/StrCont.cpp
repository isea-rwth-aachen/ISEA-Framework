#include "StrCont.h"

// STD
#include <algorithm>

// ETC
#include "../cstring/strlcpy.h"
#include "macros.h"


#ifndef __NO_STRING__
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#endif

namespace misc
{

const size_t MAX_SIZE = 102400;

StrCont::StrCont() { mContent.resize( 1, '\0' ); }

StrCont::StrCont( const char *inputString )
{
#ifdef __NO_STRING__
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
#else
    mContent = std::string( inputString );
#endif
}

StrCont::StrCont( const int &inputInt )
{
#ifdef __NO_STRING__
    char inputString[MAX_SIZE] = {0};
    real_snprintf( inputString, MAX_SIZE, "%d", inputInt );
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
#else
    mContent = boost::lexical_cast< std::string >( inputInt );
#endif
}

StrCont::StrCont( const size_t &inputInt )
{
#ifdef __NO_STRING__
    char inputString[MAX_SIZE] = {0};
    real_snprintf( inputString, MAX_SIZE, "%lu", inputInt );
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
#else
    mContent = boost::lexical_cast< std::string >( inputInt );
#endif
}

StrCont::StrCont( const std::ptrdiff_t &inputInt )
{
#ifdef __NO_STRING__
    char inputString[MAX_SIZE] = {0};
    real_snprintf( inputString, MAX_SIZE, "%l", inputInt );
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );
#else
    mContent = boost::lexical_cast< std::string >( inputInt );
#endif
}

StrCont::StrCont( const double &inputDouble )
{
#ifdef __NO_STRING__
    char inputString[MAX_SIZE] = {0};
    real_snprintf( inputString, MAX_SIZE, "%f", inputDouble );
    const size_t sizeOfBuffer = strnlen( inputString, MAX_SIZE ) + 1;
    mContent.resize( sizeOfBuffer, '\0' );
    strlcpy( &( mContent[0] ), inputString, sizeOfBuffer );

#else
    mContent = boost::str( boost::format( "%f" ) % inputDouble );
//    mContent = boost::lexical_cast< std::string >( inputDouble );
#endif
}

StrCont::~StrCont() {}

StrCont::operator const char *() const { return &( mContent[0] ); }

StrCont &StrCont::operator+=( const StrCont &rhs )
{
    mContent.reserve( mContent.size() + rhs.mContent.size() - 1 );
#ifdef __NO_STRING__
    mContent.pop_back();
#endif
    mContent.insert( mContent.end(), rhs.mContent.begin(), rhs.mContent.end() );
    return *this;
}

StrCont operator+( const StrCont &lhs, const StrCont &rhs )
{
    StrCont tmp( lhs );
    tmp += rhs;
    return tmp;
}

void StrCont::swap( StrCont &rhs ) { std::swap( mContent, rhs.mContent ); }

}    // namespace misc
