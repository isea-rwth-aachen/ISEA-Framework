/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name :
* Creation Date : 23-02-2018
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OPTIMIZATION_VISITOR_
#define _OPTIMIZATION_VISITOR_
#include "../symbolicAst.h"
#include "symbolicVisitor.h"
#include "../../object/object.h"
#include "../../object/const_obj.h"
#include "../../object/lookup_obj2d_with_state.h"
#include "../../object/lookup_obj1d_with_state.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace symbolic
{
namespace ast
{
typedef std::map< std::string, boost::shared_ptr< object::Object< double > > > ID_Map;

template < class R >
struct OpUnary : public boost::static_visitor< Expression >
{

    OpUnary( ID_Map *map = 0 )
        : mID2Object( map ){};


    Expression operator()( Variable &i )
    {
        // Sanity check
        if ( !mID2Object )
            return Nil();
        auto li = mID2Object->find( i.name );
        if ( li == mID2Object->end() )
        {
            return Nil();
        }

        if ( dynamic_cast< const object::ConstObj< double > * >( li->second.get() ) )
        {
            double l = static_cast< const object::ConstObj< double > * >( li->second.get() )->GetValue();
            double result = R().Converter( l );
            return Constant{boost::lexical_cast< std::string >( result )};
        }
        else if ( dynamic_cast< const object::LookupObj2dWithState< double > * >( li->second.get() ) )
        {
            auto l = static_cast< const object::LookupObj2dWithState< double > * >( li->second.get() );
            auto &lLookup2D = l->GetLookup();

            const std::vector< std::vector< double > > &lLookup = lLookup2D.GetLookup();

            auto &lrow = lLookup2D.GetLookupType()->GetPointsRow();
            auto &lcol = lLookup2D.GetLookupType()->GetPointsCol();

            std::vector< std::vector< double > > newLookup = R().Converter( lLookup );
            std::string name = "ID" + boost::lexical_cast< std::string >( mID2Object->size() ) + "_Merge";

            auto mergedObject =
             boost::make_shared< object::LookupObj2dWithState< double > >( newLookup, lrow, lcol, l->GetBoostStateRow(),
                                                                           l->GetBoostStateCol(), lLookup2D.mType );

            ( *mID2Object )[name] = mergedObject;

            return Variable{name};
        }
        else if ( dynamic_cast< const object::LookupObj1dWithState< double > * >( li->second.get() ) )
        {
            auto l = static_cast< const object::LookupObj1dWithState< double > * >( li->second.get() );
            auto &lLookup1D = l->GetLookup();

            const std::vector< double > &lLookup = lLookup1D.GetLookup();

            auto &lrow = lLookup1D.GetLookupType()->GetPoints();

            std::vector< double > newLookup = R().Converter( lLookup );
            std::string name = "ID" + boost::lexical_cast< std::string >( mID2Object->size() ) + "_Merge";

            auto mergedObject =
             boost::make_shared< object::LookupObj1dWithState< double > >( newLookup, lrow, l->GetBoostState(), lLookup1D.mType );

            ( *mID2Object )[name] = mergedObject;

            return Variable{name};
        }


        return Nil();
    }

    // UnaryVisitor
    template < typename T >
    Expression operator()( T & )
    {
        return Nil();
    }
    Expression operator()( Constant &i ) { return Constant{R().Converter( i )}; };

    ID_Map *mID2Object = 0;
};

template <>
struct OpUnary< ast::tag::Sqrt_T > : public boost::static_visitor< Expression >
{

    OpUnary( ID_Map *map = 0 )
        : mID2Object( map ){};


    typedef ast::tag::Sqrt_T R;
    Expression operator()( Variable &i )
    {
        // Sanity check
        if ( !mID2Object )
            return Nil();
        auto li = mID2Object->find( i.name );
        if ( li == mID2Object->end() )
        {
            return Nil();
        }

        if ( dynamic_cast< const object::ConstObj< double > * >( li->second.get() ) )
        {
            double l = static_cast< const object::ConstObj< double > * >( li->second.get() )->GetValue();
            double result = R().Converter( l );
            return Constant{boost::lexical_cast< std::string >( result )};
        }
        return Nil();
    }
    // UnaryVisitor
    template < typename T >
    Expression operator()( T & )
    {
        return Nil();
    }
    Expression operator()( Constant &i ) { return Constant{R().Converter( i )}; };

    ID_Map *mID2Object = 0;
};

template <>
struct OpUnary< ast::tag::Tanh_T > : public boost::static_visitor< Expression >
{

    OpUnary( ID_Map *map = 0 )
        : mID2Object( map ){};

    typedef ast::tag::Tanh_T R;

    Expression operator()( Variable &i )
    {
        // Sanity check
        if ( !mID2Object )
            return Nil();
        auto li = mID2Object->find( i.name );
        if ( li == mID2Object->end() )
        {
            return Nil();
        }

        if ( dynamic_cast< const object::ConstObj< double > * >( li->second.get() ) )
        {
            double l = static_cast< const object::ConstObj< double > * >( li->second.get() )->GetValue();
            double result = R().Converter( l );
            return Constant{boost::lexical_cast< std::string >( result )};
        }
        return Nil();
    }
    // UnaryVisitor
    template < typename T >
    Expression operator()( T & )
    {
        return Nil();
    }
    Expression operator()( Constant &i ) { return Constant{R().Converter( i )}; };

    ID_Map *mID2Object = 0;
};


template < class R >
struct OpBinary : public boost::static_visitor< Expression >
{
    OpBinary( ID_Map *map = 0 )
        : mID2Object( map ){};
    // BinaryVisitor
    template < typename T, typename U >
    Expression operator()( T &, U & )
    {
        return Nil();
    }

    Expression operator()( Variable &i, Variable &j )
    {
        // Sanity check
        if ( !mID2Object )
            return Nil();
        auto li = mID2Object->find( i.name );
        auto rj = mID2Object->find( j.name );
        if ( li == mID2Object->end() || rj == mID2Object->end() )
        {
            return Nil();
        }

        if ( dynamic_cast< const object::ConstObj< double > * >( li->second.get() ) &&
             dynamic_cast< const object::ConstObj< double > * >( rj->second.get() ) )
        {
            double l = static_cast< const object::ConstObj< double > * >( li->second.get() )->GetValue();
            double r = static_cast< const object::ConstObj< double > * >( rj->second.get() )->GetValue();
            double result = R().Converter( l, r );
            return Constant{boost::lexical_cast< std::string >( result )};
        }
        return Nil();
    };
    Expression operator()( Constant &i, Constant &j ) { return Constant{R().Converter( i, j )}; };

    ID_Map *mID2Object = 0;
};

template <>
struct OpBinary< ast::tag::Add_T > : public boost::static_visitor< Expression >
{

    typedef ast::tag::Add_T R;

    OpBinary( ID_Map *map = 0 )
        : mID2Object( map ){};
    // BinaryVisitor
    template < typename T, typename U >
    Expression operator()( T &, U & )
    {
        return Nil();
    }

    Expression operator()( Variable &i, Variable &j )
    {
        // Sanity check
        if ( !mID2Object )
            return Nil();
        auto li = mID2Object->find( i.name );
        auto rj = mID2Object->find( j.name );
        if ( li == mID2Object->end() || rj == mID2Object->end() )
        {
            return Nil();
        }

        if ( dynamic_cast< const object::ConstObj< double > * >( li->second.get() ) &&
             dynamic_cast< const object::ConstObj< double > * >( rj->second.get() ) )
        {
            double l = static_cast< const object::ConstObj< double > * >( li->second.get() )->GetValue();
            double r = static_cast< const object::ConstObj< double > * >( rj->second.get() )->GetValue();
            double result = R().Converter( l, r );
            return Constant{boost::lexical_cast< std::string >( result )};
        }
        if ( dynamic_cast< const object::LookupObj2dWithState< double > * >( li->second.get() ) &&
             dynamic_cast< const object::LookupObj2dWithState< double > * >( rj->second.get() ) )
        {
            auto l = static_cast< const object::LookupObj2dWithState< double > * >( li->second.get() );
            auto r = static_cast< const object::LookupObj2dWithState< double > * >( rj->second.get() );

            bool RowRowMatch = l->GetStateRow() == r->GetStateRow();
            bool RowColMatch = l->GetStateRow() == r->GetStateCol();
            bool ColRowMatch = l->GetStateCol() == r->GetStateRow();
            bool ColColMatch = l->GetStateCol() == r->GetStateCol();

            const size_t count = RowRowMatch + RowColMatch + ColRowMatch + ColColMatch;
            // Only this is supported
            if ( RowRowMatch && ColColMatch )
            {
                auto &lLookup2D = l->GetLookup();
                auto &rLookup2D = r->GetLookup();

                const std::vector< std::vector< double > > &lLookup = lLookup2D.GetLookup();
                const std::vector< std::vector< double > > &rLookup = rLookup2D.GetLookup();


                auto &lrow = lLookup2D.GetLookupType()->GetPointsRow();
                auto &rrow = rLookup2D.GetLookupType()->GetPointsRow();
                for ( size_t i = 0; i < lrow.size(); ++i )
                {
                    if ( lrow.at( i ) != rrow.at( i ) )
                    {
                        return Nil();    // not possible
                    }
                }

                auto &lcol = lLookup2D.GetLookupType()->GetPointsCol();
                auto &rcol = rLookup2D.GetLookupType()->GetPointsCol();
                for ( size_t i = 0; i < lcol.size(); ++i )
                {
                    if ( lcol.at( i ) != rcol.at( i ) )
                    {
                        return Nil();    // not possible
                    }
                }
                std::vector< std::vector< double > > newLookup = R().Converter( lLookup, rLookup );
                std::string name = "ID" + boost::lexical_cast< std::string >( mID2Object->size() ) + "_Merge";

                auto mergedObject =
                 boost::make_shared< object::LookupObj2dWithState< double > >( newLookup, lrow, lcol, l->GetBoostStateRow(),
                                                                               l->GetBoostStateCol(), lLookup2D.mType );

                ( *mID2Object )[name] = mergedObject;

                return Variable{name};
            }
            else if ( count == 2 )
            {
                std::cerr << "Unhandled case ..." << std::endl;    // TODO: Implment
            }
            // double result = R().Converter( l, r );
            // return Constant{boost::lexical_cast< std::string >( result )};
        }

        /*
        if ( dynamic_cast < const object::LookupObj2dWithState< double >( li ) &&
             dynamic_cast < const object::LookupObj2dWithState< double >( rj ) )
        {
        }
        */

        return Nil();
    };
    Expression operator()( Constant &i, Constant &j ) { return Constant{R().Converter( i, j )}; };

    ID_Map *mID2Object = 0;
};

template <>
struct OpBinary< ast::tag::Sub_T > : public boost::static_visitor< Expression >
{

    typedef ast::tag::Sub_T R;

    OpBinary( ID_Map *map = 0 )
        : mID2Object( map ){};
    // BinaryVisitor
    template < typename T, typename U >
    Expression operator()( T &, U & )
    {
        return Nil();
    }

    Expression operator()( Variable &i, Variable &j )
    {
        // Sanity check
        if ( !mID2Object )
            return Nil();
        auto li = mID2Object->find( i.name );
        auto rj = mID2Object->find( j.name );
        if ( li == mID2Object->end() || rj == mID2Object->end() )
        {
            return Nil();
        }

        if ( dynamic_cast< const object::ConstObj< double > * >( li->second.get() ) &&
             dynamic_cast< const object::ConstObj< double > * >( rj->second.get() ) )
        {
            double l = static_cast< const object::ConstObj< double > * >( li->second.get() )->GetValue();
            double r = static_cast< const object::ConstObj< double > * >( rj->second.get() )->GetValue();
            double result = R().Converter( l, r );
            return Constant{boost::lexical_cast< std::string >( result )};
        }
        if ( dynamic_cast< const object::LookupObj2dWithState< double > * >( li->second.get() ) &&
             dynamic_cast< const object::LookupObj2dWithState< double > * >( rj->second.get() ) )
        {
            auto l = static_cast< const object::LookupObj2dWithState< double > * >( li->second.get() );
            auto r = static_cast< const object::LookupObj2dWithState< double > * >( rj->second.get() );

            bool RowRowMatch = l->GetStateRow() == r->GetStateRow();
            bool RowColMatch = l->GetStateRow() == r->GetStateCol();
            bool ColRowMatch = l->GetStateCol() == r->GetStateRow();
            bool ColColMatch = l->GetStateCol() == r->GetStateCol();

            const size_t count = RowRowMatch + RowColMatch + ColRowMatch + ColColMatch;
            // Only this is supported
            if ( RowRowMatch && ColColMatch )
            {
                auto &lLookup2D = l->GetLookup();
                auto &rLookup2D = r->GetLookup();

                const std::vector< std::vector< double > > &lLookup = lLookup2D.GetLookup();
                const std::vector< std::vector< double > > &rLookup = rLookup2D.GetLookup();


                auto &lrow = lLookup2D.GetLookupType()->GetPointsRow();
                auto &rrow = rLookup2D.GetLookupType()->GetPointsRow();
                for ( size_t i = 0; i < lrow.size(); ++i )
                {
                    if ( lrow.at( i ) != rrow.at( i ) )
                    {
                        return Nil();    // not possible
                    }
                }

                auto &lcol = lLookup2D.GetLookupType()->GetPointsCol();
                auto &rcol = rLookup2D.GetLookupType()->GetPointsCol();
                for ( size_t i = 0; i < lcol.size(); ++i )
                {
                    if ( lcol.at( i ) != rcol.at( i ) )
                    {
                        return Nil();    // not possible
                    }
                }
                std::vector< std::vector< double > > newLookup = R().Converter( lLookup, rLookup );
                std::string name = "ID" + boost::lexical_cast< std::string >( mID2Object->size() ) + "_Merge";

                auto mergedObject =
                 boost::make_shared< object::LookupObj2dWithState< double > >( newLookup, lrow, lcol, l->GetBoostStateRow(),
                                                                               l->GetBoostStateCol(), lLookup2D.mType );

                ( *mID2Object )[name] = mergedObject;

                return Variable{name};
            }
            else if ( count == 2 )
            {
                std::cerr << "Unhandled case ..." << std::endl;    // TODO: Implment
            }
            // double result = R().Converter( l, r );
            // return Constant{boost::lexical_cast< std::string >( result )};
        }

        /*
        if ( dynamic_cast < const object::LookupObj2dWithState< double >( li ) &&
             dynamic_cast < const object::LookupObj2dWithState< double >( rj ) )
        {
        }
        */

        return Nil();
    };
    Expression operator()( Constant &i, Constant &j ) { return Constant{R().Converter( i, j )}; };

    ID_Map *mID2Object = 0;
};


struct OptimizationVisitor : public boost::static_visitor< Expression >
{
    std::map< std::string, std::vector< Expression > > mUniqueTokenz;
    ID_Map *mID2Object = 0;

    OptimizationVisitor( ID_Map *id2object = 0 )
        : mID2Object( id2object ){};


    Expression operator()( Constant &i ) { return i; }
    Expression operator()( Variable &i ) { return i; }

    template < typename T, typename U >
    Expression operator()( T &x, U &y )
    {
        /*
        boost::apply_visitor( *this, x );
        boost::apply_visitor( *this, y );
        */
        return Expression();
    }

    template < typename T >
    Expression operator()( T &, T & )
    {
        return Expression();
    }

    Expression operator()( Constant &, Constant & ) { return Expression(); }


    template < class T >
    Expression operator()( T &i )
    {
        return Expression( i );
    }

    bool IsNil( const Expression &i ) const
    {
        if ( boost::get< Nil >( &i ) )
            return true;
        return false;
    }

    bool DivPrefix( BinaryFunctionCall< ast::tag::Div_T > &i, Expression &returnExpression )
    {
        returnExpression = Nil();
        BinaryFunctionCall< ast::tag::Div_T > *exp = boost::get< BinaryFunctionCall< ast::tag::Div_T > >( &i.params.at( 1 ) );
        if ( exp )
        {
            Expression mul( BinaryFunctionCall< ast::tag::Mul_T >{"MUL", {i.params.at( 0 ), exp->params.at( 1 )}} );
            returnExpression = BinaryFunctionCall< ast::tag::Div_T >{"DIV", {mul, exp->params.at( 0 )}};
            return true;
        }
        exp = boost::get< BinaryFunctionCall< ast::tag::Div_T > >( &i.params.at( 0 ) );
        if ( exp )
        {
            Expression mul( BinaryFunctionCall< ast::tag::Mul_T >{"MUL", {exp->params.at( 1 ), i.params.at( 1 )}} );
            returnExpression = BinaryFunctionCall< ast::tag::Div_T >{"DIV", {exp->params.at( 0 ), mul}};
            return true;
        }

        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            UnaryFunctionCall< ast::tag::Neg_T > *exp = boost::get< UnaryFunctionCall< ast::tag::Neg_T > >( &i.params.at( j ) );
            if ( exp )
            {
                BinaryFunctionCall< ast::tag::Div_T > div;
                if ( j == 0 )
                    div = {"DIV", {exp->params.at( 0 ), i.params.at( 1 )}};
                else
                    div = {"DIV", {exp->params.at( 0 ), i.params.at( 0 )}};

                returnExpression = UnaryFunctionCall< ast::tag::Neg_T >( {"NEG", {div}} );
                return true;
            }
        }
        Constant *c = boost::get< Constant >( &i.params.at( 1 ) );
        if ( c )
        {
            if ( c->name == "1" )
            {
                returnExpression = i.params.at( 0 );
                return true;
            }
            std::string name;
            symbolic::char2doubleFct( 1.0 / boost::lexical_cast< double >( c->name ), name );
            Constant inverse{name};
            returnExpression = BinaryFunctionCall< ast::tag::Mul_T >{"MUL", {i.params.at( 0 ), inverse}};
            return true;
        }

        return false;
    }

    template < class R >
    bool ConstantFolding( BinaryFunctionCall< R > &i, Expression &returnExpression )
    {
        OpBinary< R > z( mID2Object );
        returnExpression = boost::apply_visitor( z, i.params.at( 0 ), i.params.at( 1 ) );
        if ( IsNil( returnExpression ) )
            return false;
        return true;
    }

    template < class R >
    bool ConstantFolding( UnaryFunctionCall< R > &i, Expression &returnExpression )
    {
        OpUnary< R > z( mID2Object );
        returnExpression = boost::apply_visitor( z, i.params.at( 0 ) );
        if ( IsNil( returnExpression ) )
            return false;
        return true;
    }

    bool NegPrefix( UnaryFunctionCall< ast::tag::Neg_T > &i, Expression &returnExpression )
    {
        UnaryFunctionCall< ast::tag::Neg_T > *exp = boost::get< UnaryFunctionCall< ast::tag::Neg_T > >( &i.params.at( 0 ) );
        if ( exp )
        {
            returnExpression = exp->params.at( 0 );
            return true;
        }
        return false;
    }

    bool MulPrefix( BinaryFunctionCall< ast::tag::Mul_T > &i, Expression &returnExpression )
    {
        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            {
                UnaryFunctionCall< ast::tag::Neg_T > *exp =
                 boost::get< UnaryFunctionCall< ast::tag::Neg_T > >( &i.params.at( j ) );
                if ( exp )
                {
                    BinaryFunctionCall< ast::tag::Mul_T > tmp;
                    if ( j == 0 )
                        tmp = {"MUL", {exp->params.at( 0 ), i.params.at( 1 )}};

                    if ( j == 1 )
                        tmp = {"MUL", {i.params.at( 0 ), exp->params.at( 0 )}};
                    returnExpression = Expression( UnaryFunctionCall< ast::tag::Neg_T >( {"NEG", {tmp}} ) );
                    return true;
                }
            }
            // MUL(ID0_ObjR,DIV(ID1_ObjR,ID2_ObjR)) -->  DIV(MUL(ID0_ObjR,ID1_ObjR),ID2_ObjR)
            //                                           DIV(MUL(ID0_ObjR,ID1_ObjR),ID1_ObjR)
            // MUL(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR) -->  DIV(MUL(ID2_ObjR,ID0_ObjR),ID1_ObjR)
            {
                BinaryFunctionCall< ast::tag::Div_T > *exp =
                 boost::get< BinaryFunctionCall< ast::tag::Div_T > >( &i.params.at( !j ) );
                if ( exp )
                {
                    BinaryFunctionCall< ast::tag::Mul_T > mul{"MUL", {i.params.at( j ), exp->params.at( 0 )}};
                    returnExpression = Expression( BinaryFunctionCall< ast::tag::Div_T >{"DIV", {mul, exp->params.at( 1 )}} );

                    return true;
                }
            }
        }
        std::array< Constant *, 2 > tmpArray = {boost::get< Constant >( &i.params.at( 0 ) ),
                                                boost::get< Constant >( &i.params.at( 1 ) )};
        for ( size_t j = 0; j < 2; ++j )
        {
            if ( !tmpArray[j] )
                continue;
            if ( tmpArray[j]->name == "1" )
            {
                returnExpression = i.params.at( !j );
                return true;
            }
            if ( tmpArray[j]->name == "0" )
            {
                returnExpression = Constant{"0"};
                return true;
            }
        }
        if ( ReorderMulAdd( i, returnExpression ) )
        {
            return true;
        }
        if ( ReduceMulAddConstant( i, returnExpression ) )
        {
            return true;
        }
        if ( ExpandMul( i, returnExpression ) )
        {
            return true;
        }
        if ( ExpandMulConstTree< BinaryFunctionCall< ast::tag::Mul_T >, BinaryFunctionCall< ast::tag::Add_T > >( i, returnExpression ) )
        {
            return true;
        }
        if ( ExpandMulConstTree< BinaryFunctionCall< ast::tag::Mul_T >, BinaryFunctionCall< ast::tag::Sub_T > >( i, returnExpression ) )
        {
            return true;
        }
        if ( ExpandMulTree< BinaryFunctionCall< ast::tag::Mul_T >, BinaryFunctionCall< ast::tag::Add_T > >( i, returnExpression ) )
        {
            return true;
        }
        if ( ExpandMulTree< BinaryFunctionCall< ast::tag::Mul_T >, BinaryFunctionCall< ast::tag::Sub_T > >( i, returnExpression ) )
        {
            return true;
        }


        return false;
    }

    template < class T >
    bool SubAddDevision( T &i, Expression &returnExpression )
    {
        std::array< BinaryFunctionCall< ast::tag::Div_T > *, 2 > tmpArray = {
         boost::get< BinaryFunctionCall< ast::tag::Div_T > >( &i.params.at( 0 ) ),
         boost::get< BinaryFunctionCall< ast::tag::Div_T > >( &i.params.at( 1 ) )};
        if ( tmpArray[0] && tmpArray[1] )
        {
            std::vector< std::stringstream > vecStr( 2 );
            for ( size_t j = 0; j < 2; ++j )
            {
                SymbolicVisitor x( &vecStr[j] );
                boost::apply_visitor( x, tmpArray[j]->params.at( 1 ) );
            }
            if ( vecStr[0].str() == vecStr[1].str() )
            {
                /// VIELLEICHT RICHTIG?
                T subAdd( {i.funct.name, {tmpArray[0]->params.at( 0 ), tmpArray[1]->params.at( 0 )}} );
                returnExpression = BinaryFunctionCall< ast::tag::Div_T >{"DIV", {subAdd, tmpArray[0]->params.at( 1 )}};
                return true;
            }
            else
            {
                BinaryFunctionCall< ast::tag::Mul_T > mulA{"MUL", {tmpArray[0]->params.at( 0 ), tmpArray[1]->params.at( 1 )}};
                BinaryFunctionCall< ast::tag::Mul_T > mulB{"MUL", {tmpArray[0]->params.at( 1 ), tmpArray[1]->params.at( 0 )}};
                BinaryFunctionCall< ast::tag::Mul_T > mulC{"MUL", {tmpArray[0]->params.at( 1 ), tmpArray[1]->params.at( 1 )}};
                T subAdd{i.funct.name, {mulA, mulB}};
                returnExpression = BinaryFunctionCall< ast::tag::Div_T >{"DIV", {subAdd, mulC}};
                return true;
            }
        }
        // ADD(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR) --> DIV(ADD(ID0_ObjR,MUL(ID1_ObjR,ID2_ObjR)),ID1_ObjR)
        // ADD(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR)
        // ADD(ID0_ObjR,DIV(ID1_ObjR,ID2_ObjR))
        // SUB(DIV(ID0_ObjR,ID1_ObjR),ID2_ObjR) --> DIV(SUB(ID0_ObjR,MUL(ID2_ObjR,ID1_ObjR)),ID1_ObjR)
        for ( size_t j = 0; j < 2; ++j )
        {
            if ( tmpArray[j] )
            {
                if ( j == 0 )
                {
                    BinaryFunctionCall< ast::tag::Mul_T > mulA{"MUL", {i.params.at( 1 ), tmpArray[j]->params.at( 1 )}};
                    T subAdd{i.funct.name, {tmpArray[j]->params.at( 0 ), mulA}};
                    returnExpression = BinaryFunctionCall< ast::tag::Div_T >{"DIV", {subAdd, tmpArray[j]->params.at( 1 )}};    //
                }
                else
                {
                    // ADD(ID0_ObjR,DIV(ID1_ObjR,ID2_ObjR))

                    BinaryFunctionCall< ast::tag::Mul_T > mulA{"MUL", {i.params.at( 0 ), tmpArray[j]->params.at( 1 )}};
                    T subAdd{i.funct.name, {mulA, tmpArray[j]->params.at( 0 )}};
                    returnExpression = BinaryFunctionCall< ast::tag::Div_T >{"DIV", {subAdd, tmpArray[j]->params.at( 1 )}};    //
                }
                return true;
            }
        }
        return false;
    }


    bool SubPrefix( BinaryFunctionCall< ast::tag::Sub_T > &i, Expression &returnExpression )
    {

        Constant *constant = boost::get< Constant >( &i.params.at( 1 ) );
        if ( constant )
        {
            //            assert( false );    // TODO: Here kaputtgehen muss richtig geschrieben werden
            ast::tag::Neg_T n;
            Constant neg{n.Converter( *constant )};

            returnExpression = Expression( BinaryFunctionCall< ast::tag::Add_T >( {"ADD", {neg, i.params.at( 0 )}} ) );
            return true;
        }
        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            // SUB(ID0_ObjR,NEG(ID1_ObjR))
            UnaryFunctionCall< ast::tag::Neg_T > *exp = boost::get< UnaryFunctionCall< ast::tag::Neg_T > >( &i.params.at( j ) );
            if ( exp )
            {
                if ( j == 1 )
                {
                    returnExpression =
                     Expression( BinaryFunctionCall< ast::tag::Add_T >( {"ADD", {i.params.at( 0 ), exp->params.at( 0 )}} ) );
                }
                else
                {
                    // SUB(NEG(ID1_ObjR),ID0_ObjR) // NEG(ADD(ID1_ObjR, ID0_ObjR))

                    Expression a = ( BinaryFunctionCall< ast::tag::Add_T >( {"ADD", {exp->params.at( 0 ), i.params.at( 1 )}} ) );
                    returnExpression = UnaryFunctionCall< ast::tag::Neg_T >( {"NEG", {a}} );
                }
                return true;
            }
        }
        // -->DIV(SUB(MUL(ID0_ObjR,ID3_ObjR),MUL(ID1_ObjR,ID2_ObjR)),MUL(ID1_ObjR,ID3_ObjR))
        if ( SubAddDevision( i, returnExpression ) )
        {
            return true;
        }
        // Eliminate same terms in Subtraktion
        {
            std::vector< Expression > expre{i.params.at( 0 ), i.params.at( 1 )};
            std::vector< std::stringstream > vecStr( 2 );
            for ( size_t k = 0; k < 2; ++k )
            {
                SymbolicVisitor x( &vecStr[k] );
                boost::apply_visitor( x, expre[k] );
            }
            if ( vecStr.at( 0 ).str() == vecStr.at( 1 ).str() )
            {
                returnExpression = Constant{"0"};
                return true;
            }
        }
        // SUB(ADD(ID1_ObjR,ID0_ObjR),ID1_ObjR) --> ID0_ObjR
        BinaryFunctionCall< ast::tag::Add_T > *add = boost::get< BinaryFunctionCall< ast::tag::Add_T > >( &i.params.at( 0 ) );
        if ( add )
        {
            for ( size_t k = 0; k < 2; ++k )
            {
                std::vector< Expression > expre{add->params.at( k ), i.params.at( 1 )};
                std::vector< std::stringstream > vecStr( 2 );
                for ( size_t k = 0; k < 2; ++k )
                {
                    SymbolicVisitor x( &vecStr[k] );
                    boost::apply_visitor( x, expre[k] );
                }
                if ( vecStr.at( 0 ).str() == vecStr.at( 1 ).str() )
                {
                    returnExpression = add->params.at( !k );
                    return true;
                }
            }
        }
        /*
        if ( ExpandDivTree( i, returnExpression ) )
        {
            return true;
        }
        */

        // SUB(ADD(ADD(ID1_ObjR,ID0_ObjR),ID2_ObjR),ID1_ObjR)

        /*
        BinaryFunctionCall< ast::tag::Sub_T > *sub = boost::get< BinaryFunctionCall< ast::tag::Sub_T > >(
        &i.params.at(
        0 ) );
        if (sub )
        {
        }
        */


        {
            /*
        std::array< Constant *, 2 > tmpArray = {boost::get< Constant >( &i.params.at( 0 ) ),
                                                boost::get< Constant >( &i.params.at( 1 ) )};
            if ( !tmpArray[0] ^ !tmpArray[0])
            {
                if (tmpArray[0] )
                {
                }
                for
                if
            }
            */
        }

        return false;
    }


    bool AddPrefix( BinaryFunctionCall< ast::tag::Add_T > &i, Expression &returnExpression )
    {
        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            //"ADD(NEG(ID0_ObjR),ID1_ObjR)
            {
                UnaryFunctionCall< ast::tag::Neg_T > *exp =
                 boost::get< UnaryFunctionCall< ast::tag::Neg_T > >( &i.params.at( j ) );
                if ( exp )
                {
                    if ( j == 0 )
                    {
                        returnExpression =
                         BinaryFunctionCall< ast::tag::Sub_T >( {"SUB", {i.params.at( 1 ), exp->params.at( 0 )}} );
                    }
                    else
                    {
                        returnExpression =
                         BinaryFunctionCall< ast::tag::Sub_T >( {"SUB", {i.params.at( 0 ), exp->params.at( 0 )}} );
                    }
                    return true;
                }
            }
        }
        // ADD(DIV(ID0_ObjR,ID1_ObjR),DIV(ID2_ObjR,ID1_ObjR)) --> DIV(ADD(ID0_ObjR,ID2_ObjR),ID1_ObjR)

        if ( SubAddDevision( i, returnExpression ) )
        {
            return true;
        }

        // ADD(SUB(1,ID1_ObjR),ID1_ObjR)
        for ( size_t j = 0; j < 2; ++j )
        {
            BinaryFunctionCall< ast::tag::Sub_T > *exp =
             boost::get< BinaryFunctionCall< ast::tag::Sub_T > >( &i.params.at( j ) );    // = j
            if ( exp )
            {
                std::vector< Expression > expre{exp->params.at( 1 ), i.params.at( !j )};    // !j
                std::vector< std::stringstream > vecStr( 2 );
                for ( size_t k = 0; k < 2; ++k )
                {
                    SymbolicVisitor x( &vecStr[k] );
                    boost::apply_visitor( x, expre[k] );
                }
                if ( vecStr.at( 0 ).str() == vecStr.at( 1 ).str() )
                {
                    returnExpression = exp->params.at( 0 );
                    return true;
                }
            }
        }
        if ( ReorderMulAdd( i, returnExpression ) )
        {
            return true;
        }

        if ( ReduceMulAddConstant( i, returnExpression ) )
        {
            return true;
        }
        /*
        if ( ExpandDivTree( i, returnExpression ) )
        {
            return true;
        }
        */
        return false;
    }

    template < class X >
    bool ExpandDivTree( X &i, Expression &returnExpression )
    {

        for ( size_t j = 0; j < 2; ++j )
        {
            BinaryFunctionCall< ast::tag::Div_T > *exp =
             boost::get< BinaryFunctionCall< ast::tag::Div_T > >( &i.params.at( j ) );    // = j
            if ( exp )
            {
                if ( j == 0 )
                {
                    BinaryFunctionCall< ast::tag::Mul_T > expansion{"MUL", {i.params.at( !j ), exp->params.at( 1 )}};
                    returnExpression = X{i.funct.name, {i.params.at( 0 ), expansion}};
                }
                if ( j == 1 )
                {
                    BinaryFunctionCall< ast::tag::Mul_T > expansion{"MUL", {exp->params.at( 1 ), i.params.at( !j )}};
                    returnExpression = X{i.funct.name, {expansion, i.params.at( 1 )}};
                }
                returnExpression = BinaryFunctionCall< ast::tag::Div_T >{"DIV", {returnExpression, i.params.at( 1 )}};
                return true;
            }
        }
        return false;
    }

    template < class X, class T >
    bool ExpandMulTree( X &i, Expression &returnExpression )
    {
        for ( size_t j = 0; j < 2; ++j )
        {
            T *muAd = boost::get< T >( &i.params.at( j ) );
            if ( !muAd )
                continue;

            X x_left = X{"MUL", {i.params.at( !j ), muAd->params.at( 0 )}};
            X x_right = X{"MUL", {i.params.at( !j ), muAd->params.at( 1 )}};

            returnExpression = T{muAd->funct.name, {x_left, x_right}};
            return true;
        }
        return false;
    }


    template < class X, class T >
    bool ExpandMulConstTree( X &i, Expression &returnExpression )
    {
        for ( size_t j = 0; j < 2; ++j )
        {
            T *muAd = boost::get< T >( &i.params.at( j ) );
            if ( !muAd || !boost::get< Constant >( &i.params.at( !j ) ) )
                continue;

            X x_left{"MUL", {muAd->params.at( 0 ), i.params.at( !j )}};

            X x_right{"MUL", {muAd->params.at( 1 ), i.params.at( !j )}};

            returnExpression = T{muAd->funct.name, {x_left, x_right}};
            // MUL(ADD(3,ID0_ObjR),2) --> ADD(6,MUL(2,ID0_ObjR))
            return true;
        }
        return false;
    }


    template < class X >
    bool ExpandMul( X &i, Expression &returnExpression )
    {
        typedef BinaryFunctionCall< ast::tag::Add_T > ADD;

        for ( size_t j = 0; j < 2; ++j )
        {
            ADD *muAd = boost::get< ADD >( &i.params.at( j ) );
            if ( !muAd || !boost::get< Constant >( &muAd->params.at( 0 ) ) || !boost::get< Constant >( &i.params.at( !j ) ) )
                continue;

            X x{"MUL", {muAd->params.at( 0 ), i.params.at( !j )}};    // By rotating constant values to the first
                                                                      // position we only have to check the 0th
                                                                      // position

            X x_right{"MUL", {muAd->params.at( 1 ), i.params.at( !j )}};    // By rotating constant values to the first
                                                                            // position we only have to check the 0th
                                                                            // position


            ConstantFolding( x, returnExpression );    // must return true
            returnExpression = ADD{"ADD", {returnExpression, x_right}};
            // MUL(ADD(3,ID0_ObjR),2) --> ADD(6,MUL(2,ID0_ObjR))
            return true;
        }
        return false;
    }


    template < class X >
    bool ReduceMulAddConstant( X &i, Expression &returnExpression )
    {
        for ( size_t j = 0; j < 2; ++j )
        {
            X *muAd = boost::get< X >( &i.params.at( j ) );
            if ( !muAd || !boost::get< Constant >( &muAd->params.at( 0 ) ) || !boost::get< Constant >( &i.params.at( !j ) ) )
                continue;

            X x{i.funct.name, {muAd->params.at( 0 ), i.params.at( !j )}};    // By rotating constant values to the first
                                                                             // position we only have to check the 0th
                                                                             // position

            ConstantFolding( x, returnExpression );    // must return true
            returnExpression = X{i.funct.name, {returnExpression, muAd->params.at( 1 )}};
            return true;
        }
        return false;
    }

    template < class X >
    bool ReorderMulAdd( X &i, Expression &returnExpression )
    {
        X *muAd = boost::get< X >( &i.params.at( 1 ) );
        if ( !muAd || boost::get< X >( &i.params.at( 0 ) ) )
            return false;

        if ( boost::get< Constant >( &i.params.at( 0 ) ) || boost::get< Variable >( &i.params.at( 0 ) ) )
            return false;

        X x{i.funct.name, {i.params.at( 0 ), muAd->params.at( 0 )}};
        returnExpression = X{i.funct.name, {x, muAd->params.at( 1 )}};
        return true;
    }

    template < class X, class T >
    bool Reordered( X &i )
    {
        Constant *tmpYArray = boost::get< Constant >( &i.params.at( 0 ) );
        if ( tmpYArray )
            return false;

        std::array< T *, 2 > tmpArray = {boost::get< T >( &i.params.at( 0 ) ), boost::get< T >( &i.params.at( 1 ) )};
        if ( !tmpArray[0] && tmpArray[1] )
        {
            std::swap( i.params.at( 0 ), i.params.at( 1 ) );
            return true;
        }
        return false;
    }


    template < template < class > class T, class R >
    void ApplyVisitor( T< R > &i )
    {
        for ( size_t j = 0; j < i.params.size(); ++j )
        {
            i.params.at( j ) = boost::apply_visitor( *this, i.params.at( j ) );
        }
    }

    void ReorderBinaryOperands( BinaryFunctionCall< ast::tag::Div_T > & ) { return; }

    void ReorderBinaryOperands( BinaryFunctionCall< ast::tag::Sub_T > & ) { return; }


    template < template < class > class T, class R >
    void ReorderBinaryOperands( T< R > &i )
    {
        if ( Reordered< T< R >, Constant >( i ) )
            return;

        if ( Reordered< T< R >, Variable >( i ) )
            return;
    }

    Expression operator()( BinaryFunctionCall< ast::tag::Add_T > &i )
    {
        ApplyVisitor( i );
        ReorderBinaryOperands( i );

        Expression returnValue;
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        }
        if ( AddPrefix( i, returnValue ) )
        {
            return returnValue;
        }

        return i;
    }


    Expression operator()( BinaryFunctionCall< ast::tag::Sub_T > &i )
    {
        ApplyVisitor( i );
        Expression returnValue;
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        };
        if ( SubPrefix( i, returnValue ) )
        {
            return returnValue;
        }
        return i;
    }

    Expression operator()( BinaryFunctionCall< ast::tag::Mul_T > &i )
    {
        ApplyVisitor( i );
        ReorderBinaryOperands( i );

        Expression returnValue;
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        };
        if ( MulPrefix( i, returnValue ) )
        {
            return returnValue;
        }
        return i;
    }


    Expression operator()( UnaryFunctionCall< ast::tag::Neg_T > &i )
    {
        ApplyVisitor( i );
        Expression returnValue;
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        };
        if ( NegPrefix( i, returnValue ) )
        {
            return returnValue;
        }

        return i;
    }

    Expression operator()( UnaryFunctionCall< ast::tag::Sqrt_T > &i )
    {
        ApplyVisitor( i );
        Expression returnValue;
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        };

        return i;
    }
    Expression operator()( UnaryFunctionCall< ast::tag::Tanh_T > &i )
    {
        ApplyVisitor( i );
        Expression returnValue;
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        };

        return i;
    }


    Expression operator()( BinaryFunctionCall< ast::tag::Div_T > &i )
    {
        ApplyVisitor( i );
        ReorderBinaryOperands( i );
        Expression returnValue;
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        };
        if ( DivPrefix( i, returnValue ) )
        {
            return returnValue;
        }


        return i;
    }

    template < template < class > class T, class R >
    Expression operator()( T< R > &i )
    {
        ApplyVisitor( i );
        Expression returnValue;
        ReorderBinaryOperands( i );
        if ( ConstantFolding( i, returnValue ) )
        {
            return returnValue;
        };


        return i;
    }
};
/*
template <>
void OptimizationVisitor::operator()< Constant >( Constant &, Constant & );
*/
}
}

#endif /* _OPTIMIZATION_VISITOR_ */
