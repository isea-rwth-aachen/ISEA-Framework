#include "TestStrCont.h"
#include "../../misc/StrCont.h"
#include <cstring>

static const double sDelta = 0.000001;
using misc::StrCont;


void TestStrCont::TestStringContainer()
{
    char testCharArray[30];
    testCharArray[0] = 'D';
    testCharArray[1] = 'A';
    testCharArray[2] = 'S';
    testCharArray[3] = '_';
    testCharArray[4] = 'I';
    testCharArray[5] = 'S';
    testCharArray[6] = 'T';
    testCharArray[7] = '_';
    testCharArray[8] = 'E';
    testCharArray[9] = 'I';
    testCharArray[10] = 'N';
    testCharArray[11] = '_';
    testCharArray[12] = 'T';
    testCharArray[13] = 'E';
    testCharArray[14] = 'S';
    testCharArray[15] = 'T';
    testCharArray[16] = '\0';

    StrCont stringContainer( testCharArray );

#ifdef __NO_STRING__
    TS_ASSERT_EQUALS( stringContainer.mContent.size(), 17 );
#else
    TS_ASSERT_EQUALS( stringContainer.mContent.size(), 16 );
#endif
    TS_ASSERT_EQUALS( strncmp( stringContainer, "DAS_IST_EIN_TEST", 17 ), 0 );
    // TS_ASSERT(stringContainer == StrCont("DAS_IST_EIN_TEST") );
    TS_ASSERT_DIFFERS( strncmp( stringContainer, "DAS_IST_EIN_FEHLSCHLAG", 23 ), 0 );
    // TS_ASSERT( ! (stringContainer == StrCont("DAS_IST_EIN_FEHLSCHLAG")) );
    TS_ASSERT_EQUALS( strlen( stringContainer ), 16 );
    TS_ASSERT_EQUALS( &stringContainer[0], &stringContainer.mContent.at( 0 ) );
#ifdef __NO_STRING__
    TS_ASSERT_EQUALS( stringContainer.mContent.back(), '\0' );
#else
    TS_ASSERT_EQUALS( stringContainer.mContent.back(), 'T' );
#endif


    StrCont empty( "" );
    TS_ASSERT_EQUALS( empty[0], '\0' );
#ifdef __NO_STRING__
    TS_ASSERT_EQUALS( empty.mContent.size(), 1 );
    TS_ASSERT_EQUALS( &empty[0], &empty.mContent.at( 0 ) );
#endif
    TS_ASSERT_EQUALS( empty.mContent.back(), '\0' );

    StrCont added = empty + StrCont( "Heyho," );
    TS_ASSERT_EQUALS( strncmp( added, "Heyho,", 7 ), 0 );
// TS_ASSERT(added == StrCont("Heyho,") );
#ifdef __NO_STRING__
    TS_ASSERT_EQUALS( added.mContent.size(), 7 );
#else
    TS_ASSERT_EQUALS( added.mContent.size(), 6 );
#endif
    TS_ASSERT_EQUALS( &added[0], &added.mContent.at( 0 ) );
#ifdef __NO_STRING__
    TS_ASSERT_EQUALS( added.mContent.back(), '\0' );
#else
    TS_ASSERT_EQUALS( added.mContent.back(), ',' );
#endif

    added += StrCont( " let's go!" );
    TS_ASSERT_EQUALS( strncmp( added, "Heyho, let's go!", 17 ), 0 );
// TS_ASSERT(added == StrCont("Heyho, let's go!") );
#ifdef __NO_STRING__
    TS_ASSERT_EQUALS( added.mContent.size(), 17 );
#else
    TS_ASSERT_EQUALS( added.mContent.size(), 16 );
#endif
    TS_ASSERT_EQUALS( &added[0], &added.mContent.at( 0 ) );
#ifdef __NO_STRING__
    TS_ASSERT_EQUALS( added.mContent.back(), '\0' );
#else
    TS_ASSERT_EQUALS( added.mContent.back(), '!' );
#endif


    TS_ASSERT_DELTA( atof( StrCont( 5.1 ) ), 5.1, sDelta );
    TS_ASSERT_DELTA( atof( StrCont( -0.23 ) ), -0.23, sDelta );
    TS_ASSERT_EQUALS( atoi( StrCont( 6 ) ), 6 );
    TS_ASSERT_EQUALS( atoi( StrCont( -11 ) ), -11 );
}
