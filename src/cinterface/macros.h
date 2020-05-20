/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : macros.h
* Creation Date : 24-02-2015
* Last Modified : Do 03 Mär 2016 16:05:19 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MACROS_
#define _MACROS_

// Printer and error handling function
#ifdef _DS1006
#define PrintFunction( fmt, ... ) printf( fmt, ##__VA_ARGS__ );
#define ErrorFunction( fmt, ... ) \
    printf( fmt, ##__VA_ARGS__ ); \
    abort();
#else
#define PrintFunction( fmt, ... ) mexPrintf( fmt, ##__VA_ARGS__ );
#define ErrorFunction( fmt, ... ) mexErrMsgIdAndTxt( "Error:Error", fmt, ##__VA_ARGS__ );
#endif

/// This macro disables warnings about unused variabels and is portable. The compiler will optimize this loop
#define UNUSED( expr )    \
    do                    \
    {                     \
        ( void )( expr ); \
    } while ( 0 )

#endif /* _MACROS_ */
