/*
 * objectclasswrapper.h
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#ifndef OBJECTCLASSWRAPPER_H_
#define OBJECTCLASSWRAPPER_H_

// BOOST
#include <boost/make_shared.hpp>
#include <boost/static_assert.hpp>

// STD
#include <cstring>
#include <stdexcept>

// ETC
#include "../check_positive_int.h"
#include "../classwrapper.h"
#include "../factory.h"

#include "../../electrical/electrical_data_struct.h"

#include "../../state/soc.h"
#include "../../state/thermal_state.h"
#include "../../state/crate.h"
#include "../../state/cdirection.h"
#include "../../state/lhd.h"

#include "../../object/const_obj.h"
#include "../../object/expression_obj.h"
#include "../../object/function_obj1d.h"
#include "../../object/lookup_obj1d.h"
#include "../../object/lookup_obj1d_with_state.h"
#include "../../object/lookup_obj2d.h"
#include "../../object/lookup_obj2d_with_state.h"
#include "../../object/lookup_obj3d.h"
#include "../../object/lookup_obj3d_with_state.h"
#include "../../object/multi_obj.h"


#include "../../exceptions/error_proto.h"
#include "../../operators/vectorOperator.h"

#include "../../misc/cmp_str.h"

#include "../state/stateclasswrapper.h"

using namespace object;

namespace factory
{

template < typename ValueT >
struct ArgumentTypeObject
{
    ArgumentTypeObject( ValueT objectFactor = 1.0 )
        : mObjectFactor( objectFactor )
        , mSoc( nullptr )
        , mCrate( nullptr)
        , mCdirection( nullptr)
        , mSetReverseLookUp( false ){};

    std::map< misc::StrCont, double, misc::cmp_str > mDoubleMap;
    ValueT mObjectFactor = 1.0;
    boost::shared_ptr< ElectricalDataStruct< ScalarUnit > > mElectricalDataStruct =
     boost::shared_ptr< ElectricalDataStruct< ScalarUnit > >( new ElectricalDataStruct< ScalarUnit > );

    boost::shared_ptr< state::State > mSoc;
    boost::shared_ptr< state::State > mCrate;
    boost::shared_ptr< state::State > mCdirection;
    bool mSetReverseLookUp;
};

// DIRTY DIRTY HACK!
const double UNDEFINED_BEHAVIOR = -1;
const double SCALA_MULUTPLICATION = 0;
const double OBJECT_MULTIPLICATION = 1;

/// Classwrapper base class for being used in a generic ::object::Object Factory. This class has to be derived.
template < typename ValueT >
class ObjectClassWrapperBase : public ClassWrapperBase< Object< ValueT >, ArgumentTypeObject< ValueT > >
{
    // This is needed for HACK_1
    BOOST_STATIC_ASSERT( sizeof( double ) >= sizeof( double* ) );    // This only works if pointers are smaller than doubles
    public:
    /**
     * Classwrapper constructor.
     * The factory is supplied via parameter and is used for other instances ctor parameters.
     * @param stateFactory A Factory for class in the state namespace
     */
    ObjectClassWrapperBase( Factory< state::State, ArgumentTypeState >* stateFactory,
                            Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : mStateFactory( stateFactory )
        , mObjectFactory( objectFactory )
    {
    }

    protected:
    /// Get a state::State Factory.
    Factory< state::State, ArgumentTypeState >* GetStateFactory() { return mStateFactory; }
    Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* GetObjectFactory() { return mObjectFactory; }

    void ConvertDouble2ObjectPointer( double address, Object< ValueT >* obj )
    {
        memcpy( static_cast< void* >( &obj ), static_cast< void* >( &address ), sizeof( double* ) );
    }

    double GetOperand( const ArgumentTypeObject< ValueT >* arg ) const
    {
        const char* mapEntry = "Operand";
        DoubleMapConstIterator it = arg->mDoubleMap.find( mapEntry );
        if ( it == arg->mDoubleMap.end() )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "MultiplierNotFound" );

        return it->second;
    }

    double GetOperator( const ArgumentTypeObject< ValueT >* arg ) const
    {
        const char* mapEntry = "Operator";

        DoubleMapConstIterator it = arg->mDoubleMap.find( mapEntry );
        if ( it == arg->mDoubleMap.end() )
            return UNDEFINED_BEHAVIOR;

        return it->second;
    }

    double GetMultiplier( const ArgumentTypeObject< ValueT >* arg ) const
    {
        const char* mapEntry = "Multiplier";
        DoubleMapConstIterator it = arg->mDoubleMap.find( mapEntry );
        if ( it == arg->mDoubleMap.end() )
        {
            return 1.0;
        }
        return it->second;
    }

    void CheckStateIsCacherefed( const xmlparser::XmlParameter* param ) const
    {
        if ( !param->HasElementAttribute( "cacheref" ) )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "CacherefAttribute",
                                                 param->GetElementName(), param->GetLineNumber() );
        }
    }

    /** Get the LookupType from xml file, e.g. LinearInterpolation
     * @param    param Pointer of type xmlparser::XmlParameter. This is the desired LookupType in the xml-file.
     * @return   Type of the lookup, defaults to LinearInterpolation
     */
    lookup::LookupType GetLookupType( const xmlparser::XmlParameter* param )
    {
        if ( !param )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "InvalidXML" );
        }
        else
        {
            // Get the user defined LookupType
            if ( param->HasElementAttribute( "LookupType" ) )
            {
                const char* myLookupTypeString = param->GetElementAttributeCharPtrValue( "LookupType" );
                misc::equal_str myCompareObject;
                if ( myCompareObject( myLookupTypeString, "LinearInterpolation" ) )
                    return lookup::LookupType::LINEAR_INTERPOLATION;
                else if ( myCompareObject( myLookupTypeString, "NearestNeighbour" ) )
                    return lookup::LookupType::NEAREST_NEIGHBOUR;
                else if ( myCompareObject( myLookupTypeString, "ConstantUntilThreshold" ) )
                    return lookup::LookupType::CONSTANT_UNTIL_THRESHOLD;
                else if ( myCompareObject( myLookupTypeString, "SplineInterpolation" ) )
                    return lookup::LookupType::SPLINE_INTERPOLATION;
                else
                {
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnknownLookupType" );
                }
            }
        }
        return lookup::LookupType::LINEAR_INTERPOLATION;    // Set LookupType to default LinearInterpolation
    }

    private:
    Factory< state::State, ArgumentTypeState >* const mStateFactory;
    Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* const mObjectFactory;
};

/// Classwrapper for ::object namespace. This template class has to be specialized in order to create an instance of a
/// particular class.
template < typename ValueT, template < typename > class TConcrete >
class ObjectClassWrapper : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }
};

/// Classwrapper for object::ConstObj
template < typename ValueT >
class ObjectClassWrapper< ValueT, ConstObj > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        ValueT value = param->GetElementDoubleValue( "Value" );

        if ( arg != 0 )
        {
            value *= this->GetMultiplier( arg );

            if ( arg->mObjectFactor != 1.0 )
                value *= arg->mObjectFactor;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {
                Object< ValueT >* cObj = NULL;

                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::make_shared< ConstObj< ValueT > >( value, static_cast< ConstObj< ValueT >* >( cObj ) );
            }
        }

        return boost::make_shared< ConstObj< ValueT > >( value );
    }
};

/// Classwrapper for object::MultiObj
template < typename ValueT >
class ObjectClassWrapper< ValueT, MultiObj > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        if ( arg != nullptr && this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnsupportedOperation" );
        }
        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > children = param->GetElementChildren( "Children" );
        if ( children.empty() )
        {
            ErrorFunction< factory::NoChildrenException >( __FUNCTION__, __LINE__, __FILE__, "NoChildrenException",
                                                           "MultiObj" );
        }

        std::vector< boost::shared_ptr< Object< ValueT > > > tmpVec;
        for ( std::vector< boost::shared_ptr< xmlparser::XmlParameter > >::iterator it = children.begin();
              it != children.end(); ++it )
        {
            tmpVec.push_back( this->GetObjectFactory()->CreateInstance( it->get() ) );
        }

        if ( !param->HasElementAttribute( "Operation" ) )
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnsupportedOperation" );
        }

        const char* operationType = param->GetElementAttributeCharPtrValue( "Operation" );
        misc::equal_str operationTypeCompareObject;
        boost::shared_ptr< Object< ValueT > > returnElement;

        if ( operationTypeCompareObject( operationType, "Add" ) )
            returnElement = boost::make_shared< object::AdderObj< ValueT > >( tmpVec );
        else if ( operationTypeCompareObject( operationType, "Multiply" ) )
            returnElement = boost::make_shared< object::MultiplyObj< ValueT > >( tmpVec );
        else if ( operationTypeCompareObject( operationType, "Divide" ) )
            returnElement = boost::make_shared< object::DivideObj< ValueT > >( tmpVec );
        else
        {
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "UnsupportedOperation" );
        }

        if ( arg != 0 && arg->mObjectFactor != 1.0 )
        {
            std::vector< boost::shared_ptr< Object< ValueT > > > productVec{returnElement, CreateProduct( arg )};
            returnElement = boost::make_shared< object::MultiplyObj< ValueT > >( productVec );
        }


        return returnElement;
    }

    /// This function shall only be called with arg != 0
    boost::shared_ptr< Object< ValueT > > CreateProduct( const ArgumentTypeObject< ValueT >* arg )
    {
        if ( arg != 0 && arg->mObjectFactor != 1.0 )
        {
            return boost::make_shared< ConstObj< ValueT > >( arg->mObjectFactor );
        }

        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ArgumentIsZero" );
        return boost::make_shared< ConstObj< ValueT > >( 1.0 );
    }
};


/// Classwrapper for object::LookupObj1D
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj1D > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        std::vector< ValueT > lookupData = param->GetElementDoubleVecValue( "LookupData" );
        std::vector< ValueT > measurementPoints = param->GetElementDoubleVecValue( "MeasurementPoints" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {

                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::make_shared< LookupObj1D< ValueT > >( lookupData, measurementPoints,
                                                                    static_cast< LookupObj1D< ValueT >* >( cObj ),
                                                                    lookup::LookupType( this->GetLookupType( param ) ) );
            }
        }
        return boost::make_shared< LookupObj1D< ValueT > >( lookupData, measurementPoints,
                                                            lookup::LookupType( this->GetLookupType( param ) ) );
    }
};

/// Classwrapper for object::LookupObj1dWithState
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj1dWithState > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }


    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        /* TODO cacheref entfernen?
        this->CheckStateIsCacherefed( param->GetElementChild( "State" ).get() );
        */

        ArgumentTypeState stateArg;

        boost::shared_ptr< state::State > state;

        std::vector< ValueT > lookupData = param->GetElementDoubleVecValue( "LookupData" );
        std::vector< ValueT > measurementPoints = param->GetElementDoubleVecValue( "MeasurementPoints" );

        if ( arg )
        {
            stateArg.mElectricalDataStruct = arg->mElectricalDataStruct;
            state = arg->mSoc ? arg->mSoc : this->GetStateFactory()->CreateInstance( param->GetElementChild( "State" ), &stateArg );

            if ( arg->mSetReverseLookUp )
                std::swap( lookupData, measurementPoints );
        }

        else
            state = this->GetStateFactory()->CreateInstance( param->GetElementChild( "State" ), &stateArg );


        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {
                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::make_shared< LookupObj1dWithState< ValueT > >( lookupData, measurementPoints, state,
                                                                             static_cast< LookupObj1dWithState< ValueT >* >( cObj ),
                                                                             lookup::LookupType( this->GetLookupType( param ) ) );
            }
        }
        return boost::make_shared< LookupObj1dWithState< ValueT > >( lookupData, measurementPoints, state,
                                                                     lookup::LookupType( this->GetLookupType( param ) ) );
    }
};

/// Classwrapper for object::LookupObj2D
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj2D > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }


    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        std::vector< std::vector< ValueT > > lookupData = param->GetElementDoubleVecVecValue( "LookupData" );
        std::vector< ValueT > measurementPointsRow = param->GetElementDoubleVecValue( "MeasurementPointsRow" );
        std::vector< ValueT > measurementPointsColumn = param->GetElementDoubleVecValue( "MeasurementPointsColumn" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {
                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::make_shared< LookupObj2D< ValueT > >( lookupData, measurementPointsRow, measurementPointsColumn,
                                                                    static_cast< LookupObj2D< ValueT >* >( cObj ),
                                                                    lookup::LookupType( this->GetLookupType( param ) ) );
            }
        }

        return boost::make_shared< LookupObj2D< ValueT > >( lookupData, measurementPointsRow, measurementPointsColumn,
                                                            lookup::LookupType( this->GetLookupType( param ) ) );
    }
};

/// Classwrapper for object::LookupObj2dWithState
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj2dWithState > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        boost::shared_ptr< state::State > soc;

        ArgumentTypeState stateArg;
        if ( arg )
        {
            stateArg.mElectricalDataStruct = arg->mElectricalDataStruct;

            if ( arg->mSoc )
                soc = arg->mSoc;
        }

        /* TODO cacheref entfernen?
        this->CheckStateIsCacherefed( param->GetElementChild( "RowState" ).get() );
        this->CheckStateIsCacherefed( param->GetElementChild( "ColState" ).get() );
        */

        const auto rowStateNode = param->GetElementChild( "RowState" );

        boost::shared_ptr< state::State > rowstate;

        if ( rowStateNode->HasElementAttribute( "cacheref", "Soc" ) && soc )
            rowstate = soc;

        else
            rowstate = this->GetStateFactory()->CreateInstance( param->GetElementChild( "RowState" ), &stateArg );

        const auto colStateNode = param->GetElementChild( "ColState" );

        boost::shared_ptr< state::State > colstate;

        if ( colStateNode->HasElementAttribute( "cacheref", "Soc" ) && soc )
            colstate = soc;

        else
            colstate = this->GetStateFactory()->CreateInstance( param->GetElementChild( "ColState" ), &stateArg );

        std::vector< std::vector< ValueT > > lookupData = param->GetElementDoubleVecVecValue( "LookupData" );
        std::vector< ValueT > measurementPointsRow = param->GetElementDoubleVecValue( "MeasurementPointsRow" );
        std::vector< ValueT > measurementPointsColumn = param->GetElementDoubleVecValue( "MeasurementPointsColumn" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {

                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );

                return boost::make_shared< LookupObj2dWithState< ValueT > >( lookupData, measurementPointsRow,
                                                                             measurementPointsColumn, rowstate, colstate,
                                                                             static_cast< LookupObj2dWithState< ValueT >* >( cObj ),
                                                                             lookup::LookupType( this->GetLookupType( param ) ) );
            }
        }

        return boost::make_shared< LookupObj2dWithState< ValueT > >( lookupData, measurementPointsRow,
                                                                     measurementPointsColumn, rowstate, colstate,
                                                                     lookup::LookupType( this->GetLookupType( param ) ) );
    }
};

/// Classwrapper for object::LookupObj3D
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj3D > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }


    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        std::vector< std::vector< ValueT > > lookupData = param->GetElementDoubleVecVecValue( "LookupData" );
        std::vector< ValueT > measurementPointsRow = param->GetElementDoubleVecValue( "MeasurementPointsRow" );
        std::vector< ValueT > measurementPointsColumn = param->GetElementDoubleVecValue( "MeasurementPointsColumn" );
        std::vector< ValueT > measurementPointsSlice = param->GetElementDoubleVecValue( "MeasurementPointsSlice" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {
                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );
                return boost::make_shared< LookupObj3D< ValueT > >( lookupData, measurementPointsRow,
                                                                    measurementPointsColumn, measurementPointsSlice,
                                                                    static_cast< LookupObj3D< ValueT >* >( cObj ),
                                                                    lookup::LookupType( this->GetLookupType( param ) ) );
            }
        }

        return boost::make_shared< LookupObj3D< ValueT > >( lookupData, measurementPointsRow, measurementPointsColumn,
                                                            measurementPointsSlice,
                                                            lookup::LookupType( this->GetLookupType( param ) ) );
    }
};

/// Classwrapper for object::LookupObj3dWithState
template < typename ValueT >
class ObjectClassWrapper< ValueT, LookupObj3dWithState > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        boost::shared_ptr< state::State > soc;
        boost::shared_ptr< state::State > crate;
        boost::shared_ptr< state::State > cdirection;
        ArgumentTypeState stateArg;

        if ( arg )
        {
            stateArg.mElectricalDataStruct = arg->mElectricalDataStruct;

            if ( arg->mSoc )
            {
                soc = arg->mSoc;
            }


            if ( arg->mCrate )
            {
                crate = arg->mCrate;
            }

            if ( arg->mCdirection )
            {
                cdirection = arg->mCdirection;
            }
        }

        const auto rowStateNode = param->GetElementChild( "RowState" );

        boost::shared_ptr< state::State > rowstate;
        if ( rowStateNode->HasElementAttribute( "cacheref", "Soc" ) && soc )
        {
            rowstate = soc;
        }
        else if( rowStateNode->HasElementAttribute( "cacheref", "CRateState" ) && crate ) 
        { 
            rowstate = crate; 
        }
        else if( rowStateNode->HasElementAttribute( "cacheref", "CDirection" ) && cdirection ) 
        { 
            rowstate = cdirection; 
        }
        else
            rowstate = this->GetStateFactory()->CreateInstance( param->GetElementChild( "RowState" ), &stateArg );

        // Column
        const auto colStateNode = param->GetElementChild( "ColState" );

        boost::shared_ptr< state::State > colstate;

        if ( colStateNode->HasElementAttribute( "cacheref", "Soc" ) && soc )
        {
            colstate = soc;
        }
        else if ( colStateNode->HasElementAttribute( "cacheref", "CRateState" ) && crate )
        {
            colstate = crate;
        }
        else if ( colStateNode->HasElementAttribute( "cacheref", "CDirection" ) && cdirection )
        {
            colstate = cdirection;
        }
        else
            colstate = this->GetStateFactory()->CreateInstance( param->GetElementChild( "ColState" ), &stateArg );

        // Slice
        const auto sliStateNode = param->GetElementChild( "SliState" );

        boost::shared_ptr< state::State > slistate;

        if ( sliStateNode->HasElementAttribute( "cacheref", "CRateState" ) && crate )
        {
            slistate = crate;
        }
        else if ( sliStateNode->HasElementAttribute( "cacheref", "CRateState" ) && crate )
        {
            slistate = crate;
        }
        else if ( sliStateNode->HasElementAttribute( "cacheref", "CDirection" ) && cdirection )
        {
            slistate = cdirection;
        }
        else
        {
            slistate = this->GetStateFactory()->CreateInstance( param->GetElementChild( "SliState" ), &stateArg );
        }

        // Virtual
        boost::shared_ptr< state::State > virstate;

        if ( param->HasElementDirectChild( "VirtualState" ) )
        {
            const auto virStateNode = param->GetElementChild( "VirtualState" );
            if ( sliStateNode->HasElementAttribute( "cacheref", "Soc" ) && soc )
            {
                virstate = soc;
            }
            else if ( virStateNode->HasElementAttribute( "cacheref", "CRateState" ) && crate )
            {
                virstate = crate;
            }
            else if ( virStateNode->HasElementAttribute( "cacheref", "CDirection" ) && cdirection )
            {
                virstate = cdirection;
            }
            else
            {
                virstate = this->GetStateFactory()->CreateInstance( param->GetElementChild( "VirtualState" ), &stateArg );
            }
        }

        std::vector< std::vector< ValueT > > lookupData = param->GetElementDoubleVecVecValue( "LookupData" );
        std::vector< ValueT > measurementPointsRow = param->GetElementDoubleVecValue( "MeasurementPointsRow" );
        std::vector< ValueT > measurementPointsColumn = param->GetElementDoubleVecValue( "MeasurementPointsColumn" );
        std::vector< ValueT > measurementPointsSlice = param->GetElementDoubleVecValue( "MeasurementPointsSlice" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            lookupData = lookupData * multiplier;

            if ( this->GetOperator( arg ) == factory::OBJECT_MULTIPLICATION )
            {

                Object< ValueT >* cObj = NULL;
                DoubleMapConstIterator it = arg->mDoubleMap.find( "Operand" );
                double add = it->second;
                memcpy( static_cast< void* >( &cObj ), static_cast< void* >( &add ), sizeof( double* ) );

                if ( !param->HasElementDirectChild( "VirtualState" ) )
                {
                    return boost::make_shared< LookupObj3dWithState< ValueT > >(
                     lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice, rowstate,
                     colstate, slistate, static_cast< LookupObj3dWithState< ValueT >* >( cObj ),
                     lookup::LookupType( this->GetLookupType( param ) ) );
                }
                else
                {
                    return boost::make_shared< LookupObj3dWithState< ValueT > >(
                     lookupData, measurementPointsRow, measurementPointsColumn, measurementPointsSlice, rowstate,
                     colstate, slistate, virstate, static_cast< LookupObj3dWithState< ValueT >* >( cObj ),
                     lookup::LookupType( this->GetLookupType( param ) ) );
                }
            }
        }

        if ( !param->HasElementDirectChild( "VirtualState" ) )
        {
            return boost::make_shared< LookupObj3dWithState< ValueT > >( lookupData, measurementPointsRow, measurementPointsColumn,
                                                                         measurementPointsSlice, rowstate, colstate, slistate,
                                                                         lookup::LookupType( this->GetLookupType( param ) ) );
        }
        else
        {
            return boost::make_shared< LookupObj3dWithState< ValueT > >( lookupData, measurementPointsRow,
                                                                         measurementPointsColumn, measurementPointsSlice,
                                                                         rowstate, colstate, slistate, virstate,
                                                                         lookup::LookupType( this->GetLookupType( param ) ) );
        }
    }
};

/// Classwrapper for object::ExpressionObject
template < typename ValueT >
class ObjectClassWrapper< ValueT, ExpressionObject > : public ObjectClassWrapperBase< ValueT >
{
    public:
    ObjectClassWrapper( Factory< state::State, ArgumentTypeState >* stateFactory,
                        Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory )
        : ObjectClassWrapperBase< ValueT >( stateFactory, objectFactory )
    {
    }

    typename ExpressionObject< ValueT >::RescaleRange GetRange( const boost::shared_ptr< xmlparser::XmlParameter > param )
    {
        double inputMin = param->GetElementDoubleValue( "InputRangeMinimum" );
        double inputMax = param->GetElementDoubleValue( "InputRangeMaximum" );
        double outputMin = param->GetElementDoubleValue( "OutputRangeMinimum" );
        double outputMax = param->GetElementDoubleValue( "OutputRangeMaximum" );
        return typename ExpressionObject< ValueT >::RescaleRange( {inputMin, inputMax}, {outputMin, outputMax} );
    }

    boost::shared_ptr< Object< ValueT > >
    CreateInstance( const xmlparser::XmlParameter* param, const ArgumentTypeObject< ValueT >* arg = 0 )
    {
        std::string expressionString = param->GetElementStringValue( "Expression" );

        if ( arg != 0 )
        {
            double multiplier = this->GetMultiplier( arg );
            if ( arg->mObjectFactor != 1.0 )
                multiplier *= arg->mObjectFactor;
            if ( multiplier != 1.0 )
                expressionString = std::to_string( multiplier ) + " * (" + expressionString + ")";
        }

        std::vector< typename ExpressionObject< ValueT >::Parameter > parameters;
        for ( const auto& element : param->GetElementChildren( "Parameters" ) )
        {
            const boost::shared_ptr< state::State >& state = this->GetStateFactory()->CreateInstance( element );
            std::string name;
            if ( element->HasElementAttribute( "cacheref" ) && !element->HasElementAttribute( "name" ) )
                name = element->GetElementAttributeCharPtrValue( "cacheref" );
            else
                name = element->GetElementAttributeCharPtrValue( "name" );
            if ( element->HasElementDirectChild( "Rescale" ) )
                parameters.push_back( {name, state, GetRange( element->GetElementChild( "Rescale" ) )} );
            else
                parameters.push_back( {name, state} );
        }

        if ( param->HasElementDirectChild( "Rescale" ) )
            return boost::make_shared< ExpressionObject< ValueT > >( expressionString, parameters,
                                                                     GetRange( param->GetElementChild( "Rescale" ) ) );
        else
            return boost::make_shared< ExpressionObject< ValueT > >( expressionString, parameters );
    }
};

} /* namespace factory */
#endif /* OBJECTCLASSWRAPPER_H_ */
