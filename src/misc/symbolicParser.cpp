/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : symbolicParser.cpp
* Creation Date : 10-01-2017
* Last Modified : Fr 13 Jan 2017 10:35:51 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "symbolicParser.h"

template struct symbolic::SymbolicParser< std::string::const_iterator >;

void symbolic::char2doubleFct( const double j, std::string &returnvalue )
{
    // if ( j < 0 )
    //{
    //    returnvalue = "NEG(" + boost::lexical_cast< std::string >( -j ) + ")";
    //}
    // else
    {
        returnvalue = boost::lexical_cast< std::string >( j );
    }
}


template <>
void symbolic::Arithmetic< std::multiplies< const double > >( const double i, const double j, std::string &returnvalue )
{
    if ( i == 1 )
    {
        returnvalue = boost::lexical_cast< std::string >( j );
    }
    else if ( j == 1 )
    {
        returnvalue = boost::lexical_cast< std::string >( i );
    }
    else
        returnvalue = boost::lexical_cast< std::string >( std::multiplies< const double >()( i, j ) );
}

void symbolic::MulConstantFolding2( const double j, std::string &operand, std::string &returnvalue )
{
    if ( j == 1 )
    {
        returnvalue = operand;
    }
    else if ( j == -1 )
    {
        returnvalue = "NEG(" + operand + ")";
    }

    else
    {
        returnvalue = "MUL(" + boost::lexical_cast< std::string >( j ) + "," + operand + ")";
    }
}
void symbolic::MulConstantFolding( std::string &operand, const double j, std::string &returnvalue )
{
    if ( j == 1 )
    {
        returnvalue = operand;
    }
    else if ( j == -1 )
    {
        returnvalue = "NEG(" + operand + ")";
    }
    else
    {
        returnvalue = "MUL(" + operand + "," + boost::lexical_cast< std::string >( j ) + ")";
    }
}
void symbolic::Sqrt( const double i, std::string &returnvalue )
{
    returnvalue = boost::lexical_cast< std::string >( std::sqrt( i ) );
}
