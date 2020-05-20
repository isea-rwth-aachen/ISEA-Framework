/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : error_proto.h
* Creation Date : 15-10-2015
* Last Modified : Do 15 Okt 2015 13:16:34 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ERROR_PROTO_
#define _ERROR_PROTO_
#include <stdexcept>

namespace factory
{
class NoChildrenException;
class ClassNotFoundException;
class ClassNotCachedException;
}
namespace observer
{
class CantOpenFile;
}
namespace misc
{
class StrCont;
}

template < typename ErrorType >
void ErrorFunction( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

/// extern defintions for link time optimization
extern template void
ErrorFunction< std::runtime_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

extern template void
ErrorFunction< std::logic_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

extern template void
ErrorFunction< std::out_of_range >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

extern template void
ErrorFunction< std::range_error >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

extern template void ErrorFunction< factory::ClassNotFoundException >( const char *fct, const int line, const char *file,
                                                                       const char *XML_FEHLER_ID, ... );

extern template void ErrorFunction< factory::ClassNotCachedException >( const char *fct, const int line, const char *file,
                                                                        const char *XML_FEHLER_ID, ... );

extern template void ErrorFunction< factory::NoChildrenException >( const char *fct, const int line, const char *file,
                                                                    const char *XML_FEHLER_ID, ... );

extern template void
ErrorFunction< misc::StrCont >( const char *fct, const int line, const char *file, const char *XML_FEHLER_ID, ... );

extern template void ErrorFunction< observer::CantOpenFile >( const char *fct, const int line, const char *file,
                                                              const char *XML_FEHLER_ID, ... );

#endif /* _ERROR_PROTO_ */
