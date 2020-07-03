#ifndef _EXPRESSION_OBJ_
#define _EXPRESSION_OBJ_

#include "../../libraries/exprtk/exprtk.hpp"
#include "object.h"


namespace object
{

/// Object that gets its value from a mathematical expression. Uses the exprtk library.
template < typename T >
class ExpressionObject : public Object< T >
{
    public:
    ExpressionObject( const exprtk::expression< T > &expression );
    T GetValue() const override;
    T operator()() const override;

    private:
    exprtk::expression< T > mExpression;
};

template < typename T >
ExpressionObject< T >::ExpressionObject( const exprtk::expression< T > &expression )
    : mExpression( expression )
{
}

template < typename T >
T ExpressionObject< T >::GetValue() const
{
    return mExpression.value();
}

template < typename T >
T ExpressionObject< T >::operator()() const
{
    return mExpression.value();
}

}    // namespace object

#endif