// agingclasswrapper.h
// Autor: Christopher Abele, Andreas Hase

#ifndef _AGINGCLASSWRAPPER_
#define _AGINGCLASSWRAPPER_

#include "../../misc/macros.h"

#include "../../aging/aging_lam.h"
#include "../../aging/aging_lli.h"
#include "../../aging/anode_overhang.h"
#include "../../aging/calendarian_aging.h"
#include "../../aging/cyclical_aging.h"
#include "../../aging/calendarian_aging2.h"
#include "../../aging/cyclical_aging2.h"
#include "../../aging/generic_aging.h"
#include "../../electrical/cellelement.h"
#include "../../object/const_obj.h"
#include "../../state/aging_state.h"
#include "../../state/valueStateWrapper.h"

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
        auto voltageState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto temperatureState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto socState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        this->GetStateFactory()->CacheInstance( "V", voltageState );
        this->GetStateFactory()->CacheInstance( "T", temperatureState );
        this->GetStateFactory()->CacheInstance( "SOC", socState );

        boost::shared_ptr< object::Object< double > > alphaCapacity;
        boost::shared_ptr< object::Object< double > > alphaResistance;

        std::vector< typename object::ExpressionObject< double >::Parameter > objectParameters{ { "V", voltageState },
                                                                                                { "T", temperatureState },
                                                                                                { "SOC", socState } };
        // Collects the calendarian data from the cell-specific aging node
        if ( param->HasElementDirectChild( "AlphaCapacity" ) )
        {
            alphaCapacity = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "AlphaCapacity" ) );
        }
        else
        {
            const std::string formulaAlphaCap = param->GetElementStringValue( "FormulaCapacity" );
            alphaCapacity = boost::make_shared< object::ExpressionObject< double > >( formulaAlphaCap, objectParameters );
        }
        if ( param->HasElementDirectChild( "AlphaResistance" ) )
        {
            alphaResistance = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "AlphaResistance" ) );
        }
        else
        {
            const std::string formulaAlphaRes = param->GetElementStringValue( "FormulaResistance" );
            alphaResistance = boost::make_shared< object::ExpressionObject< double > >( formulaAlphaRes, objectParameters );
        }
        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );
        const double timeExponent = param->GetElementDoubleValue( "TimeExponent" );
        const double minAlphaCap = param->GetElementDoubleValue( "MinAlphaCapacity", 0 );
        const double minAlphaRes = param->GetElementDoubleValue( "MinAlphaResistance", 0 );
        const double maxAlphaCap = param->GetElementDoubleValue( "MaxAlphaCapacity", std::numeric_limits< double >::max() );
        const double maxAlphaRes = param->GetElementDoubleValue( "MaxAlphaResistance", std::numeric_limits< double >::max() );

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
         boost::make_shared< aging::CalendarianAging >( agingStepTime, minAlphaCap, minAlphaRes, maxAlphaCap, maxAlphaRes,
                                                        alphaCapacity, alphaResistance, initialCapacityFactor,
                                                        initialResistanceFactor, isEnabled, timeExponent );

        *voltageState = state::ValueStateWrapper< double >( &calendarianAging->mActualVoltage );
        *temperatureState = state::ValueStateWrapper< double >( &calendarianAging->mActualTemperature );
        *socState = state::ValueStateWrapper< double >( &calendarianAging->mActualSoc );

        return calendarianAging;
    }
};

/// Specialized AgingClassWrapper for CalendarianAging2
template <>
class AgingClassWrapper< aging::CalendarianAging2 > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        auto voltageState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto temperatureState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto socState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        this->GetStateFactory()->CacheInstance( "V", voltageState );
        this->GetStateFactory()->CacheInstance( "T", temperatureState );
        this->GetStateFactory()->CacheInstance( "SOC", socState );

        boost::shared_ptr< object::Object< double > > alphaCapacity;
        boost::shared_ptr< object::Object< double > > alphaResistance;

        std::vector< typename object::ExpressionObject< double >::Parameter > objectParameters{ { "V", voltageState },
                                                                                                { "T", temperatureState },
                                                                                                { "SOC", socState } };
        // Collects the calendarian data from the cell-specific aging node
        if ( param->HasElementDirectChild( "AlphaCapacity" ) )
        {
            alphaCapacity = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "AlphaCapacity" ) );
        }
        else
        {
            const std::string formulaAlphaCap = param->GetElementStringValue( "FormulaCapacity" );
            alphaCapacity = boost::make_shared< object::ExpressionObject< double > >( formulaAlphaCap, objectParameters );
        }
        if ( param->HasElementDirectChild( "AlphaResistance" ) )
        {
            alphaResistance = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "AlphaResistance" ) );
        }
        else
        {
            const std::string formulaAlphaRes = param->GetElementStringValue( "FormulaResistance" );
            alphaResistance = boost::make_shared< object::ExpressionObject< double > >( formulaAlphaRes, objectParameters );
        }
        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );

        double timeExponentResistance;
        double timeExponentCapacity;
        if ( param->HasElementDirectChild( "TimeExponent" ) )
        {
            timeExponentResistance = param->GetElementDoubleValue( "TimeExponent" );
            timeExponentCapacity = param->GetElementDoubleValue( "TimeExponent" );
        }
        else
        {
            timeExponentResistance = param->GetElementDoubleValue( "TimeExponentResistance" );
            timeExponentCapacity = param->GetElementDoubleValue( "TimeExponentCapacity" );
        }
        const double minAlphaCap = param->GetElementDoubleValue( "MinAlphaCapacity", 0 );
        const double minAlphaRes = param->GetElementDoubleValue( "MinAlphaResistance", 0 );
        const double maxAlphaCap = param->GetElementDoubleValue( "MaxAlphaCapacity", std::numeric_limits< double >::max() );
        const double maxAlphaRes = param->GetElementDoubleValue( "MaxAlphaResistance", std::numeric_limits< double >::max() );

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
        boost::shared_ptr< aging::CalendarianAging2 > calendarianAging2 =
         boost::make_shared< aging::CalendarianAging2 >( agingStepTime, minAlphaCap, minAlphaRes, maxAlphaCap, maxAlphaRes,
                                                         alphaCapacity, alphaResistance, initialCapacityFactor, initialResistanceFactor,
                                                         isEnabled, timeExponentResistance, timeExponentCapacity );

        *voltageState = state::ValueStateWrapper< double >( &calendarianAging2->mActualVoltage );
        *temperatureState = state::ValueStateWrapper< double >( &calendarianAging2->mActualTemperature );
        *socState = state::ValueStateWrapper< double >( &calendarianAging2->mActualSoc );

        return calendarianAging2;
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
        auto dodState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto voltageState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto socState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto currentState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto temperatureState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        this->GetStateFactory()->CacheInstance( "deltaDOD", dodState );
        this->GetStateFactory()->CacheInstance( "meanV", voltageState );
        this->GetStateFactory()->CacheInstance( "meanSOC", socState );
        this->GetStateFactory()->CacheInstance( "meanI", currentState );
        this->GetStateFactory()->CacheInstance( "meanT", temperatureState );

        boost::shared_ptr< object::Object< double > > betaCapacity;
        boost::shared_ptr< object::Object< double > > betaResistance;

        std::vector< typename object::ExpressionObject< double >::Parameter > objectParameters{ { "deltaDOD", dodState },
                                                                                                { "meanV", voltageState },
                                                                                                { "meanSOC", socState },
                                                                                                { "meanI", currentState },
                                                                                                { "meanT", temperatureState} };
        // Collects the cyclical data from the cell-specific aging node
        if ( param->HasElementDirectChild( "BetaCapacity" ) )
        {
            betaCapacity = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "BetaCapacity" ) );
        }
        else
        {
            const std::string formulaBetaCap = param->GetElementStringValue( "FormulaCapacity" );
            betaCapacity = boost::make_shared< object::ExpressionObject< double > >( formulaBetaCap, objectParameters );
        }
        if ( param->HasElementDirectChild( "BetaResistance" ) )
        {
            betaResistance = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "BetaResistance" ) );
        }
        else
        {
            const std::string formulaBetaRes = param->GetElementStringValue( "FormulaResistance" );
            betaResistance = boost::make_shared< object::ExpressionObject< double > >( formulaBetaRes, objectParameters );
        }

        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );
        const double chargeExponentCapacity = param->GetElementDoubleValue( "ChargeExponentCapacity" );
        const double chargeExponentResistance = param->GetElementDoubleValue( "ChargeExponentResistance" );
        const double minBetaCap = param->GetElementDoubleValue( "MinBetaCapacity" );
        const double minBetaRes = param->GetElementDoubleValue( "MinBetaResistance" );
        const std::string chargeCountingType = param->GetElementStringValue( "ChargeCountingType", "Ah" );
        const double maxBetaCap = param->GetElementDoubleValue( "MaxBetaCapacity", std::numeric_limits< double >::max() );
        const double maxBetaRes = param->GetElementDoubleValue( "MaxBetaResistance", std::numeric_limits< double >::max() );

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
         boost::make_shared< aging::CyclicalAging >( agingStepTime, minBetaCap, minBetaRes, maxBetaCap, maxBetaRes, betaCapacity,
                                                     betaResistance, initialCapacityFactor, initialResistanceFactor,
                                                     isEnabled, chargeExponentCapacity, chargeExponentResistance, chargeCountingType );

        *dodState = state::ValueStateWrapper< double >( &cyclicalAging->mActualDod );
        *voltageState = state::ValueStateWrapper< double >( &cyclicalAging->mActualVoltage );
        *socState = state::ValueStateWrapper< double >( &cyclicalAging->mActualSoc );
        *currentState = state::ValueStateWrapper< double >( &cyclicalAging->mActualCurrent );
        *temperatureState = state::ValueStateWrapper< double >( &cyclicalAging->mActualTemperature );

        return cyclicalAging;
    }
};

/// Specialized AgingClassWrapper for CyclicalAging2
template <>
class AgingClassWrapper< aging::CyclicalAging2 > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        auto dodState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto voltageState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto socState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        auto currentState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        this->GetStateFactory()->CacheInstance( "deltaDOD", dodState );
        this->GetStateFactory()->CacheInstance( "meanV", voltageState );
        this->GetStateFactory()->CacheInstance( "meanSOC", socState );
        this->GetStateFactory()->CacheInstance( "meanI", currentState );

        boost::shared_ptr< object::Object< double > > betaCapacity;
        boost::shared_ptr< object::Object< double > > betaResistance;

        std::vector< typename object::ExpressionObject< double >::Parameter > objectParameters{ { "deltaDOD", dodState },
                                                                                                { "meanV", voltageState },
                                                                                                { "meanSOC", socState },
                                                                                                { "meanI", currentState } };
        // collects the cyclical data from the cell-specific aging node
        if ( param->HasElementDirectChild( "BetaCapacity" ) )
        {
            betaCapacity = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "BetaCapacity" ) );
        }
        else
        {
            const std::string formulaBetaCap = param->GetElementStringValue( "FormulaCapacity" );
            betaCapacity = boost::make_shared< object::ExpressionObject< double > >( formulaBetaCap, objectParameters );
        }
        if ( param->HasElementDirectChild( "BetaResistance" ) )
        {
            betaResistance = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "BetaResistance" ) );
        }
        else
        {
            const std::string formulaBetaRes = param->GetElementStringValue( "FormulaResistance" );
            betaResistance = boost::make_shared< object::ExpressionObject< double > >( formulaBetaRes, objectParameters );
        }

        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );
        const double EFCExponentCapacity = param->GetElementDoubleValue( "EFCExponentCapacity" );
        const double EFCExponentResistance = param->GetElementDoubleValue( "EFCExponentResistance" );
        const double minBetaCap = param->GetElementDoubleValue( "MinBetaCapacity" );
        const double minBetaRes = param->GetElementDoubleValue( "MinBetaResistance" );
        const double maxBetaCap = param->GetElementDoubleValue( "MaxBetaCapacity", std::numeric_limits< double >::max() );
        const double maxBetaRes = param->GetElementDoubleValue( "MaxBetaResistance", std::numeric_limits< double >::max() );

        double initialCapacityFactor = 1;
        double initialResistanceFactor = 1;
        if ( param->HasElementDirectChild( "InitialCapacityFactor" ) )
            initialCapacityFactor = param->GetElementDoubleValue( "InitialCapacityFactor" );
        if ( param->HasElementDirectChild( "InitialResistanceFactor" ) )
            initialResistanceFactor = param->GetElementDoubleValue( "InitialResistanceFactor" );

        // collects data from the again options node
        const auto agingNodeOptions =
         param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "Aging" );
        const double agingStepTime = agingNodeOptions->GetElementDoubleValue( "AgingStepTime" );

        // creates an object of the class "aging::cyclicalaging"
        boost::shared_ptr< aging::CyclicalAging2 > cyclicalAging2 =
         boost::make_shared< aging::CyclicalAging2 >( agingStepTime, minBetaCap, minBetaRes, maxBetaCap, maxBetaRes, betaCapacity,
                                                      betaResistance, initialCapacityFactor, initialResistanceFactor,
                                                      isEnabled, EFCExponentCapacity, EFCExponentResistance );

        *dodState = state::ValueStateWrapper< double >( &cyclicalAging2->mActualDod );
        *voltageState = state::ValueStateWrapper< double >( &cyclicalAging2->mActualVoltage );
        *socState = state::ValueStateWrapper< double >( &cyclicalAging2->mActualSoc );
        *currentState = state::ValueStateWrapper< double >( &cyclicalAging2->mActualCurrent );

        return cyclicalAging2;
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
        boost::shared_ptr< object::Object< double > > dva =
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "DVA" ), argObject.get() );
        double activCoef = param->GetElementDoubleValue( "ActivationCoefficient" );
        double tauCoef = param->GetElementDoubleValue( "TauCoefficient" );
        double zeroOffsetSoc = param->GetElementDoubleValue( "SocWhereOffsetIsZero" );
        bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );

        return boost::make_shared< aging::AnodeOverhang >( agingStepTime, voltage, dva, activCoef, tauCoef, soc, zeroOffsetSoc, isEnabled );
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

template <>
class AgingClassWrapper< aging::AgingLAM > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        const double agingStepTime = param->GetConfigurationRoot()
                                      ->GetElementChild( "Options" )
                                      ->GetElementChild( "Aging" )
                                      ->GetElementDoubleValue( "AgingStepTime" );
        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );
        auto cyclesState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        this->GetStateFactory()->CacheInstance( "cycles", cyclesState );
        auto valueObject = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "Value" ) );
        auto aging = boost::make_shared< aging::AgingLAM >( agingStepTime, isEnabled, valueObject );
        *cyclesState = state::ValueStateWrapper< double >( &aging->mCycles );

        return aging;
    }
};

template <>
class AgingClassWrapper< aging::AgingLLI > : public AgingClassWrapperBase
{
    public:
    AgingClassWrapper( Factory< object::Object< double >, ArgumentTypeObject< double > >* objectFactory,
                       Factory< state::State, ArgumentTypeState >* stateFactory )
        : AgingClassWrapperBase( objectFactory, stateFactory ){};

    boost::shared_ptr< aging::AgingBase > CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeAging* /* arg */ = 0 )
    {
        const double agingStepTime = param->GetConfigurationRoot()
                                      ->GetElementChild( "Options" )
                                      ->GetElementChild( "Aging" )
                                      ->GetElementDoubleValue( "AgingStepTime" );
        const bool isEnabled = param->GetElementAttributeBoolValue( "enabled", true );
        auto cyclesState = boost::make_shared< state::ValueStateWrapper< double > >( nullptr );
        this->GetStateFactory()->CacheInstance( "cycles", cyclesState );
        auto valueObject = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "Value" ) );
        auto aging = boost::make_shared< aging::AgingLLI >( agingStepTime, isEnabled, valueObject );
        *cyclesState = state::ValueStateWrapper< double >( &aging->mCycles );

        return aging;
    }
};

}    // Namespace factory

#endif    // _AGINGCLASSWRAPPER
