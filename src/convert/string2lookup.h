/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : string2lookup.h
* Creation Date : 31-10-2012
* Last Modified : Do 15 Okt 2015 12:15:56 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _PARSER2LOOKUP_
#define _PARSER2LOOKUP_

#ifndef __NO_STRINGS__
// STD
#include <string>
#include <vector>

// BOOST
#ifndef BOOST_NO_TYPEID
#include <boost/lexical_cast.hpp>
#else
#include <cstdlib>
#endif

#include <boost/algorithm/string.hpp>

// ETC
#include "../exceptions/error_proto.h"

// This namespace is about converting data from one type into another
namespace convert
{
/// Make1DLookupTable will transform a string into a Container< TYPE > for 1D arrays
template < template < class > class T, class C >
T< C > Make1DLookupTable( const std::string& toParse )
{
    T< C > mat;
    std::vector< std::string > rows;
    boost::split( rows, toParse, boost::is_any_of( "," ) );
    for ( size_t i = 0; i < rows.size(); ++i )
#ifndef BOOST_NO_TYPEID
        mat << boost::lexical_cast< C >( rows.at( i ) );
#else
        mat << std::atof( rows.at( i ).c_str() );
#endif
    return mat;
}

/// Make1DLookupTable will transform a string into a std::vector< TYPE > for 1D arrays
template < typename C >
std::vector< C > Make1DLookupTable( const std::string& toParse )
{
    std::vector< C > result;
    std::vector< std::string > rows;
    boost::split( rows, toParse, boost::is_any_of( "," ) );
    for ( size_t i = 0; i < rows.size(); ++i )
#ifndef BOOST_NO_TYPEID
        result.push_back( boost::lexical_cast< C >( rows.at( i ) ) );
#else
        result.push_back( std::atof( rows.at( i ).c_str() ) );
#endif
    return result;
};

/// Make2DLookupTable will transform a string into a std::vector< TYPE > for 2D arrays
/// Seperators are ',' for colums and ';' for rows
template < typename T >
std::vector< std::vector< T > > Make2DLookupTable( const std::string& toParse )
{
    std::vector< std::vector< T > > result;
    std::vector< std::string > rows;
    boost::split( rows, toParse, boost::is_any_of( ";" ) );
    int maxelements = -1;
    for ( size_t i = 0; i < rows.size(); ++i )
    {
        result.push_back( std::vector< T >() );
        std::vector< std::string > elements;
        boost::split( elements, rows.at( i ), boost::is_any_of( "," ) );
        for ( size_t j = 0; j < elements.size(); ++j )
        {
            if ( maxelements == -1 )
                maxelements = int( elements.size() );
            else
            {
                if ( maxelements != int( elements.size() ) )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "AsymetricLookup",
                                                         elements.at( j ).c_str() );
            }
#ifndef BOOST_NO_TYPEID
            result.at( i ).push_back( boost::lexical_cast< T >( elements.at( j ) ) );
#else
            result.at( i ).push_back( atof( elements.at( j ).c_str() ) );
#endif
        }
    }
    return result;
};

// template<template<class> class T> T< double > Make1DLookupTable(const std::string& toParse);
// template<> vector<double> Make1DLookupTable(const std::string& toParse);
// template<> vector< vector<double> > Make2DLookupTable(const std::string& toParse);

} /* END NAMESPACE */

#endif /* __NO_STRINGS__ */

#endif /* _PARSER2LOOKUP_ */
