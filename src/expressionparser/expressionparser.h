// expressionparser.h
// Autor: Christopher

#ifndef _EXPRESSIONPARSER_
#define _EXPRESSIONPARSER_

#include "../../libraries/exprtk/exprtk.hpp"

#include <memory>
#include <string>

namespace expressionparser
{

/// This class calculates a solution for a given formula
class ExpressionParser
{
    public:
    // Constructor
    ExpressionParser();

    /// Constructor
    ExpressionParser( const std::string& expressionString, const std::vector< std::pair< const char*, double& > >& variables );

    /// Destructor
    ~ExpressionParser();

    /// Add a variable via a pair of the variables name as const char* and the variables values as double&
    void AddContent( const std::string& expressionString, const std::vector< std::pair< const char*, double& > >& variables );

    /// Returns the solution for given formula and acutal values of the inputted variables
    double GetSolution() const;

    private:
    std::shared_ptr< exprtk::expression< double > > mExpressionPtr;
};
}    // namespace expressionparser
#endif
