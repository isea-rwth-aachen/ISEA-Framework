/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : error_function.cpp
* Creation Date : 15-10-2015
* Last Modified : Do 15 Okt 2015 13:14:21 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "error_function.h"

// STD
#include <exception>

// ETC
#include "../misc/StrCont.h"
#include "../observer/observerException.h"
#include "../factory/exceptions/classnotfoundexception.h"
#include "../factory/exceptions/nochildrenexception.h"
#include "../factory/exceptions/classnotcachedexception.h"


template void ErrorFunction< std::runtime_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

template void ErrorFunction< std::logic_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

template void ErrorFunction< std::out_of_range >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

template void ErrorFunction< std::range_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );


template void ErrorFunction< misc::StrCont >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

template void ErrorFunction< observer::CantOpenFile >( const char *fct, const int line, const char *file,
                                                       const char *XML_FEHLER_ID, ... );

template void ErrorFunction< factory::ClassNotFoundException >( const char *fct, const int line, const char *file,
                                                                const char *XML_FEHLER_ID, ... );

template void ErrorFunction< factory::NoChildrenException >( const char *fct, const int line, const char *file,
                                                             const char *XML_FEHLER_ID, ... );

template void ErrorFunction< factory::ClassNotCachedException >( const char *fct, const int line, const char *file,
                                                                 const char *XML_FEHLER_ID, ... );
