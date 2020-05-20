/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMakeLookUp.cpp
* Creation Date : 21-02-2014
* Last Modified : Di 13 Okt 2015 18:21:38 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestMakeLookUp.h"
#include "../../convert/string2lookup.h"

// STD
#include <string>
void TestMakeLookUp::testCorrectLookup(){{std::string correctstring( "1,2,3;4,5,6;7,8,9" );
std::vector< std::vector< int > > blobb = convert::Make2DLookupTable< int >( correctstring );
TS_ASSERT_EQUALS( blobb.at( 0 ).at( 0 ), 1 );
TS_ASSERT_EQUALS( blobb.at( 0 ).at( 1 ), 2 );
TS_ASSERT_EQUALS( blobb.at( 0 ).at( 2 ), 3 );

TS_ASSERT_EQUALS( blobb.at( 1 ).at( 0 ), 4 );
TS_ASSERT_EQUALS( blobb.at( 1 ).at( 1 ), 5 );
TS_ASSERT_EQUALS( blobb.at( 1 ).at( 2 ), 6 );

TS_ASSERT_EQUALS( blobb.at( 2 ).at( 0 ), 7 );
TS_ASSERT_EQUALS( blobb.at( 2 ).at( 1 ), 8 );
TS_ASSERT_EQUALS( blobb.at( 2 ).at( 2 ), 9 );
}
{
    std::string correctstring( "0.1,1.2,9.3;1.4,5,6;7,8,9" );
    std::vector< std::vector< double > > blobb = convert::Make2DLookupTable< double >( correctstring );
}
{
    std::string correctstring( "0.1,1.2,9.3,1.4,5,6,7,8,9" );
    std::vector< double > blobb = convert::Make1DLookupTable< double >( correctstring );
    TS_ASSERT_EQUALS( blobb.at( 0 ), double( 0.1 ) );
    TS_ASSERT_DELTA( blobb.at( 1 ), double( 1.2 ), 0.00001 );
    TS_ASSERT_EQUALS( blobb.at( 2 ), double( 9.3 ) );
    TS_ASSERT_DELTA( blobb.at( 3 ), double( 1.4 ), 0.00001 );
    TS_ASSERT_EQUALS( blobb.at( 4 ), double( 5. ) );
    TS_ASSERT_EQUALS( blobb.at( 5 ), double( 6. ) );
    TS_ASSERT_EQUALS( blobb.at( 6 ), double( 7. ) );
    TS_ASSERT_EQUALS( blobb.at( 7 ), double( 8. ) );
    TS_ASSERT_EQUALS( blobb.at( 8 ), double( 9. ) );
}
}
;
void TestMakeLookUp::testErrorLookup(){{std::string badstring( "1,3;4,5,6;7,8,9" );
#ifdef __EXCEPTIONS__
TS_ASSERT_THROWS( std::vector< std::vector< int > > blobb = convert::Make2DLookupTable< int >( badstring ), const std::runtime_error& );
#endif
}
{
    std::string badstring( "1,3;4,5,6;7,8,9" );
#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS( std::vector< int > blobb = convert::Make1DLookupTable< int >( badstring ), const std::bad_cast& );
#endif
}
}
;
void TestMakeLookUp::testBadInput()
{
    std::string badstring( "1,3asd;4,5,6;7,8,9" );
#ifdef __EXCEPTIONS__
    TS_ASSERT_THROWS( std::vector< std::vector< int > > blobb = convert::Make2DLookupTable< int >( badstring ), const std::bad_cast& );
#endif
};

void TestMakeLookUp::testForBadness()
{
    {
        std::string correctstring( "1,2,3;4,5,6;7,8,9" );
        std::vector< std::vector< int > > blobb = convert::Make2DLookupTable< int >( correctstring );

        TS_ASSERT_EQUALS( blobb.at( 0 ).at( 0 ), 1 );
        TS_ASSERT_EQUALS( blobb.at( 0 ).at( 1 ), 2 );
        TS_ASSERT_EQUALS( blobb.at( 0 ).at( 2 ), 3 );

        TS_ASSERT_EQUALS( blobb.at( 1 ).at( 0 ), 4 );
        TS_ASSERT_EQUALS( blobb.at( 1 ).at( 1 ), 5 );
        TS_ASSERT_EQUALS( blobb.at( 1 ).at( 2 ), 6 );

        TS_ASSERT_EQUALS( blobb.at( 2 ).at( 0 ), 7 );
        TS_ASSERT_EQUALS( blobb.at( 2 ).at( 1 ), 8 );
        TS_ASSERT_EQUALS( blobb.at( 2 ).at( 2 ), 9 );

#ifdef __EXCEPTIONS__
        TS_ASSERT_THROWS( blobb.at( 3 ).at( 3 ), const std::out_of_range& );
#endif
        TS_ASSERT_DIFFERS( blobb.at( 0 ).at( 0 ), 11 );
    }
    {
        std::string correctstring( "0.1,1.2,9.3,1.4,5,6,7,8,9" );
        std::vector< double > blobb = convert::Make1DLookupTable< double >( correctstring );
        TS_ASSERT_EQUALS( blobb.at( 0 ), 0.1 );
        TS_ASSERT_DELTA( blobb.at( 1 ), 1.2, 0.00001 );
        TS_ASSERT_EQUALS( blobb.at( 2 ), 9.3 );
        TS_ASSERT_DELTA( blobb.at( 3 ), 1.4, 0.00001 );
        TS_ASSERT_EQUALS( blobb.at( 4 ), 5 );
        TS_ASSERT_EQUALS( blobb.at( 5 ), 6 );
        TS_ASSERT_EQUALS( blobb.at( 6 ), 7 );
        TS_ASSERT_EQUALS( blobb.at( 7 ), 8 );
        TS_ASSERT_EQUALS( blobb.at( 8 ), 9 );

#ifdef __EXCEPTIONS__
        TS_ASSERT_THROWS( blobb.at( 9 ), const std::out_of_range& );
#endif
        TS_ASSERT_DIFFERS( blobb.at( 0 ), 11 );
    }
}
