/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name :
* Creation Date : 23-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _SYMBOLIC_AST_
#define _SYMBOLIC_AST_
#include "../symbolicAst.h"
#include "symbolicVisitor.h"

namespace symbolic
{
namespace ast
{
/// This class derives a dot representation of the AST
template < std::ostream *stream = &std::cout, bool maxDepth = true >
struct DotVisitor : public boost::static_visitor< std::string >
{
    typedef std::map< std::string, std::vector< Expression * > > EMap;
    std::map< std::string, size_t > colormapping;
    std::vector< std::string > color;
    DotVisitor( EMap emap = EMap() )
        : mEMap( emap )
    {
        ( *stream ) << "#dot -Tpng esb.dot > esb.png\n";
        ( *stream ) << "digraph Network\n{\n";


        color.push_back( "\"#FFFFFF\"" );
        for ( int i = 99; i > 41; i -= 10 )
        {
            for ( int j = 99; j > 41; j -= 10 )
            {
                for ( int g = 99; g > 41; g -= 10 )
                {
                    color.push_back( "\"#" + boost::lexical_cast< std::string >( i ) + boost::lexical_cast< std::string >( j ) +
                                     boost::lexical_cast< std::string >( g ) + "\"" );
                }
            }
        }
    }

    ~DotVisitor()
    {
        CreateLabels();
        ( *stream ) << "}" << std::endl;
    }
    size_t mConstantConuter = 1;
    size_t mTreeCounter = 1;

    std::string operator()( Constant &i )
    {
        const size_t count = ++counter;
        //        mLabels.push_back( "Constant" + boost::lexical_cast< std::string >( count ) + "[label=C" +
        //                           boost::lexical_cast< std::string >( mConstantConuter++ ) +    //+ i.name +
        //                           " style=\"filled\" fillcolor=" + color.at(colorCounter) + "]" );

        mLabels.push_back( "Constant" + boost::lexical_cast< std::string >( count ) + "[label=" + i.name +
                           " style=\"filled\" fillcolor=" + color.at( colorCounter ) + "]" );
        return "Constant" + boost::lexical_cast< std::string >( count );
    }

    std::string operator()( Variable &i )
    {
        const size_t count = ++counter;
        //        mLabels.push_back( "Variable" + boost::lexical_cast< std::string >( count ) + "[label=T" +
        //                           boost::lexical_cast< std::string >( mTreeCounter++ ) +    // + i.name +
        //                           " style=\"filled\" fillcolor=" + color.at(colorCounter) + "]" );
        //
        mLabels.push_back( "Variable" + boost::lexical_cast< std::string >( count ) + "[label=" + i.name +
                           " style=\"filled\" fillcolor=" + color.at( colorCounter ) + "]" );
        return "Variable" + boost::lexical_cast< std::string >( count );
    }

    template < class T >
    std::string operator()( T & )
    {
        return "";
    }


    template < template < class > class T, class R >
    std::string operator()( T< R > &i )
    {
        const size_t count = ++counter;
        ( *stream ) << i.funct.name << count << std::endl;

        std::stringstream x;
        SymbolicVisitor symV( &x );
        symV( i );

        bool hasSetColor = false;
        auto it = mEMap.find( x.str() );
        if ( it != mEMap.end() && it->second.size() > 1 )
        {
            auto cit = colormapping.find( x.str() );
            if ( ( !maxDepth || colorCounter == 0 ) && cit == colormapping.end() )
            {
                colorCounter = ( ++lastUnusedColorCounter );
                colormapping[x.str()] = colorCounter;    // Save colormapping
                hasSetColor = true;
            }
            else if ( ( !maxDepth || colorCounter == 0 ) && cit != colormapping.end() )
            {
                colorCounter = colormapping[x.str()];    // Recall colormapping
                hasSetColor = true;
            }
        }

        mLabels.push_back( i.funct.name + boost::lexical_cast< std::string >( count ) + "[label=" + i.funct.name +
                           " style=\"filled\" fillcolor=" + color.at( colorCounter ) + "]" );


        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            ( *stream ) << i.funct.name << count << " -> " << boost::apply_visitor( *this, i.params.at( j ) ) << std::endl;
        }

        if ( hasSetColor )
        {
            colorCounter = 0;
        }

        return i.funct.name + boost::lexical_cast< std::string >( count );
    }

    size_t counter = 0;

    size_t colorCounter = 0;
    size_t lastUnusedColorCounter = 0;

    void CreateLabels()
    {
        for ( const auto &i : mLabels )
        {
            ( *stream ) << i << std::endl;
        }
    }
    std::vector< std::string > mLabels;
    EMap mEMap;
};
}
}
#endif /* _FILENAME_ */
