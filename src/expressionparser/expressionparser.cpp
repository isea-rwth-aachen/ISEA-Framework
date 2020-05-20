// expressionparser.cpp
// Autor: Christopher

#include "expressionparser.h"
#include "../exceptions/error_proto.h"

namespace expressionparser
{

ExpressionParser::ExpressionParser()
    : mExpressionPtr( new exprtk::expression< double > )
{}


ExpressionParser::ExpressionParser( const std::string& expressionString, const std::vector< std::pair< const char*, double& > >& variables )
    : mExpressionPtr( new exprtk::expression< double > )
{
    this->AddContent( expressionString, variables );
}

ExpressionParser::~ExpressionParser()
{
}

void ExpressionParser::AddContent( const std::string& expressionString, const std::vector< std::pair< const char*, double& > >& variables )
{
    exprtk::symbol_table< double > symbolTable;

    for( size_t i = 0; i < variables.size(); ++i )
    {
        symbolTable.add_variable( variables[i].first, variables[i].second );
    }

    mExpressionPtr->register_symbol_table( symbolTable );

    exprtk::parser< double > parser;
    parser.compile( expressionString, *mExpressionPtr.get() );
}

double ExpressionParser::GetSolution() const
{
    if( !exprtk::is_valid( *mExpressionPtr.get() ) )
        ErrorFunction< std::runtime_error >(__FUNCTION__,__LINE__,__FILE__,"ExpressionSolverInvalidSolutionException");


    return mExpressionPtr->value();
}

} // End namespace expressionparser


