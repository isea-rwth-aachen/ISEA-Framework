/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matio_file.cpp
* Creation Date : 06-07-2015
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#ifndef _DS1006
#include "matio_file.h"
// STD
#include <map>
// BOOST
#include <boost/algorithm/string.hpp>

// ETC
#include "../exceptions/error_proto.h"
#include <chrono>
#include <thread>
#include <iostream>


namespace matlab
{

MatFile::MatFile()
    : mMatFile( 0 )
    , mRootVar( 0 )
    , mFileOp( MAT_ACC_RDONLY ){};

MatFile::MatFile( std::string filename, mat_acc op )
    : mMatFile( 0 )
    , mRootVar( 0 )
    , mFileOp( op )
{
    if ( filename == "" )
        return;    // graceful exit
    if ( op == MAT_ACC_RDONLY )
    {
        mMatFile = Mat_Open( filename.c_str(), op );
    }
    else
    {
        size_t counter = 0;
        while ( !mMatFile )
        {
            try
            {
                mMatFile = Mat_Create( filename.c_str(), NULL );
            }
            catch ( ... )
            {
                ++counter;
                std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
            }
            if ( counter > 10 )
            {
                std::cerr << "Couldn't open file retrying " << std::endl;
                break;
            }
        }
    }
    if ( !mMatFile )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknowFile", filename.c_str() );
}

MatFile::~MatFile()
{
    if ( mRootVar )
    {
        Mat_VarFree( mRootVar );
        mRootVar = 0;
    }

    WriteData();

    if ( mMatFile )
    {
        Mat_Close( mMatFile );
        mMatFile = 0;
    }
}

MatioData &MatFile::operator[]( std::string variableName )
{
    if ( !mMatFile )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "FileNotWorking" );

    if ( mRootVar )
    {
        Mat_VarFree( mRootVar );
    }

    std::vector< std::string > tokens;
    boost::split( tokens, variableName, boost::is_any_of( "." ) );
    if ( tokens.empty() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "EmptyToken" );

    matvar_t *cell;
    cell = Mat_VarRead( mMatFile, tokens[0].c_str() );    // <-- allocates new memory and needs to be deleted
    if ( !cell )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnfindableToken", tokens[0].c_str() );

    mRootVar = cell;

    for ( size_t i = 1; i < tokens.size(); ++i )
    {

        cell = Mat_VarGetStructFieldByName( cell, tokens[i].c_str(), 0 );
        if ( !cell )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnfindableToken", tokens[i].c_str() );
    }
    mReturnMember.reset( new MatioData( cell ) );
    return *mReturnMember;
}

MatFile &MatFile::operator<<( MatioData matData )
{
    mLazyWriteVec.push_back( matData );
    return *this;
}

void MatFile::WriteData()
{

    if ( mFileOp == MAT_ACC_RDONLY || mLazyWriteVec.empty() )
        return;
    mWriteTree.reset( new MatioWriteTree( mLazyWriteVec ) );
    std::vector< matvar_t * > data = mWriteTree->GetLinearizedTree();

    for ( size_t i = 0; i < data.size(); ++i )
    {
        Mat_VarWrite( mMatFile, data[i], MAT_COMPRESSION_ZLIB );
        Mat_VarFree( data[i] );
    }
}

} /* matlab */

#endif /* _DS1006 */
