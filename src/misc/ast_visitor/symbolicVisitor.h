/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name :
* Creation Date : 23-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLIC_VISITOR_
#define _SYMBOLIC_VISITOR_
#include "../symbolicAst.h"

namespace symbolic
{
namespace ast
{
/// This class derives a symbolic representation of the AST
// template < std::ostream *stream = &std::cout >
struct SymbolicVisitor : public boost::static_visitor<>
{
    SymbolicVisitor( std::ostream *strea = &std::cout )
        : mStream( strea ){};

    std::ostream *mStream = 0;

    void operator()( Constant &i ) { ( *mStream ) << i.name; }
    void operator()( Variable &i ) { ( *mStream ) << i.name; }

    template < class T >
    void operator()( T & )
    {
    }

    template < template < class > class T, class R >
    void operator()( T< R > &i )
    {
        ( *mStream ) << i.funct.name << "(";
        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            if ( j > 0 )
            {
                ( *mStream ) << ",";
            }
            boost::apply_visitor( *this, i.params.at( j ) );
        }
        ( *mStream ) << ")";
    }
};
}
}

#endif /* _SYMBOLIC_VISITOR_ */
