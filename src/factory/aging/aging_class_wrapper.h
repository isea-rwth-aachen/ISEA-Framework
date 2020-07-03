// agingclasswrapper.h
// Autor: Christopher Abele, Andreas Hase

#ifndef _AGINGCLASSWRAPPER_
#define _AGINGCLASSWRAPPER_

#include "../../misc/macros.h"

#include "../../aging/anode_overhang.h"
#include "../../aging/calendarian_aging.h"
#include "../../aging/cyclical_aging.h"
#include "../../aging/generic_aging.h"
#include "../../electrical/cellelement.h"
#include "../../object/const_obj.h"
#include "../../state/aging_state.h"

#include "../classwrapper.h"
#include "../factory.h"
#include "../object/objectclasswrapper.h"

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

namespace factory
{

struct ArgumentTypeAging
{
    ArgumentTypeAging()
        : mCellCounter( 0 )
        , mAgingState( nullptr ){};

    size_t mCellCounter;
    boost::shared_ptr< state::AgingState > mAgingState;
};

/// Base class for the the different aging class wrappers.
class AgingClassWrapperBase : public ClassWrapperBase< aging::AgingBase, ArgumentTypeAging >
{
    public:
    AgingClassWrapperBase( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                           Factory< state::State, ArgumentTypeState >* stateFactory )
        : mObjectFactory( objectFactory )
        , mStateFactory( stateFactory )
    {
    }

    protected:
    Factory< object::Object< double >, ArgumentTypeObject< double > >* mObjectFactory;
    Factory< state::State, ArgumentTypeState >* mStateFactory;

    Factory< object::Object< double >, ArgumentTypeObject< double > >* GetObjectFactory() { return mObjectFactory; }
    Factory< state::State, ArgumentTypeState >* GetStateFactory() { return mStateFactory; }
};

/// General Template AgingClassWrapper
template < class TConcrete >
class AgingClassWrapper : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};
};

/// Specialized AgingClassWrapper for CalendarianAging
template <>
class AgingClassWrapper< aging::CalendarianAging > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        // Collects the calendarian data from the cell-specific aging node
        const std::string formulaAlphaCap = param->GetElementStringValue( "FormulaCapacity" );
        const std::string formulaAlphaRes = param->GetElementStringValue( "FormulaResistance" );
        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );
        const double timeExponent = param->GetElementDoubleValue( "TimeExponent" );
        const double minAlphaCap = param->GetElementDoubleValue( "MinAlphaCapacity" );
        const double minAlphaRes = param->GetElementDoubleValue( "MinAlphaResistance" );

        double initialCapacityFactor = 1;
        double initialResistanceFactor = 1;
        if ( param->HasElementDirectChild( "InitialCapacityFactor" ) )
            initialCapacityFactor = param->GetElementDoubleValue( "InitialCapacityFactor" );
        if ( param->HasElementDirectChild( "InitialResistanceFactor" ) )
            initialResistanceFactor = param->GetElementDoubleValue( "InitialResistanceFactor" );

        // Collects data from the again options node
        const auto agingNodeOptions =
         param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "Aging" );
        const double agingStepTime = agingNodeOptions->GetElementDoubleValue( "AgingStepTime" );

        // Creates an object of the class "aging::CalendarianAging"
        boost::shared_ptr< aging::CalendarianAging > calendarianAging =
         boost::make_shared< aging::CalendarianAging >( agingStepTime, minAlphaCap, minAlphaRes, formulaAlphaCap,
                                                        formulaAlphaRes, initialCapacityFactor, initialResistanceFactor,
                                                        isEnabled, timeExponent );

        return calendarianAging;
    }
};

/// Specialized AgingClassWrapper for CyclicalAging
template <>
class AgingClassWrapper< aging::CyclicalAging > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        // Collects the cyclical data from the cell-specific aging node
        const std::string formulaBetaCap = param->GetElementStringValue( "FormulaCapacity" );
        const std::string formulaBetaRes = param->GetElementStringValue( "FormulaResistance" );
        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );
        const double chargeExponentCapacity = param->GetElementDoubleValue( "ChargeExponentCapacity" );
        const double chargeExponentResistance = param->GetElementDoubleValue( "ChargeExponentResistance" );
        const double minBetaCap = param->GetElementDoubleValue( "MinBetaCapacity" );
        const double minBetaRes = param->GetElementDoubleValue( "MinBetaResistance" );

        double initialCapacityFactor = 1;
        double initialResistanceFactor = 1;
        if ( param->HasElementDirectChild( "InitialCapacityFactor" ) )
            initialCapacityFactor = param->GetElementDoubleValue( "InitialCapacityFactor" );
        if ( param->HasElementDirectChild( "InitialResistanceFactor" ) )
            initialResistanceFactor = param->GetElementDoubleValue( "InitialResistanceFactor" );

        // Collects data from the again options node
        const auto agingNodeOptions =
         param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "Aging" );
        const double agingStepTime = agingNodeOptions->GetElementDoubleValue( "AgingStepTime" );

        // Creates an object of the class "aging::CyclicalAging"
        boost::shared_ptr< aging::CyclicalAging > cyclicalAging =
         boost::make_shared< aging::CyclicalAging >( agingStepTime, minBetaCap, minBetaRes, formulaBetaCap,
                                                     formulaBetaRes, initialCapacityFactor, initialResistanceFactor,
                                                     isEnabled, chargeExponentCapacity, chargeExponentResistance );

        return cyclicalAging;
    }
};

/// Specialized AgingClassWrapper for AnodeOverhang
template <>
class AgingClassWrapper< aging::AnodeOverhang > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        // Collects data from the aging options node
        const auto agingNodeOptions =
         param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "Aging" );
        const double agingStepTime = agingNodeOptions->GetElementDoubleValue( "AgingStepTime" );

        boost::shared_ptr< state::Soc > soc = boost::static_pointer_cast< state::Soc >(
         this->GetStateFactory()->CreateInstance( param->GetElementChild( "Soc" ) ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double >() );
        argObject->mSoc = soc;
        boost::shared_ptr< object::Object< double > > voltage =
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "Voltage" ), argObject.get() );
        double activCoef = param->GetElementDoubleValue( "ActivationCoefficient" );
        double tauCoef = param->GetElementDoubleValue( "TauCoefficient" );
        double zeroOffsetSoc = param->GetElementDoubleValue( "SocWhereOffsetIsZero" );
        bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );

        return boost::make_shared< aging::AnodeOverhang >( agingStepTime, voltage, activCoef, tauCoef, soc, zeroOffsetSoc, isEnabled );
    }
};

/// Specialized AgingClassWrapper for GenericAging
template <>
class AgingClassWrapper< aging::GenericAging > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        // Collects data from the aging options node
        const auto agingNodeOptions =
         param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "Aging" );
        const double agingStepTime = agingNodeOptions->GetElementDoubleValue( "AgingStepTime" );
        const double capFactor = param->GetElementDoubleValue( "CapacityFactor" );
        const double resFactor = param->GetElementDoubleValue( "ResistanceFactor" );
        const double socOffset = param->GetElementDoubleValue( "SocOffset" );

        return boost::make_shared< aging::GenericAging >( agingStepTime, capFactor, socOffset, resFactor );
    }
};

}    // Namespace factory

#endif    // _AGINGCLASSWRAPPER
