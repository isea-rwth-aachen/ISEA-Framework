/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matio_data.cpp
* Creation Date : 06-07-2015
* Last Modified : Di 13 Okt 2015 17:19:39 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _DS1006
#include "matio_data.h"

// STD
#include <exception>

// BOOST
#include <boost/algorithm/string.hpp>

// ETC
#include "../exceptions/error_proto.h"

namespace matlab
{

MatioData::MatioData() {}

MatioData::MatioData( matvar_t *matvar )
    : mMatlabVar( matvar )
{
    if ( !matvar )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable" );
    mName = matvar->name;
};

MatioData::MatioData( typename DataVector< 1 >::DataT &data, std::string name ) { LoadFromVector< 1 >( data, name ); }

MatioData::MatioData( typename DataVector< 2 >::DataT &data, std::string name ) { LoadFromVector< 2 >( data, name ); }

MatioData::MatioData( std::string &data, std::string name )
{
    size_t dims[2] = {1, data.size() + 1};
    mMatlabVar = Mat_VarCreate( SetNames( name ).c_str(), MAT_C_CHAR, MAT_T_UTF8, 2, dims, &data[0], 0 );
}

std::string MatioData::SetNames( std::string &name )
{
    size_t found = name.find_last_of( "." );

    std::string matvarName;
    if ( found == std::string::npos )
    {
        mName = "";
        matvarName = name;
    }
    else
    {
        std::string tmpstring = name.substr( 0, found );    // <-- can't be done without the tmp variable
        mName = tmpstring.c_str();
        matvarName = name.substr( found + 1, std::string::npos );    // +1 because of '.'
    }
    name = matvarName;
    return name;
}

void MatioData::CheckSanity() const
{
    if ( !mMatlabVar )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable" );
}

double &MatioData::operator()( size_t x, size_t y )
{
    CheckSanity();
    CheckOutOfBoundAccess( x, y );
    size_t offset = ( mMatlabVar->dims[X_DIM] ) * y + x;
    return static_cast< double * >( mMatlabVar->data )[offset];
}

void MatioData::CheckOutOfBoundAccess( size_t x, size_t y ) const
{

    if ( x > mMatlabVar->dims[X_DIM] )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", x, mMatlabVar->dims[X_DIM] );

    if ( y > mMatlabVar->dims[Y_DIM] )
        ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "OutOfBound", y, mMatlabVar->dims[Y_DIM] );
}

double MatioData::operator()( size_t x, size_t y ) const
{
    CheckSanity();
    CheckOutOfBoundAccess( x, y );
    size_t offset = ( mMatlabVar->dims[X_DIM] ) * y + x;
    return static_cast< double * >( mMatlabVar->data )[offset];
}

MatioData &MatioData::operator[]( std::string variableName )
{
    if ( !mMatlabVar )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable" );

    std::vector< std::string > tokens;
    boost::split( tokens, variableName, boost::is_any_of( "." ) );

    if ( tokens.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyToken" );

    matvar_t *cell = Mat_VarGetStructFieldByName( mMatlabVar, tokens[0].c_str(), 0 );

    if ( !cell )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnfindableToken", tokens[0].c_str() );


    for ( size_t i = 1; i < tokens.size(); ++i )
    {

        cell = Mat_VarGetStructFieldByName( cell, tokens[i].c_str(), 0 );
        if ( !cell )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnfindableToken", tokens[i].c_str() );
    }

    mReturnMember.reset( new MatioData( cell ) );
    return *mReturnMember;
}

template <>
void MatioData::LoadFromVector< 1 >( typename DataVector< 1 >::DataT &data, std::string name )
{
    size_t dims[2];
    dims[0] = 1;
    GetDimensions< 1 >( data, dims + 1 );
    mMatlabVar = Mat_VarCreate( SetNames( name ).c_str(), MAT_C_DOUBLE, MAT_T_DOUBLE, 2, dims, &data[0], 0 );
}

template <>
void CopyData< 1 >( typename DataVector< 1 >::DataT &vecIn, std::vector< double > &vecOut, size_t *dims, size_t position, size_t stepsize )
{
    size_t length = vecIn.size();
    if ( length != dims[0] )
    {
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "matioEmptyVariable", length, dims[0] );
    }
    for ( size_t i = 0; i < length; ++i )
    {
        vecOut[position + i * stepsize] = vecIn[i];
    }
}

template <>
void GetDimensions< 1 >( typename DataVector< 1 >::DataT &data, size_t *dims )
{
    dims[0] = data.size();
}

}    // namespace matlab

#endif /* _DS1006 */
