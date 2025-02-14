// File: electricalclasswrapper.h
// Author: chris, Christopher Abele

#ifndef _ELECTRICALCLASSWRAPPER_
#define _ELECTRICALCLASSWRAPPER_

// STD
#include <cmath>
#include <type_traits>

// BOOST
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>

// ETC
#include "../check_positive_int.h"
#include "../classwrapper.h"
#include "../factory.h"
#include "../get_electrical_discretization.h"

#include "../../state/aging_state.h"
#include "../../state/soc.h"
#include "../../state/thermal_state.h"
#include "../../state/crate.h"
#include "../../state/cdirection.h"
#include "../../state/lhd.h"

#include "../../electrical/anode_element.h"
#include "../../electrical/capacity.h"
#include "../../electrical/cathode_element.h"
#include "../../electrical/cellelement.h"
#include "../../electrical/constantphaseelement.h"
#include "../../electrical/electrode.h"
#include "../../electrical/inductance.h"
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/parallelRCAlg.h"
#include "../../electrical/parallelrc.h"
#include "../../electrical/paralleltwoport.h"
#include "../../electrical/rmphn.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/sphericalDiffusion.h"
#include "../../electrical/voltagesource.h"
#include "../../electrical/voltagesource_onestate.h"
#include "../../electrical/voltagesource_preisacheverett.h"
#include "../../electrical/voltagesource_preisachdiscrete.h"
#include "../../electrical/warburgCotanh.h"
#include "../../electrical/warburgTanh.h"
#include "../../electrical/zarc.h"
#include "../../electrical/zarcalg.h"

#include "../aging/aging_twoport_class_wrapper.h"
#include "../object/objectclasswrapper.h"
#include "../state/stateclasswrapper.h"

#include "electrical_xml_validator.h"

#include "../../exceptions/error_proto.h"


using namespace electrical;
extern template class electrical::TwoPort< myMatrixType >;
extern template class electrical::OhmicResistance< myMatrixType >;
extern template class electrical::Capacity< myMatrixType >;
extern template class electrical::ParallelRC< myMatrixType >;
extern template class electrical::ParallelTwoPort< myMatrixType >;
extern template class electrical::SerialTwoPort< myMatrixType >;
extern template class electrical::Cellelement< myMatrixType >;
extern template class electrical::Zarc< myMatrixType >;
extern template class electrical::ZarcAlg< myMatrixType >;
extern template class electrical::VoltageSource< myMatrixType >;
extern template class electrical::VoltageSource_OneState< myMatrixType >;
extern template class electrical::VoltageSource_PreisachEverett< myMatrixType >;
extern template class electrical::VoltageSource_PreisachDiscrete< myMatrixType >;
extern template class electrical::WarburgTanh< myMatrixType >;
extern template class electrical::WarburgCotanh< myMatrixType >;
extern template class electrical::Rmphn< myMatrixType >;
extern template class electrical::SphericalDiffusion< myMatrixType >;
extern template class electrical::ConstantPhaseElement< myMatrixType >;
extern template class electrical::Inductance< myMatrixType >;
extern template class electrical::Electrode< myMatrixType >;
extern template class electrical::AnodeElement< myMatrixType >;
extern template class electrical::CathodeElement< myMatrixType >;
extern template class electrical::ElectrodeElement< myMatrixType >;

namespace factory
{

struct ArgumentTypeElectrical
{
    public:
    ArgumentTypeElectrical()
        : mCellDiscretization( 1 )
        , mObjectMultiplier( 1 )
        , mCapacityFactor( 1 )
        , mResistanceFactor( 1 )
        , mSetReverseLookup( false )
        , mSocDivisorAnode( 1 )
        , mSocDivisorKathode( 1 )
        , mSoc( nullptr )
        , mCrate( nullptr )
        , mCdirection( nullptr )
        , mParentCell( nullptr )
        , mMakeAllECMElementsObservable( false )
    {
    }

    size_t mCellDiscretization;

    int mObjectMultiplier;

    double mCapacityFactor;
    double mResistanceFactor;

    bool mSetReverseLookup;

    size_t mSocDivisorAnode;
    size_t mSocDivisorKathode;

    boost::shared_ptr< state::Soc > mSoc;
    boost::shared_ptr< state::CRateState< double > > mCrate;
    boost::shared_ptr< state::CDirection< double > > mCdirection;
    boost::shared_ptr< electrical::Cellelement< myMatrixType > > mParentCell;

    /// make all twoports observable that are a discrete element in the ECM. Does not apply to serial and parallel
    /// twoports and elements that don't explicitly appear in the ECM (e.g. rc elements inside a warburg)
    bool mMakeAllECMElementsObservable;
};

/// Classwrapper base class for being used in a generic ::electrical::TwoPort Factory. This class has to be derived.
template < typename MatrixT >
class ElectricalClassWrapperBase : public ClassWrapperBase< TwoPort< MatrixT >, ArgumentTypeElectrical >
{
    public:
    /**
     * Classwrapper constructor.
     * The three different factories are supplied via parameter and are used for other instances ctor parameters.
     * @param electricalFactory A Factory for class in the ::electrical namespace
     * @param objectFactory A Factory for class in the ::object namespace
     * @param stateFactory A Factory for class in the state namespace
     */
    ElectricalClassWrapperBase( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                                Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                                Factory< state::State, ArgumentTypeState > *stateFactory )
        : mElectricalFactory( electricalFactory )
        , mObjectFactory( objectFactory )
        , mStateFactory( stateFactory )
    {
    }

    protected:
    typename electrical::TwoPort< MatrixT >::DataType CreateElectricalData()
    {
        return boost::make_shared< ElectricalDataStruct< ScalarUnit > >();
    }
    /// Get an ::electrical::TwoPort Factory.
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *GetElectricalFactory()
    {
        return mElectricalFactory;
    }

    /// Get an ::object::Object Factory.
    Factory< object::Object< double >, ArgumentTypeObject< double > > *GetObjectFactory() { return mObjectFactory; }

    /// Get a state::State Factory.
    Factory< state::State, ArgumentTypeState > *GetStateFactory() { return mStateFactory; }

    /// Returns ArgumentType for adjusting ::object::Object creation if cell consist of more than one cell element
    ArgumentTypeObject< double > *SetObjectFactorToArg( const ArgumentTypeElectrical *argElectrical, bool multiply,
                                                        boost::scoped_ptr< ArgumentTypeObject< double > > &argObject,
                                                        typename electrical::TwoPort< MatrixT >::DataType &dataStruct )
    {
        if ( argElectrical )
        {
            if ( argElectrical->mCellDiscretization != 1 )
            {
                if ( !argObject )
                    argObject.reset( new ArgumentTypeObject< double > );

                argObject->mObjectFactor = argElectrical->mCellDiscretization;

                if ( !multiply )
                    argObject->mObjectFactor = 1.0 / argObject->mObjectFactor;
            }

            if ( argElectrical->mSoc )
                argObject->mSoc = argElectrical->mSoc;
            if ( multiply )
                argObject->mObjectFactor *= argElectrical->mResistanceFactor;
        }

        if ( argObject && dataStruct )
        {
            argObject->mElectricalDataStruct = dataStruct;
        }

        return argObject.get();
    }

    bool SimplifyRC( const xmlparser::XmlParameter *param, double tauLookup )
    {
        return tauLookup < GetTauForSimplification( param );
    }

    boost::shared_ptr< electrical::ElectrodeElement< MatrixT > >
    CreateElectrodeElement( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg )
    {
        if ( !arg )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "arg",
                                                 "ElectricalClassWrapper::ElectricalClassWrapperBase::"
                                                 "CreateElectrodeElement" );

        if ( !arg->mSoc )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "mSoc",
                                                 "ElectricalClassWrapper::ElectricalClassWrapperBase::"
                                                 "CreateElectrodeElement" );

        if ( !arg->mParentCell )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException",
                                                 "mParentCell",
                                                 "ElectricalClassWrapper::ElectricalClassWrapperBase::"
                                                 "CreateElectrodeElement" );


        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );

        bool isAnodeElement = param->HasElementAttribute( "class", "AnodeElement" );

        boost::shared_ptr< state::Soc > socObject;

        if ( param->HasElementDirectChild( "Soc" ) )
        {
            boost::scoped_ptr< ArgumentTypeState > argState( new ArgumentTypeState() );
            argState->mCapacityFactor = arg->mCapacityFactor;
            socObject = boost::static_pointer_cast< state::Soc >(
             this->GetStateFactory()->CreateInstance( param->GetElementChild( "Soc" ), argState.get() ) );
        }
        else
        {
            boost::shared_ptr< state::Soc > parentSoc = arg->mSoc;

            const size_t socDivisor = isAnodeElement ? arg->mSocDivisorAnode : arg->mSocDivisorKathode;

            const double parentCellSoc = parentSoc->GetValue();
            const double parentCellInitialCapacity = parentSoc->GetInitialCapacity<>() / ( 3600 * socDivisor );
            const double parentCellActualCapacity = parentSoc->GetActualCapacity<>() / ( 3600 * socDivisor );

            socObject = boost::make_shared< state::Soc >( parentCellInitialCapacity, parentCellActualCapacity, parentCellSoc );
        }

        auto thermalState = arg->mParentCell->GetThermalState();
        boost::shared_ptr< ElectrodeElement< MatrixT > > electrodeElement;
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false );

        if ( isAnodeElement )
        {
            electrodeElement = boost::make_shared< AnodeElement< MatrixT > >( socObject, thermalState, observable );
        }
        else
        {
            electrodeElement = boost::make_shared< CathodeElement< MatrixT > >( socObject, thermalState, observable );
        }

        for ( const auto &child : children )
        {
            boost::shared_ptr< TwoPort< MatrixT > > childtp;

            boost::shared_ptr< ArgumentTypeElectrical > argChildren = boost::make_shared< ArgumentTypeElectrical >();
            argChildren->mSoc = socObject;
            argChildren->mResistanceFactor = arg->mResistanceFactor;

            if ( child->HasElementAttribute( "class", "VoltageSource" ) )
            {
                if ( isAnodeElement )
                    argChildren->mObjectMultiplier = -1;

                childtp = this->GetElectricalFactory()->CreateInstance( child, argChildren.get() );
            }

            else
                childtp = this->GetElectricalFactory()->CreateInstance( child, argChildren.get() );

            if ( childtp != nullptr )
                electrodeElement->AddChild( childtp );
        }

        arg->mParentCell->AddElectrodeElement( electrodeElement );

        return electrodeElement;
    }

    private:
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *const mElectricalFactory;
    Factory< object::Object< double >, ArgumentTypeObject< double > > *const mObjectFactory;
    Factory< state::State, ArgumentTypeState > *const mStateFactory;

    double GetTauForSimplification( const xmlparser::XmlParameter *param )
    {
        double minDt = 1.0e-6;    // Default Initializer if Samplerate is not given in XML file
        if ( param->GetConfigurationRoot()->HasElementDirectChild( "Options" ) &&
             param->GetConfigurationRoot()->GetElementChild( "Options" )->HasElementDirectChild( "SampleRate" ) )
        {
            double samp =
             param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementDoubleValue( "SampleRate", 1000 );
            if ( samp > 1.0e-8 )    // Check if greater 0
                minDt = 1.0 / samp;
            else
                ErrorFunction< std::range_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );

            if ( param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementChild( "SampleRate" )->HasElementAttribute( "SimplificationType" ) )
            {
                const char *mySimplificationType = param->GetConfigurationRoot()
                                                    ->GetElementChild( "Options" )
                                                    ->GetElementChild( "SampleRate" )
                                                    ->GetElementAttributeCharPtrValue( "SimplificationType" );
                misc::equal_str myCompareObject;
                if ( myCompareObject( mySimplificationType, "Euler" ) )
                {
                    return 0.50001 * minDt;    // Exakt Stability limit at 0.5*minDt, safe side given with 0.50001
                }
            }
        }
        return minDt / 3.0;
    }
};

/// Classwrapper for electrical namespace. This template class has to be specialized in order to create an instance of a
/// particular class.
template < typename MatrixT, template < typename > class TConcrete >
class ElectricalClassWrapper : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }
};

/// Classwrapper for electrical::Capacity
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Capacity > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();

        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, false, argObject, elecData ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new Capacity< MatrixT >( obj, observable, elecData ) );
    }
};

/// Classwrapper for electrical::OhmicResistance
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, OhmicResistance > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();

        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new OhmicResistance< MatrixT >( obj, observable, elecData ) );
    }
};

/// Classwrapper for electrical::ConstantPhaseElement dummy
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ConstantPhaseElement > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();

        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new ConstantPhaseElement< MatrixT >( obj, observable, elecData ) );
    }
};

/// Classwrapper for electrical::Inductance dummy
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Inductance > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();

        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new Inductance< MatrixT >( obj, observable, elecData ) );
    }
};


/// Classwrapper for electrical::VoltageSource
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, VoltageSource > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        auto elecData = this->CreateElectricalData();
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        argObject->mElectricalDataStruct = elecData;

        if ( arg )
        {
            if ( arg->mSoc )
                argObject->mSoc = arg->mSoc;

            if ( arg->mObjectMultiplier )
                argObject->mObjectFactor = arg->mObjectMultiplier;

            if ( arg->mSetReverseLookup )
                argObject->mSetReverseLookUp = arg->mSetReverseLookup;
        }

        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "Object" ) );
        boost::shared_ptr< object::Object< double > > obj( this->GetObjectFactory()->CreateInstance( objparam, argObject.get() ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new VoltageSource< MatrixT >( obj, observable, elecData ) );
    }
};

/// Classwrapper for electrical::VoltageSource_OneState
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, VoltageSource_OneState > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );
        

        boost::shared_ptr< xmlparser::XmlParameter > OCVavg_objparam( param->GetElementChild( "LookupAverageOCV" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();

        boost::shared_ptr< object::Object< double > > OCVavg_Obj(
         this->GetObjectFactory()->CreateInstance( OCVavg_objparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        boost::shared_ptr< xmlparser::XmlParameter > OCVwidth_objparam( param->GetElementChild( "LookupHystWidth" ) );

        argObject.reset( new ArgumentTypeObject< double > );
        argObject->mElectricalDataStruct = elecData;

        boost::shared_ptr< object::Object< double > > OCVwidth_Obj(
         this->GetObjectFactory()->CreateInstance( OCVwidth_objparam, argObject.get() ) );

        boost::shared_ptr< xmlparser::XmlParameter > OCVtau_objparam( param->GetElementChild( "LookupHystTau" ) );

        argObject.reset( new ArgumentTypeObject< double > );
        argObject->mElectricalDataStruct = elecData;

        boost::shared_ptr< object::Object< double > > OCVtau_Obj(
         this->GetObjectFactory()->CreateInstance( OCVtau_objparam, argObject.get() ) );

        double InitialCapacity = 1;
        if ( param->HasElementDirectChild( "InitialCapacity" ) )
            InitialCapacity = param->GetElementDoubleValue( "InitialCapacity" );
        double gamma = 1;
        if ( param->HasElementDirectChild( "gamma" ) )
            gamma = param->GetElementDoubleValue( "gamma" );
        double eta = 1;
        if ( param->HasElementDirectChild( "eta" ) )
            eta = param->GetElementDoubleValue( "eta" );

        return boost::shared_ptr< TwoPort< MatrixT > >( new VoltageSource_OneState< MatrixT >( OCVavg_Obj, OCVwidth_Obj, OCVtau_Obj, InitialCapacity, eta, observable, elecData ) );
    }
};

/// Classwrapper for electrical::VoltageSource_PreisachEverett
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, VoltageSource_PreisachEverett > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > objparam( param->GetElementChild( "LookupEverettFunctionValues" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();


        boost::shared_ptr< object::Object< double > > obj(
         this->GetObjectFactory()->CreateInstance( objparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        double minOCV = 0;
        if ( param->HasElementDirectChild( "OCVmin" ) )
            minOCV = param->GetElementDoubleValue( "OCVmin" );

        boost::shared_ptr< ArgumentTypeState > argState = boost::make_shared< ArgumentTypeState >();
        boost::shared_ptr< state::Soc > socObj( boost::static_pointer_cast< state::Soc >(this->GetStateFactory()->CreateInstance( param->GetElementChild( "Soc" ), argState.get() ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new VoltageSource_PreisachEverett< MatrixT >( socObj, minOCV, obj, observable, elecData ) );
    }
};

/// Classwrapper for electrical::VoltageSource_PreisachDiscrete
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, VoltageSource_PreisachDiscrete > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > objparamWeight( param->GetElementChild( "LookupWeights" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );
        auto elecData = this->CreateElectricalData();
        boost::shared_ptr< object::Object< double > > objWeight(
         this->GetObjectFactory()->CreateInstance( objparamWeight, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        boost::shared_ptr< xmlparser::XmlParameter > objparamSwitchUpTH( param->GetElementChild( "LookupSwitchUpTH" ) );
        argObject.reset( new ArgumentTypeObject< double > );
        argObject->mElectricalDataStruct = elecData;
        boost::shared_ptr< object::Object< double > > objSwitchUpTH(
         this->GetObjectFactory()->CreateInstance( objparamSwitchUpTH, argObject.get() ) );

        boost::shared_ptr< xmlparser::XmlParameter > objparamSwitchDownTH(param->GetElementChild( "LookupSwitchDownTH" ) );
        argObject.reset( new ArgumentTypeObject< double > );
        argObject->mElectricalDataStruct = elecData;
        boost::shared_ptr< object::Object< double > > objSwitchDownTH(
         this->GetObjectFactory()->CreateInstance( objparamSwitchDownTH, argObject.get() ) );  

        int Discretization = 10;
        if ( param->HasElementDirectChild( "Discretization" ) )
            Discretization = param->GetElementDoubleValue( "Discretization" );

        boost::shared_ptr< ArgumentTypeState > argState = boost::make_shared< ArgumentTypeState >();
        boost::shared_ptr< state::Soc > socObj( boost::static_pointer_cast< state::Soc >(
         this->GetStateFactory()->CreateInstance( param->GetElementChild( "Soc" ), argState.get() ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >(
         new VoltageSource_PreisachDiscrete< MatrixT >( socObj, Discretization, objWeight, objSwitchUpTH, objSwitchDownTH, observable, elecData ) );
    }
};

/// Classwrapper for electrical::ParallelRC
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ParallelRC > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "LookupOhmicResistance" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();

        boost::shared_ptr< object::Object< double > > rObj(
         this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        boost::shared_ptr< xmlparser::XmlParameter > cobjparam( param->GetElementChild( "LookupTau" ) );

        argObject.reset( new ArgumentTypeObject< double > );
        argObject->mElectricalDataStruct = elecData;

        boost::shared_ptr< object::Object< double > > cObj(
         this->GetObjectFactory()->CreateInstance( cobjparam, argObject.get() ) );

        double maxRelaxationTime = cObj.get()->GetMaxValueOfLookup();

        if ( ElectricalClassWrapperBase< MatrixT >::SimplifyRC( param, maxRelaxationTime ) )
        {
            return boost::shared_ptr< TwoPort< MatrixT > >( new OhmicResistance< MatrixT >( rObj, observable, elecData ) );
        }
        return boost::shared_ptr< TwoPort< MatrixT > >( new ParallelRC< MatrixT >( rObj, cObj, observable, elecData ) );
    }
};

/// Classwrapper for electrical::Rmphn
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Rmphn > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
        , mRCCreater( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > rmpobjparam( param->GetElementChild( "OhmicResistance_RMP" ) );
        boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "LookupOhmicResistance" ) );

        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );

        auto elecData = this->CreateElectricalData();

        boost::shared_ptr< object::Object< double > > rmpObj(
         this->GetObjectFactory()->CreateInstance( rmpobjparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        boost::shared_ptr< object::Object< double > > rObj(
         this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );


        auto rmphn = boost::make_shared< Rmphn< MatrixT > >( rObj, rmpObj, observable, elecData );

        auto rc = mRCCreater.CreateInstance( param, arg );
        rmphn->AddChild( rc );
        return rmphn;

        //        auto rmphn = boost::make_sharedTwoPort< MatrixT >(

        //        return boost::shared_ptr< TwoPort< MatrixT > >( new ParallelRC< MatrixT >( rObj, cObj, observable,
        //        elecData ) );
    }

    private:
    ElectricalClassWrapper< MatrixT, electrical::ParallelRC > mRCCreater;
};


/// Classwrapper for electrical::SpericalDiffusion
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, SphericalDiffusion > : public ElectricalClassWrapperBase< MatrixT >
{
    BOOST_STATIC_ASSERT( sizeof( double ) >= sizeof( double * ) );    // This only works if pointers are smaller than doubles

    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    bool SaneInput( int maxElements ) const
    {
        if ( maxElements <= 0 )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "negativeValue", maxElements );
        }
        if ( maxElements > this->maximumElements )
        {
            ErrorFunction< std::out_of_range >( __FUNCTION__, __LINE__, __FILE__, "tooLargeValue", maxElements, this->maximumElements );
        }
        return true;
    }


    double ConvertObjectPointer2double( object::Object< double > *objP ) const
    {
        double address = 0;
        memcpy( static_cast< void * >( &address ), static_cast< void * >( &objP ), sizeof( double * ) );
        return address;
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        auto elecData = this->CreateElectricalData();

        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        int iMaxElements = param->GetElementAttributeIntValue( "RCCounter", 5 );    // default is 5 RC Elements

        SaneInput( iMaxElements );    // throws

        boost::scoped_ptr< ArgumentTypeObject< double > > argument( new ArgumentTypeObject< double > );

        boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "OhmicResistance" ) );

        boost::shared_ptr< xmlparser::XmlParameter > tauobjparam( param->GetElementChild( "Tau" ) );
        boost::shared_ptr< object::Object< double > > rObj(
         this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argument, elecData ) ) );

        boost::shared_ptr< object::Object< double > > tauObj(
         this->GetObjectFactory()->CreateInstance( tauobjparam, this->SetObjectFactorToArg( arg, true, argument, elecData ) ) );


        const double maxRelaxationTime = tauObj.get()->GetMaxValueOfLookup();
        size_t rcElementsCount = 0;

        const int withCapacity = static_cast< int >( param->GetElementAttributeBoolValue( "withCapacity", false ) );

        for ( ; rcElementsCount < maximumElements; ++rcElementsCount )
        {
            if ( ElectricalClassWrapperBase< MatrixT >::SimplifyRC(
                  param, maxRelaxationTime * electrical::SphericalDiffusion< myMatrixType >::mTauFactor[rcElementsCount] ) )
                break;
        }

        auto tp = boost::make_shared< SphericalDiffusion< MatrixT > >( rObj, tauObj, rcElementsCount, withCapacity,
                                                                       observable, elecData );

        return tp;
    }

    private:
    static const int maximumElements = 29;
};

/// Classwrapper for electrical::WarburgCotanh
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, WarburgCotanh > : public ElectricalClassWrapperBase< MatrixT >
{
    BOOST_STATIC_ASSERT( sizeof( double ) >= sizeof( double * ) );    // This only works if pointers are smaller than doubles

    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
        // Nedded for HACK_1
    }

    double CalculateCoefficent( size_t elementNumber ) const
    {
        return 4.0 / std::pow( ( elementNumber * 3.141592653589793 ), 2.0 );
    }

    bool SaneInput( int maxElements )
    {
        if ( maxElements <= 0 )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "negativeValue", maxElements );
        }
        return true;
    }


    void AddResidualOhmicResistance( boost::shared_ptr< SerialTwoPort< MatrixT > > tp,
                                     boost::shared_ptr< xmlparser::XmlParameter > cObjParam,
                                     ArgumentTypeObject< double > *arg, size_t start, size_t end, bool observable,
                                     typename electrical::TwoPort< MatrixT >::DataType &elecData )
    {
        double residualOhmicResistance = 0;

        for ( ; start < end; ++start )
            residualOhmicResistance += CalculateCoefficent( start + 1 );

        arg->mDoubleMap["Multiplier"] = residualOhmicResistance;
        arg->mElectricalDataStruct = elecData;

        boost::shared_ptr< object::Object< double > > rObj( this->GetObjectFactory()->CreateInstance( cObjParam, arg ) );    // R
        tp->AddChild( new OhmicResistance< MatrixT >( rObj, observable ) );    // OhmicRessitance needs its own state,
                                                                               // therefore don't pass elecData
    }

    double ConvertObjectPointer2double( object::Object< double > *objP )
    {
        double address = 0;
        memcpy( static_cast< void * >( &address ), static_cast< void * >( &objP ), sizeof( double * ) );
        return address;
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        auto elecData = this->CreateElectricalData();
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );
        const bool withCapacity = param->GetElementAttributeBoolValue( "withCapacity", true );
        int iMaxElements = param->GetElementAttributeIntValue( "RCCounter", 5 );    // default is 5 RC Elements
        size_t maxElements( iMaxElements );

        SaneInput( iMaxElements );    // throws

        boost::shared_ptr< WarburgCotanh< MatrixT > > tp( new WarburgCotanh< MatrixT >( observable, elecData ) );

        // HACK: GIGANTIC HACK
        // After reading these lines you should feel dirty

        boost::scoped_ptr< ArgumentTypeObject< double > > argument( new ArgumentTypeObject< double > );
        argument->mElectricalDataStruct = elecData;

        boost::shared_ptr< xmlparser::XmlParameter > sigmaObjParam( param->GetElementChild( "Sigma" ) );
        boost::shared_ptr< object::Object< double > > sigmaObj(
         this->GetObjectFactory()->CreateInstance( sigmaObjParam, argument.get() ) );

        argument->mDoubleMap.insert( std::pair< const char *, double >( "Operator", factory::OBJECT_MULTIPLICATION ) );    // Achtung

        // From here we are in nomansland where Object are mere bytes and the address of and Obj is represented as a
        // double...
        // pack our objectaddress into a nice little double
        // no easy conversion is known

        double addressR = ConvertObjectPointer2double( sigmaObj.get() );
        argument->mDoubleMap["Operand"] = addressR;

        // sigma^2
        boost::shared_ptr< object::Object< double > > sigmaSquared(
         this->GetObjectFactory()->CreateInstance( sigmaObjParam, argument.get() ) );


        boost::shared_ptr< xmlparser::XmlParameter > cObjParam( param->GetElementChild( "Clim" ) );


        // Add the packed to our map
        for ( size_t counter = 0; counter < maxElements; ++counter )
        {
            argument->mDoubleMap["Multiplier"] = CalculateCoefficent( counter + 1 );

            double addressSigmaSquared = ConvertObjectPointer2double( sigmaSquared.get() );
            argument->mDoubleMap["Operand"] = addressSigmaSquared;

            boost::shared_ptr< object::Object< double > > rObj(
             this->GetObjectFactory()->CreateInstance( cObjParam, this->SetObjectFactorToArg( arg, true, argument, elecData ) ) );    // R
            argument->mObjectFactor = 1.0;

            double addressTau = ConvertObjectPointer2double( rObj.get() );

            argument->mDoubleMap["Operand"] = addressTau;
            argument->mDoubleMap["Multiplier"] = 0.5;

            boost::shared_ptr< object::Object< double > > tauObj(
             this->GetObjectFactory()->CreateInstance( cObjParam, this->SetObjectFactorToArg( arg, false, argument, elecData ) ) );    // Tau
            argument->mObjectFactor = 1.0;

            double maxRelaxationTime = tauObj.get()->GetMaxValueOfLookup();

            if ( ElectricalClassWrapperBase< MatrixT >::SimplifyRC( param, maxRelaxationTime ) )
            {
                argument->mDoubleMap["Operand"] = addressSigmaSquared;
                argument->mDoubleMap["Multiplier"] = 1.0;
                AddResidualOhmicResistance( tp, cObjParam, this->SetObjectFactorToArg( arg, true, argument, elecData ),
                                            counter, maxElements, false, elecData );
                argument->mObjectFactor = 1.0;
                break;
            }

            tp->AddChild( new ParallelRC< MatrixT >( rObj, tauObj, false ) );    // <-- don't pass elecData here as
                                                                                 // states shoudl be dependent on
                                                                                 // elecData
        }

        if ( withCapacity )
        {
            boost::scoped_ptr< ArgumentTypeObject< double > > argForCapacity( new ArgumentTypeObject< double >() );

            boost::shared_ptr< object::Object< double > > cObj(
             this->GetObjectFactory()->CreateInstance( cObjParam, this->SetObjectFactorToArg( arg, false, argForCapacity, elecData ) ) );    // C
            tp->AddChild( new Capacity< MatrixT >( cObj, false ) );
        }
        return tp;
    }
};

/// Classwrapper for electrical::WarburgTanh
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, WarburgTanh > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    double CalculateCoefficent( size_t elementNumber ) const { return std::pow( ( 2 * elementNumber ) - 1, 2.0 ); }

    bool SaneInput( int maxElements )
    {
        if ( maxElements <= 0 )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "negativeValue", maxElements );
        }
        return true;
    }


    void AddResidualOhmicResistance( boost::shared_ptr< SerialTwoPort< MatrixT > > tp,
                                     boost::shared_ptr< xmlparser::XmlParameter > robjparam, size_t start, size_t end,
                                     bool observable, typename electrical::TwoPort< MatrixT >::DataType &elecData )
    {
        double residualOhmicResistance = 0;
        for ( ; start < end; ++start )
            residualOhmicResistance += 1 / CalculateCoefficent( start + 1 );

        boost::scoped_ptr< ArgumentTypeObject< double > > argument( new ArgumentTypeObject< double > );
        const char *mapEntry = "Multiplier";
        argument->mDoubleMap.insert( std::pair< const char *, double >( mapEntry, residualOhmicResistance ) );
        argument->mElectricalDataStruct = elecData;

        boost::shared_ptr< object::Object< double > > rObj(
         this->GetObjectFactory()->CreateInstance( robjparam, argument.get() ) );

        tp->AddChild( new OhmicResistance< MatrixT >( rObj, observable ) );
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        auto elecData = this->CreateElectricalData();
        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );
        int iMaxElements = param->GetElementAttributeIntValue( "RCCounter", 5 );    // default is 5 RC Elements
        size_t maxElements( iMaxElements );

        SaneInput( iMaxElements );    // throws

        boost::shared_ptr< WarburgTanh< MatrixT > > tp( new WarburgTanh< MatrixT >( observable, elecData ) );


        for ( size_t counter = 0; counter < maxElements; ++counter )
        {

            boost::scoped_ptr< ArgumentTypeObject< double > > argument( new ArgumentTypeObject< double > );
            const char *mapEntry = "Multiplier";
            argument->mDoubleMap.insert( std::pair< const char *, double >( mapEntry, 1.0 / CalculateCoefficent( counter + 1 ) ) );

            boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "OhmicResistance" ) );

            boost::shared_ptr< object::Object< double > > rObj(
             this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argument, elecData ) ) );
            argument->mObjectFactor = 1.0;

            boost::shared_ptr< xmlparser::XmlParameter > cobjparam( param->GetElementChild( "Tau" ) );
            boost::shared_ptr< object::Object< double > > tauObj(
             this->GetObjectFactory()->CreateInstance( cobjparam, argument.get() ) );

            double maxRelaxationTime = tauObj.get()->GetMaxValueOfLookup();

            if ( ElectricalClassWrapperBase< MatrixT >::SimplifyRC( param, maxRelaxationTime ) )
            {
                AddResidualOhmicResistance( tp, robjparam, counter, maxElements, false, elecData );
                break;
            }

            tp->AddChild( new ParallelRC< MatrixT >( rObj, tauObj, false ) );
        }
        return tp;
    }
};

/// Classwrapper for electrical::ParallelRCAlg
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ParallelRCAlg > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        auto elecData = this->CreateElectricalData();

        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );

        boost::shared_ptr< xmlparser::XmlParameter > robjparam( param->GetElementChild( "RObject" ) );
        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );
        boost::shared_ptr< object::Object< double > > rObj(
         this->GetObjectFactory()->CreateInstance( robjparam, this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) );

        boost::shared_ptr< xmlparser::XmlParameter > cobjparam( param->GetElementChild( "CObject" ) );
        argObject.reset( new ArgumentTypeObject< double > );
        boost::shared_ptr< object::Object< double > > cObj(
         this->GetObjectFactory()->CreateInstance( cobjparam, this->SetObjectFactorToArg( arg, false, argObject, elecData ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new ParallelRCAlg< MatrixT >( rObj, cObj, observable, elecData ) );
    }
};

/// Classwrapper for electrical::Cellelement
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Cellelement > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
        , mCellCounter( 0 )
    {
    }

    boost::shared_ptr< state::SurfaceSoc > CreateSurfaceSoc( const xmlparser::XmlParameter *param, const ArgumentTypeState *arg = 0 )
    {
        typedef state::SurfaceSoc SurfaceSocState_t;
        return boost::static_pointer_cast< SurfaceSocState_t >(
         this->GetStateFactory()->CreateInstance( param->GetElementChild( "SurfaceSoc" ), arg ) );
    }

    void HandleChildren( const std::vector< boost::shared_ptr< xmlparser::XmlParameter > > &childrenNode,
                         const boost::shared_ptr< electrical::TwoPortWithChild< MatrixT > > &parentElement,
                         const boost::shared_ptr< ArgumentTypeElectrical > arg,
                         std::vector< electrical::TwoPort< myMatrixType > * > &surfaceEffectedElements )
    {
        for ( const auto &child : childrenNode )
        {
            boost::shared_ptr< electrical::TwoPort< myMatrixType > > element(
             this->GetElectricalFactory()->CreateInstance( child.get(), arg.get() ) );
            if ( element == nullptr )
                continue;

            if ( child->HasElementAttribute( "SurfaceEffect" ) )
                surfaceEffectedElements.push_back( element.get() );

            parentElement->AddChild( element );
        }
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        typedef state::ThermalState< double > ThermalState_t;
        typedef state::CRateState< double > CRateState_t;
        typedef state::CDirection< double > CDirection_t;
        typedef state::Soc SocState_t;
        typedef state::LHDState< double > LHDState_t;

        const bool isHalfcellsimulation = param->HasElementDirectChild( "Anode" );

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children;

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > anodeChildren;
        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > cathodeChildren;
        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > otherChildren;

        size_t countAnode = 1;
        size_t countCathode = 1;

        size_t count = 1;

        // Halfcellsimulation
        if ( isHalfcellsimulation )
        {
            anodeChildren = param->GetElementChild( "Anode" )->GetElementChildren( "Children" );
            cathodeChildren = param->GetElementChild( "Cathode" )->GetElementChildren( "Children" );
            otherChildren = param->GetElementChild( "Other" )->GetElementChildren( "Children" );

            countAnode = param->GetElementChild( "Anode" )
                          ->GetElementChild( "Children" )
                          ->GetElementAttributeUnsignedIntValue( "count", 1 );
            countCathode = param->GetElementChild( "Cathode" )
                            ->GetElementChild( "Children" )
                            ->GetElementAttributeUnsignedIntValue( "count", 1 );
        }

        // Fullcellsimulation
        else
        {
            children = param->GetElementChildren( "Children" );
            count = param->GetElementChild( "Children" )->GetElementAttributeUnsignedIntValue( "count", 1 );
        }

        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false );

        const size_t numberOfCellElements =
         param->HasElementDirectChild( "ThermalBlock" ) ?
          factory::GetElectricalDiscretization( param->GetElementChild( "ThermalBlock" ) ) :
          1;

        boost::shared_ptr< ArgumentTypeState > argState = boost::make_shared< ArgumentTypeState >();
        boost::shared_ptr< ArgumentTypeElectrical > argElectrical = boost::make_shared< ArgumentTypeElectrical >();
        if ( arg )
            argElectrical->mMakeAllECMElementsObservable = arg->mMakeAllECMElementsObservable;

        if ( numberOfCellElements != 1 )
        {
            argState->mCellDiscretization = numberOfCellElements;
            argElectrical->mCellDiscretization = numberOfCellElements;
        }

        boost::shared_ptr< ParallelTwoPort< MatrixT > > parallelCellElements( new ParallelTwoPort< MatrixT >( true ) );

        for ( size_t i = 0; i < numberOfCellElements; ++i )
        {
            boost::shared_ptr< ThermalState_t > tempObj( boost::static_pointer_cast< ThermalState_t >(
             this->GetStateFactory()->CreateInstance( param->GetElementChild( "ThermalState" ), argState.get() ) ) );


            boost::shared_ptr< SocState_t > socObj( boost::static_pointer_cast< SocState_t >(
             this->GetStateFactory()->CreateInstance( param->GetElementChild( "Soc" ), argState.get() ) ) );

            boost::shared_ptr< CRateState_t > crateObj;
            if ( param->HasElementDirectChild( "CRateState" ) )
            {
                crateObj = boost::static_pointer_cast< CRateState_t >(
                 this->GetStateFactory()->CreateInstance( param->GetElementChild( "CRateState" ), argState.get() ) );
            }

            boost::shared_ptr< CDirection_t > cdirectionObj;
            if ( param->HasElementDirectChild( "CDirection" ) )
            {
                cdirectionObj = boost::static_pointer_cast< CDirection_t >(
                 this->GetStateFactory()->CreateInstance( param->GetElementChild( "CDirection" ), argState.get() ) );
            }

            boost::shared_ptr< LHDState_t > lhdObj;
            if ( param->HasElementDirectChild( "LHDState" ) )
            {
                lhdObj = boost::static_pointer_cast< LHDState_t >(
                 this->GetStateFactory()->CreateInstance( param->GetElementChild( "LHDState" ), argState.get() ) );
            }

            boost::shared_ptr< object::Object< double > > reversibleHeat( 0 );
            if ( param->HasElementDirectChild( "ReversibleHeat" ) )
            {
                reversibleHeat = this->GetObjectFactory()->CreateInstance( param->GetElementChild( "ReversibleHeat" ) );
            }

            boost::shared_ptr< state::SurfaceSoc > surfaceSoc;

            // TODO: Should be probably checked if it is set to True or not False
            if ( param->HasElementDirectChild( "SurfaceSoc" ) )
                surfaceSoc = CreateSurfaceSoc( param, argState.get() );

            boost::shared_ptr< electrical::Cellelement< MatrixT > > cellelement;
            if ( param->HasElementDirectChild( "CRateState" ) && !param->HasElementDirectChild( "LHDState" ) )
            {
                cellelement = boost::make_shared< Cellelement< MatrixT > >( mCellCounter, tempObj, socObj, crateObj, true,
                                                                            this->CreateElectricalData(), reversibleHeat );
            }
            else if ( param->HasElementDirectChild( "CRateState" ) && param->HasElementDirectChild( "LHDState" ) )
            {
                cellelement = boost::make_shared< Cellelement< MatrixT > >( mCellCounter, tempObj, socObj, crateObj, lhdObj,
                                                                            true, this->CreateElectricalData(), reversibleHeat );
            }
            else if ( param->HasElementDirectChild( "CDirection" ) && !param->HasElementDirectChild( "LHDState" ) )
            {
                cellelement = boost::make_shared< Cellelement< MatrixT > >( mCellCounter, tempObj, socObj, cdirectionObj, true,
                                                                            this->CreateElectricalData(), reversibleHeat );
            }
            else if ( param->HasElementDirectChild( "CDirection" ) && param->HasElementDirectChild( "LHDState" ) )
            {
                cellelement = boost::make_shared< Cellelement< MatrixT > >( mCellCounter, tempObj, socObj, cdirectionObj, lhdObj,
                                                                            true, this->CreateElectricalData(), reversibleHeat );
            }
            else
            {
                cellelement = boost::make_shared< Cellelement< MatrixT > >( mCellCounter, tempObj, socObj, true,
                                                                            this->CreateElectricalData(), reversibleHeat );
            }

            std::vector< electrical::TwoPort< myMatrixType > * > surfaceEffectedElements;

            if ( isHalfcellsimulation )
            {
                boost::shared_ptr< ParallelTwoPort< MatrixT > > anode = boost::make_shared< Electrode< MatrixT > >( false );
                boost::shared_ptr< ParallelTwoPort< MatrixT > > cathode = boost::make_shared< Electrode< MatrixT > >( false );

                cellelement->SetElectrodes( anode, cathode );

                boost::shared_ptr< ArgumentTypeElectrical > argElectrodeElements( new ArgumentTypeElectrical( *argElectrical ) );
                argElectrodeElements->mSoc = socObj;
                argElectrodeElements->mParentCell = cellelement;
                argElectrodeElements->mSocDivisorAnode = countAnode * anodeChildren.size();
                argElectrodeElements->mSocDivisorKathode = countCathode * cathodeChildren.size();

                for ( size_t i = 0; i < countAnode; ++i )
                    HandleChildren( anodeChildren, anode, argElectrodeElements, surfaceEffectedElements );

                cellelement->AddChild( anode );

                HandleChildren( otherChildren, cellelement, argElectrical, surfaceEffectedElements );

                for ( size_t i = 0; i < countCathode; ++i )
                    HandleChildren( cathodeChildren, cathode, argElectrodeElements, surfaceEffectedElements );

                cellelement->AddChild( cathode );
            }

            else
            {
                for ( size_t i = 0; i < count; ++i )
                {
                    HandleChildren( children, cellelement, argElectrical, surfaceEffectedElements );
                }
            }

            // TODO: Should be probably checked if it is set to True or not False
            if ( param->HasElementDirectChild( "SurfaceSoc" ) )
            {
                surfaceSoc->SetElementsForLookup( surfaceEffectedElements );
                surfaceSoc->SetReverseLookup( this->GetObjectFactory()->CreateInstance(
                 param->GetElementChild( "SurfaceSoc" )->GetElementChild( "ReverseLookup" ) ) );

                cellelement->SetSurfaceSOC( surfaceSoc );
            }
            parallelCellElements->AddChild( cellelement );
        }

        boost::shared_ptr< TwoPort< MatrixT > > cell;

        if ( numberOfCellElements == 1 )
            cell = parallelCellElements->shared_at( 0 );

        else
            cell = parallelCellElements;

        // Increase the variable "mCellCounter" if the cellelement was created successfully
        ++mCellCounter;

        return cell;
    }

    private:
    size_t mCellCounter;
};

/// Classwrapper for electrical::AnodeElement
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, AnodeElement > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
        , mAnodeElementCounter( 0 )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        validator::ValidateAnodeElementNode( param, mAnodeElementCounter );

        if ( !arg )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ObjectNotExistException", "arg",
                                                 "ElectricalClassWrapper<AnodeElement>::CreateInstance" );

        const auto anodeNode = param->GetElementParent()->GetElementParent();

        double resistanceVariationFactor = 1;

        if ( anodeNode->HasElementDirectChild( "Variation" ) )
        {
            const auto variationNode = param->GetElementParent()->GetElementParent()->GetElementChild( "Variation" );

            const std::vector< double > variationResistanceValues =
             variationNode->GetElementDoubleVecValue( "Resistance" );
            resistanceVariationFactor = variationResistanceValues[mAnodeElementCounter];
        }

        boost::shared_ptr< ArgumentTypeElectrical > argAnodeElement = boost::make_shared< ArgumentTypeElectrical >( *arg );
        argAnodeElement->mResistanceFactor = resistanceVariationFactor;

        boost::shared_ptr< TwoPort< MatrixT > > anodeElement = this->CreateElectrodeElement( param, argAnodeElement.get() );

        mAnodeElementCounter++;

        return anodeElement;
    }

    private:
    double mAnodeElementCounter;
};

/// Classwrapper for electrical::CathodeElement
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, CathodeElement > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        validator::ValidateEletrodeElementNode( param );
        return this->CreateElectrodeElement( param, arg );
    }
};

/// Classwrapper for electrical::Zarc
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, Zarc > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        typedef object::LookupObj2dWithState< double > Obj2D_t;

        auto elecData = this->CreateElectricalData();

        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );
        double dtValue = 0.001;

        if ( param->GetConfigurationRoot()->HasElementDirectChild( "Options" ) &&
             param->GetConfigurationRoot()->GetElementChild( "Options" )->HasElementDirectChild( "SampleRate" ) )
        {
            double samp =
             param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementDoubleValue( "SampleRate", 1000 );
            if ( samp > 0.0 )
                dtValue = 1.0 / samp;
            else
                ErrorFunction< std::range_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );
        }

        boost::shared_ptr< Obj2D_t > tauObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupTau" ) ) ) );

        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );
        boost::shared_ptr< Obj2D_t > ohmObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupOhmicResistance" ),
                                                   this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) ) );

        boost::shared_ptr< Obj2D_t > phiObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupPhi" ) ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >( new Zarc< MatrixT >( tauObj, ohmObj, phiObj, dtValue, observable, elecData ) );
    }
};

/// Classwrapper for electrical::ZarcAlg
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ZarcAlg > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        typedef object::LookupObj2dWithState< double > Obj2D_t;

        auto elecData = this->CreateElectricalData();

        const bool observable = param->GetElementAttributeBoolValue( "observable", false ) ||
                                param->GetElementAttributeBoolValue( "SurfaceEffect", false ) ||
                                ( arg && arg->mMakeAllECMElementsObservable );
        double sampleRate = 0.001;
        if ( param->GetConfigurationRoot()->HasElementDirectChild( "Options" ) &&
             param->GetConfigurationRoot()->GetElementChild( "Options" )->HasElementDirectChild( "SampleRate" ) )
        {
            double samp =
             param->GetConfigurationRoot()->GetElementChild( "Options" )->GetElementDoubleValue( "SampleRate", 1000 );
            if ( samp > 0.0 )
                sampleRate = samp;
            else
                ErrorFunction< std::range_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidSampleRate" );
        }


        boost::shared_ptr< Obj2D_t > tauObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupTau" ) ) ) );

        boost::scoped_ptr< ArgumentTypeObject< double > > argObject( new ArgumentTypeObject< double > );
        boost::shared_ptr< Obj2D_t > ohmObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupOhmicResistance" ),
                                                   this->SetObjectFactorToArg( arg, true, argObject, elecData ) ) ) );

        boost::shared_ptr< Obj2D_t > phiObj( boost::static_pointer_cast< Obj2D_t >(
         this->GetObjectFactory()->CreateInstance( param->GetElementChild( "LookupPhi" ) ) ) );

        return boost::shared_ptr< TwoPort< MatrixT > >(
         new ZarcAlg< MatrixT >( tauObj, ohmObj, phiObj, sampleRate, observable, elecData ) );
    }
};

/// Classwrapper for electrical::ParallelTwoPort
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, ParallelTwoPort > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        validator::ValidateParallelTwoPortNode( param );
        const bool observable = param->GetElementAttributeBoolValue( "observable", false );

        boost::shared_ptr< ParallelTwoPort< MatrixT > > tp = boost::make_shared< ParallelTwoPort< MatrixT > >( observable );

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );

        const size_t count = param->GetElementChild( "Children" )->GetElementAttributeUnsignedIntValue( "count", 1 );

        boost::shared_ptr< ArgumentTypeElectrical > argChildren;
        const ArgumentTypeElectrical *argChildrenRaw;

        if ( arg )
            argChildrenRaw = arg;
        else
        {
            argChildren = boost::make_shared< ArgumentTypeElectrical >();
            argChildrenRaw = argChildren.get();
        }

        for ( size_t i = 0; i < count; ++i )
        {
            for ( std::vector< boost::shared_ptr< xmlparser::XmlParameter > >::iterator it = children.begin();
                  it != children.end(); ++it )
            {
                boost::shared_ptr< TwoPort< MatrixT > > childtp(
                 this->GetElectricalFactory()->CreateInstance( it->get(), argChildrenRaw ) );
                if ( childtp != nullptr )
                    tp->AddChild( childtp );
            }
        }

        return tp;
    }
};

/// Classwrapper for electrical::SerialTwoPort
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, SerialTwoPort > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        validator::ValidateSerialTwoPortNode( param );
        const bool observable = param->GetElementAttributeBoolValue( "observable", false );

        boost::shared_ptr< SerialTwoPort< MatrixT > > tp( new SerialTwoPort< MatrixT >( observable ) );

        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );

        const size_t count = param->GetElementChild( "Children" )->GetElementAttributeUnsignedIntValue( "count", 1 );

        boost::shared_ptr< ArgumentTypeElectrical > argChildren;
        const ArgumentTypeElectrical *argChildrenRaw;

        if ( arg )
            argChildrenRaw = arg;
        else
        {
            argChildren = boost::make_shared< ArgumentTypeElectrical >();
            argChildrenRaw = argChildren.get();
        }

        for ( size_t i = 0; i < count; ++i )
            for ( std::vector< boost::shared_ptr< xmlparser::XmlParameter > >::iterator it = children.begin();
                  it != children.end(); ++it )
            {
                boost::shared_ptr< TwoPort< MatrixT > > childtp(
                 this->GetElectricalFactory()->CreateInstance( it->get(), argChildrenRaw ) );
                if ( childtp != nullptr )
                    tp->AddChild( childtp );
            }

        return tp;
    }
};

template < typename Matrix >
class None;

/// Classwrapper that produces no element. Used for BuildFakeElectricalFactory()
template < typename MatrixT >
class ElectricalClassWrapper< MatrixT, None > : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapper( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                            Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                            Factory< state::State, ArgumentTypeState > *stateFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        return nullptr;
    }
};

#ifdef BUILD_AGING_SIMULATION

/// Classwrapper that produces an instance of TConcrete using the ElectricalClassWrapper
/// and calls the aging twoport factory to produce an AgingTwoPort
template < typename MatrixT, template < typename > class TConcrete >
class ElectricalClassWrapperForAging : public ElectricalClassWrapperBase< MatrixT >
{
    public:
    ElectricalClassWrapperForAging( Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory,
                                    Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory,
                                    Factory< state::State, ArgumentTypeState > *stateFactory,
                                    Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort > *agingTwoPortFactory )
        : ElectricalClassWrapperBase< MatrixT >( electricalFactory, objectFactory, stateFactory )
        , mAgingTwoPortFactory( agingTwoPortFactory )
        , mWrapper( electricalFactory, objectFactory, stateFactory )
    {
    }

    boost::shared_ptr< TwoPort< MatrixT > >
    CreateInstance( const xmlparser::XmlParameter *param, const ArgumentTypeElectrical *arg = 0 )
    {
        boost::shared_ptr< TwoPort< MatrixT > > twoport( mWrapper.CreateInstance( param, arg ) );
        if ( twoport == nullptr )
            return twoport;
        ArgumentTypeAgingTwoPort argAgingTP;
        argAgingTP.mElectricalTwoPort = boost::dynamic_pointer_cast< electrical::TwoPortWithState< MatrixT > >( twoport );
        if ( std::is_same< TConcrete< MatrixT >, electrical::Cellelement< MatrixT > >::value )
            argAgingTP.mCellelement = boost::static_pointer_cast< electrical::Cellelement< MatrixT > >( twoport );
        else
        {
            auto &cell = arg->mParentCell;
            argAgingTP.mCellelement = cell;
        }
        const auto &agingTp = mAgingTwoPortFactory->CreateInstance( param, &argAgingTP );
        // copy age and charge throughput to all children
        for ( const auto &child : agingTp->GetChildren() )
        {
            *child->GetAgingState() = *agingTp->GetAgingState();
        }
        return twoport;
    }

    private:
    Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort > *const mAgingTwoPortFactory;
    ElectricalClassWrapper< MatrixT, TConcrete > mWrapper;
};

#endif

}    // End namespace factory

#endif