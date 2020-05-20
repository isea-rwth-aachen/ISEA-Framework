/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSlice.cpp
* Creation Date : 08-04-2015
* Last Modified : Mi 15 Apr 2015 11:33:58 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestSlice.h"
#include "../../misc/slice.h"

template < typename T >
void TestVariousNumericTypes()
{
    const size_t sizeOfArray = 10;
    T x[sizeOfArray];
    std::vector< T * > vec;
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        x[i] = i;
        vec.push_back( &x[i] );
    }
    SliceView< T > xSlice( vec );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        TS_ASSERT_EQUALS( xSlice[i], x[i] );
    }
}


void TestSlice::TestSliceCreation()
{
    TestVariousNumericTypes< double >();
    TestVariousNumericTypes< float >();
    TestVariousNumericTypes< int >();
    TestVariousNumericTypes< size_t >();
    TestVariousNumericTypes< char >();
}

void TestSlice::TestSliceEditing()
{
    const size_t sizeOfArray = 10;
    double x[sizeOfArray];
    std::vector< double * > vec;
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        x[i] = i;
        vec.push_back( &x[i] );
    }
    SliceView< double > slice( vec );
    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        slice[sizeOfArray - i - 1] = i;
    }

    for ( size_t i = 0; i < sizeOfArray; ++i )
    {
        TS_ASSERT_EQUALS( x[i], slice[i] );
        TS_ASSERT_EQUALS( slice[i], sizeOfArray - i - 1 );
    }
}
