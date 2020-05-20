/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : uniqueTokenizerVisitor.h
* Creation Date : 24-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _UNIQUETOKENIZERVISITOR_
#define _UNIQUETOKENIZERVISITOR_

//#include "../symbolicAst.h"
#include "symbolicVisitor.h"
#include <vector>
#include <map>

namespace symbolic
{
namespace ast
{
struct UniqueVisitor : public boost::static_visitor<>
{
    typedef std::pair< std::string, Expression * > str2Expression;
    void operator()( Constant & ){};
    void operator()( Variable & ){};

    template < class T >
    void operator()( T & )
    {
    }

    template < template < class > class T, class R >
    void operator()( T< R > &i )
    {
        std::stringstream x;
        SymbolicVisitor symV( &x );
        symV( i );


        // Depth first to get linearized tree
        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            boost::apply_visitor( *this, i.params.at( j ) );
            // mReverseMap[&i] = &i.params.at( j );
        }

        this->mUniqueTokenz[x.str()].push_back( (Expression *)&i );
    }

    void CreateOutput()
    {
        for ( const auto &i : mUniqueTokenz )
        {
            std::cerr << i.first << " has " << i.second.size() << " entries" << std::endl;
        }
    }

    ~UniqueVisitor() { CreateOutput(); }
    std::map< std::string, std::vector< Expression * > > mUniqueTokenz;
    // std::map< Expression *, Expression * > mReverseMap;
};
}
}
#endif /* _UNIQUETOKENIZERVISITOR_ */
