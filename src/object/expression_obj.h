#ifndef _EXPRESSION_OBJ_
#define _EXPRESSION_OBJ_

#include "../../libraries/exprtk/exprtk.hpp"
#include "../state/state.h"
#include "object.h"
#include <boost/shared_ptr.hpp>

namespace object
{

/// Object that gets its value from a mathematical expression. Uses the exprtk library.
template < typename T >
class ExpressionObject : public Object< T >
{
    public:
    using ParameterT = std::pair< std::string, boost::shared_ptr< state::State > >;
    ExpressionObject( const std::string& expressionString, const std::vector< ParameterT >& parameters );
    T GetValue() const override;
    inline T operator()() const override;
    /// needed to add the default parameters for calendric and cyclical aging
    void AddParameters( const std::vector< ParameterT >& parameters );

    private:
    void CompileExpression();

    std::string mExpressionString;
    exprtk::expression< T > mExpression;
    std::vector< ParameterT > mParameters;
    /// exprtk has no way to add a variable as const, so we need a non-const copy of each state value
    mutable std::vector< T > mVariables;
};

template < typename T >
ExpressionObject< T >::ExpressionObject( const std::string& expressionString, const std::vector< ParameterT >& parameters )
    : mExpressionString( expressionString )
    , mParameters( parameters )
    , mVariables( std::vector< double >( parameters.size() ) )
{
    CompileExpression();
}

template < typename T >
T ExpressionObject< T >::GetValue() const
{
    for ( size_t i = 0; i < mParameters.size(); ++i )
    {
        mVariables[i] = mParameters[i].second->GetValue();
    }
    return mExpression.value() * this->mFactor;
}

template < typename T >
T ExpressionObject< T >::operator()() const
{
    return GetValue();
}

template < typename T >
void ExpressionObject< T >::AddParameters( const std::vector< ParameterT >& parameters )
{
    mParameters.insert( mParameters.end(), parameters.begin(), parameters.end() );
    mVariables.resize( mParameters.size() );
    CompileExpression();
}

template < typename T >
void ExpressionObject< T >::CompileExpression()
{
    exprtk::symbol_table< T > symbolTable;
    for ( size_t i = 0; i < mParameters.size(); ++i )
    {
        symbolTable.add_variable( mParameters[i].first, mVariables[i] );
    }
    mExpression.register_symbol_table( symbolTable );
    exprtk::parser< T > parser;
    parser.compile( mExpressionString, mExpression );
}

}    // namespace object

#endif