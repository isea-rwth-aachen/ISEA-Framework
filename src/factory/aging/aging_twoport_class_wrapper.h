#ifndef _AGING_TWOPORT_CLASS_WRAPPER_
#define _AGING_TWOPORT_CLASS_WRAPPER_

#ifdef BUILD_AGING_SIMULATION

#include "../../aging/aging_twoport.h"
#include "../../electrical/twoport_with_state.h"
#include "../../misc/matrixInclude.h"
#include "../factory.h"
#include "aging_class_wrapper.h"
#include <boost/shared_ptr.hpp>

extern template class electrical::TwoPortWithState< myMatrixType >;

namespace factory
{

struct ArgumentTypeAgingTwoPort
{
    boost::shared_ptr< electrical::TwoPortWithState< myMatrixType > > mElectricalTwoPort;
    boost::shared_ptr< electrical::Cellelement< myMatrixType > > mCellelement;
};

/// Base class for the the different aging class wrappers.
template < typename MatrixT >
class AgingTwoPortClassWrapperBase : public ClassWrapperBase< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >
{
    public:
    AgingTwoPortClassWrapperBase( Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* agingTwoPortFactory,
                                  Factory< aging::AgingBase, ArgumentTypeAging >* agingFactory,
                                  Factory< state::State, ArgumentTypeState >* stateFactory )
        : mAgingTwoPortFactory( agingTwoPortFactory )
        , mAgingFactory( agingFactory )
        , mStateFactory( stateFactory )
    {
    }

    protected:
    Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* const mAgingTwoPortFactory;
    Factory< aging::AgingBase, ArgumentTypeAging >* const mAgingFactory;
    Factory< state::State, ArgumentTypeState >* const mStateFactory;
};

/// General class wrapper for AgingTwoPort that needs to be specialized
template < typename MatrixT, template < typename > class TConcrete >
class AgingTwoPortClassWrapper : public AgingTwoPortClassWrapperBase< MatrixT >
{
    public:
    AgingTwoPortClassWrapper( Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* agingTwoPortFactory,
                              Factory< aging::AgingBase, ArgumentTypeAging >* agingFactory,
                              Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingTwoPortClassWrapperBase< MatrixT >( agingTwoPortFactory, agingFactory, stateFactory )
    {
    }
};

/// Class wrapper for AgingTwoPort
template < typename MatrixT >
class AgingTwoPortClassWrapper< MatrixT, aging::AgingTwoPort > : public AgingTwoPortClassWrapperBase< MatrixT >
{
    public:
    AgingTwoPortClassWrapper( Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* agingTwoPortFactory,
                              Factory< aging::AgingBase, ArgumentTypeAging >* agingFactory,
                              Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingTwoPortClassWrapperBase< MatrixT >( agingTwoPortFactory, agingFactory, stateFactory )
    {
    }

    boost::shared_ptr< aging::AgingTwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAgingTwoPort* arg = nullptr )
    {
        if ( !arg )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "arg",
                                                 "AgingTwoPortClassWrapper<AgingTwoPort>::CreateInstance" );
        if ( !arg->mElectricalTwoPort )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException",
                                                 "arg->mElectricalTwoPort",
                                                 "AgingTwoPortClassWrapper<AgingTwoPort>::CreateInstance" );

        boost::shared_ptr< electrical::TwoPortWithState< MatrixT > > electricalTP = arg->mElectricalTwoPort;
        boost::shared_ptr< state::AgingState > agingState = boost::make_shared< state::AgingState >();

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > agingNodes;
        std::vector< boost::shared_ptr< aging::AgingBase > > agingEffects;
        if ( param->HasElementDirectChild( "Aging" ) )
            agingNodes = param->GetElementChildren( "Aging" );

        if ( electricalTP->IsCellelement() )
        {
            auto cell = static_cast< electrical::Cellelement< MatrixT >* >( electricalTP.get() );
            if ( param->HasElementDirectChild( "AgingState" ) )
                agingState = boost::static_pointer_cast< state::AgingState >(
                 this->mStateFactory->CreateInstance( param->GetElementChild( "AgingState" ) ) );
            if ( cell->HasHalfcells() )
            {
                auto cellAgingTP = boost::make_shared< aging::AgingTwoPort< MatrixT > >( agingEffects, electricalTP,
                                                                                         arg->mCellelement, agingState );
                // aging effects for the cell will be added to all electrode elements, so all aging effects
                // have to be created each time
                std::vector< boost::shared_ptr< electrical::ElectrodeElement< MatrixT > > > elements;
                std::vector< boost::shared_ptr< aging::AgingTwoPort< MatrixT > > > createdInstances;
                this->mAgingTwoPortFactory->GetObjects( createdInstances );
                elements.reserve( cell->GetAnodeElements().size() + cell->GetCathodeElements().size() );
                for ( auto& element : cell->GetAnodeElements() )
                {
                    elements.push_back( boost::static_pointer_cast< electrical::ElectrodeElement< MatrixT > >( element ) );
                }
                for ( auto& element : cell->GetCathodeElements() )
                {
                    elements.push_back( boost::static_pointer_cast< electrical::ElectrodeElement< MatrixT > >( element ) );
                }
                std::vector< boost::shared_ptr< aging::AgingTwoPort< MatrixT > > > children;
                children.reserve( elements.size() );
                for ( auto& twoport : elements )
                {
                    auto& electrodeAgingTP =
                     *std::find_if( createdInstances.begin(), createdInstances.end(),
                                    [twoport]( const boost::shared_ptr< aging::AgingTwoPort< MatrixT > >& tp ) -> bool {
                                        return twoport == tp->GetTwoPort();
                                    } );
                    auto agingEffects = electrodeAgingTP->GetAgingEffects();
                    agingEffects.reserve( agingEffects.size() + agingNodes.size() );
                    for ( auto& agingNode : agingNodes )
                    {
                        agingEffects.push_back( this->mAgingFactory->CreateInstance( agingNode ) );
                    }
                    electrodeAgingTP->SetAgingEffects( agingEffects );
                    children.push_back( electrodeAgingTP );
                }
                cellAgingTP->SetChildren( children );
                return cellAgingTP;
            }
        }

        agingEffects.reserve( agingNodes.size() );
        for ( auto& agingNode : agingNodes )
            agingEffects.push_back( this->mAgingFactory->CreateInstance( agingNode ) );
        return boost::make_shared< aging::AgingTwoPort< MatrixT > >( agingEffects, electricalTP, arg->mCellelement, agingState );
    }
};


}    // namespace factory

#endif
#endif