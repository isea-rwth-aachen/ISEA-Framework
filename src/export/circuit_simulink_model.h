#ifndef _ELECTRICAL_SIMULINK_MODEL_
#define _ELECTRICAL_SIMULINK_MODEL_

#ifdef _SYMBOLIC_

#include "../electrical/twoport.h"
#include "../electrical/twoport_withchild.h"
#include "../misc/StrCont.h"
#include "../misc/matrixInclude.h"
#include "../object/const_obj.h"
#include "../object/lookup_obj1d_with_state.h"
#include "../object/lookup_obj2d_with_state.h"
#include "../state/soc.h"
#include "mex.h"
#include <boost/array.hpp>
#include <stack>

#include "../electrical/capacity.h"
#include "../electrical/cellelement.h"
#include "../electrical/ohmicresistance.h"
#include "../electrical/parallelRCAlg.h"
#include "../electrical/parallelrc.h"
#include "../electrical/paralleltwoport.h"
#include "../electrical/serialtwoport.h"
#include "../electrical/voltagesource.h"
#include "../electrical/warburgCotanh.h"
#include "../electrical/warburgTanh.h"
#include "../electrical/zarc.h"
#include "../electrical/zarcalg.h"
#include "../exceptions/error_proto.h"

namespace visualizer
{

template < typename Matrix, typename T >
class ElectricalSimulinkModel
{
    enum IOType
    {
        IU,
        UI
    };
    enum ModelType
    {
        ALGEBRAIC,
        DERIVATIVE
    };
    struct SubSystem
    {
        SubSystem( const misc::StrCont &nameOfSubSystem, IOType iOType )
            : mNameOfSubSystem( nameOfSubSystem )
            , mIOType( iOType )
        {
        }
        misc::StrCont mNameOfSubSystem;
        IOType mIOType;
    };

    public:
    ElectricalSimulinkModel( const boost::shared_ptr< electrical::TwoPort< Matrix > > &rootTwoPort,
                             const char *filename = "ElectricalModel" );
    virtual ~ElectricalSimulinkModel(){};
    void CreateModelWithAlgebraicLoops();
    void CreateModelWithDerivatives();

    private:
    void EvaluateTwoPort( const electrical::TwoPort< Matrix > *twoPort, ModelType modelType );

    void PlaceCapacity( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceOhmicResistance( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceVoltageSource( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceParallelRC( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceParallelRCAlg( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceZarc( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceZarcAlg( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceTwoPortWithChild( const electrical::TwoPort< Matrix > *twoPort, ModelType modelType, const char *createFunction,
                                T *addArgCreateFunction, bool connectAccordinglyOrSerial, const char *additionalConnectFunction );

    void GetObjectData( const object::Object< T > *object, mxArray *&rowPoints, mxArray *&colPoints, mxArray *&dataMatrix ) const;
    inline misc::StrCont
    MakeDestination( const electrical::TwoPort< Matrix > *twoPort ) const;    // Should be restructured if Return Value
                                                                              // Optimization does not work
    inline bool IsIU() const;
    inline void SwitchIOType( IOType &iOType ) const;

    std::stack< SubSystem > mSubSystemStack;
    misc::StrCont mSimulinkModel;
    boost::shared_ptr< electrical::TwoPort< Matrix > > mRootTwoPort;
};

template < typename Matrix, typename T >
ElectricalSimulinkModel< Matrix, T >::ElectricalSimulinkModel( const boost::shared_ptr< electrical::TwoPort< Matrix > > &rootTwoPort,
                                                               const char *filename )
    : mSimulinkModel( filename )
    , mRootTwoPort( rootTwoPort )
{
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::CreateModelWithAlgebraicLoops()
{
    mxArray **plhs = 0;
    boost::array< mxArray *, 1 > prhs;
    prhs.at( 0 ) = mxCreateString( mSimulinkModel );
    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "InitializeModelCreation" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "CreateModelWithAlgebraicLoops", 1 );

    mSubSystemStack.push( SubSystem( mSimulinkModel, IU ) );
    EvaluateTwoPort( mRootTwoPort.get(), ALGEBRAIC );
    mSubSystemStack.pop();

    prhs.at( 0 ) = mxCreateString( mSimulinkModel + "/Thermal" );
    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "CreateNoThermalModel" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "CreateModelWithAlgebraicLoops", 2 );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::CreateModelWithDerivatives()
{
    mxArray **plhs = 0;
    boost::array< mxArray *, 1 > prhs;
    prhs.at( 0 ) = mxCreateString( mSimulinkModel );
    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "InitializeModelCreation" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "CreateModelWithDerivatives", 1 );

    mSubSystemStack.push( SubSystem( mSimulinkModel, IU ) );
    EvaluateTwoPort( mRootTwoPort.get(), DERIVATIVE );
    mSubSystemStack.pop();

    prhs.at( 0 ) = mxCreateString( mSimulinkModel + "/Thermal" );
    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "CreateNoThermalModel" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "CreateModelWithDerivatives", 2 );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::EvaluateTwoPort( const electrical::TwoPort< Matrix > *twoPort, ModelType modelType )
{
    if ( dynamic_cast< const electrical::WarburgTanh< Matrix > * >( twoPort ) )
        PlaceTwoPortWithChild( twoPort, modelType, "CreateWarburgTanh", 0, mSubSystemStack.top().mIOType == IU,
                               "ConnectCircuitWithStates" );
    else if ( dynamic_cast< const electrical::WarburgCotanh< Matrix > * >( twoPort ) )
        PlaceTwoPortWithChild( twoPort, modelType, "CreateWarburgCotanh", 0, mSubSystemStack.top().mIOType == IU,
                               "ConnectCircuitWithStates" );
    else if ( dynamic_cast< const electrical::Cellelement< Matrix > * >( twoPort ) )
    {
        T maxCapacityAndInitialSoC[2] = {0};
        maxCapacityAndInitialSoC[0] =
         static_cast< const electrical::Cellelement< Matrix > * >( twoPort )->GetSoc()->GetInitialCapacity();
        maxCapacityAndInitialSoC[1] = static_cast< const electrical::Cellelement< Matrix > * >( twoPort )->GetSoc()->GetValue();
        PlaceTwoPortWithChild( twoPort, modelType, "CreateCellelement", maxCapacityAndInitialSoC,
                               mSubSystemStack.top().mIOType == IU, "ConnectCellelement" );
    }
    else if ( dynamic_cast< const electrical::ParallelTwoPort< Matrix > * >( twoPort ) )
        PlaceTwoPortWithChild( twoPort, modelType, "CreateCircuit", 0, mSubSystemStack.top().mIOType == UI, 0 );
    else if ( dynamic_cast< const electrical::SerialTwoPort< Matrix > * >( twoPort ) )
        PlaceTwoPortWithChild( twoPort, modelType, "CreateCircuit", 0, mSubSystemStack.top().mIOType == IU, 0 );
    else if ( dynamic_cast< const electrical::Capacity< Matrix > * >( twoPort ) )
        PlaceCapacity( twoPort );
    else if ( dynamic_cast< const electrical::OhmicResistance< Matrix > * >( twoPort ) )
        PlaceOhmicResistance( twoPort );
    else if ( dynamic_cast< const electrical::VoltageSource< Matrix > * >( twoPort ) )
        PlaceVoltageSource( twoPort );
    else if ( dynamic_cast< const electrical::ParallelRC< Matrix > * >( twoPort ) )
        PlaceParallelRC( twoPort );
    else if ( dynamic_cast< const electrical::ParallelRCAlg< Matrix > * >( twoPort ) )
        PlaceParallelRCAlg( twoPort );
    else if ( dynamic_cast< const electrical::Zarc< Matrix > * >( twoPort ) )
        PlaceZarc( twoPort );
    else if ( dynamic_cast< const electrical::ZarcAlg< Matrix > * >( twoPort ) )
        PlaceZarcAlg( twoPort );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceCapacity( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 6 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateLogicalScalar( IsIU() );
    prhs.at( 2 ) = mxCreateDoubleScalar( 0.001 );

    const electrical::Capacity< Matrix > *capacity = static_cast< const electrical::Capacity< Matrix > * >( twoPort );
    GetObjectData( capacity->GetElementObject(), prhs.at( 3 ), prhs.at( 4 ), prhs.at( 5 ) );

    if ( mexCallMATLAB( 0, plhs, 6, prhs.data(), "CreateCapacity" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "Capacity" );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceOhmicResistance( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 5 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateLogicalScalar( IsIU() );

    const electrical::OhmicResistance< Matrix > *ohmicResistance =
     static_cast< const electrical::OhmicResistance< Matrix > * >( twoPort );
    GetObjectData( ohmicResistance->GetElementObject(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );

    if ( mexCallMATLAB( 0, plhs, 5, prhs.data(), "CreateOhmicResistance" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "OhmicResistance" );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceVoltageSource( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 5 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateLogicalScalar( IsIU() );

    const electrical::VoltageSource< Matrix > *voltageSource =
     static_cast< const electrical::VoltageSource< Matrix > * >( twoPort );
    GetObjectData( voltageSource->GetElementObject(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );

    if ( mexCallMATLAB( 0, plhs, 5, prhs.data(), "CreateVoltageSource" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "VoltageSource" );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceParallelRC( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 9 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateLogicalScalar( IsIU() );
    prhs.at( 2 ) = mxCreateDoubleScalar( 0.001 );

    const electrical::ParallelRC< Matrix > *parallelRC = static_cast< const electrical::ParallelRC< Matrix > * >( twoPort );
    GetObjectData( parallelRC->GetElementObject(), prhs.at( 3 ), prhs.at( 4 ), prhs.at( 5 ) );
    GetObjectData( parallelRC->GetTauObject(), prhs.at( 6 ), prhs.at( 7 ), prhs.at( 8 ) );

    if ( mexCallMATLAB( 0, plhs, 9, prhs.data(), "CreateParallelRC" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "ParallelRC" );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceParallelRCAlg( const electrical::TwoPort< Matrix > * )
{
    ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "PlaceParallelRCAlg" );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceZarc( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 11 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateLogicalScalar( IsIU() );

    const electrical::Zarc< Matrix > *zarc = static_cast< const electrical::Zarc< Matrix > * >( twoPort );
    GetObjectData( zarc->GetRObject(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );
    GetObjectData( zarc->GetTauObject(), prhs.at( 5 ), prhs.at( 6 ), prhs.at( 7 ) );
    GetObjectData( zarc->GetPhiObject(), prhs.at( 8 ), prhs.at( 9 ), prhs.at( 10 ) );

    if ( mexCallMATLAB( 0, plhs, 11, prhs.data(), "CreateZarc" ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "ParallelRC" );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceZarcAlg( const electrical::TwoPort< Matrix > * )
{
    ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "PlaceZarcAlg" );
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::PlaceTwoPortWithChild( const electrical::TwoPort< Matrix > *twoPort,
                                                                  ModelType modelType, const char *createFunction,
                                                                  T *addArgCellelement, bool connectAccordinglyOrSerial,
                                                                  const char *additionalConnectFunction )
{
    misc::StrCont destination = MakeDestination( twoPort );

    boost::array< mxArray *, 1 > plhs;
    boost::array< mxArray *, 4 > prhs;
    prhs.at( 0 ) = mxCreateString( destination );
    prhs.at( 1 ) = mxCreateLogicalScalar( IsIU() );
    if ( addArgCellelement )
    {
        prhs.at( 2 ) = mxCreateDoubleScalar( addArgCellelement[0] );
        prhs.at( 3 ) = mxCreateDoubleScalar( addArgCellelement[1] );
        if ( mexCallMATLAB( 1, plhs.data(), 4, prhs.data(), createFunction ) != 0 )
            ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                            "PlaceTwoPortWithChild 0_1" );
    }
    else
    {
        if ( mexCallMATLAB( 1, plhs.data(), 2, prhs.data(), createFunction ) != 0 )
            ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                            "PlaceTwoPortWithChild 0_2" );
    }

    const electrical::TwoPortWithChild< Matrix > *twoPortWithChild =
     static_cast< const electrical::TwoPortWithChild< Matrix > * >( twoPort );
    const char *connect = 0;
    std::vector< char > stringbuffer( mxGetN( plhs.at( 0 ) ) + 1 );
    mxGetString( plhs[0], &stringbuffer[0], stringbuffer.size() );
    destination = misc::StrCont( &stringbuffer[0] );

    if ( modelType == DERIVATIVE )
    {
        IOType newIOType = mSubSystemStack.top().mIOType;
        mSubSystemStack.push( SubSystem( destination, newIOType ) );

        if ( connectAccordinglyOrSerial )
        {
            for ( size_t i = 0; i < twoPortWithChild->size(); ++i )
                EvaluateTwoPort( twoPortWithChild->at( i ), modelType );
        }
        else    // connect diametrically
        {
            EvaluateTwoPort( twoPortWithChild->at( 0 ), modelType );
            SwitchIOType( mSubSystemStack.top().mIOType );
            for ( size_t i = 1; i < twoPortWithChild->size(); ++i )
                EvaluateTwoPort( twoPortWithChild->at( i ), modelType );
        }

        if ( connectAccordinglyOrSerial )
            connect = "ConnectAccordingly";
        else    // connect diametrically
            connect = "ConnectDiametrically";
    }
    else    // modelType == ALGEBRAIC
    {
        mSubSystemStack.push( SubSystem( destination, IU ) );

        for ( size_t i = 0; i < twoPortWithChild->size(); ++i )
            EvaluateTwoPort( twoPortWithChild->at( i ), modelType );

        if ( connectAccordinglyOrSerial )    // serial
            connect = "ConnectAccordingly";
        else    // parallel
            connect = "ConnectAlgebraic";
    }
    /*
    if(modelType == DERIVATIVE)
    {
        IOType newIOType = mSubSystemStack.top().mIOType;
        mSubSystemStack.push( SubSystem(destination, newIOType) );

        if(connectAccordinglyOrSerial)
        {
            if(twoPortWithChild->size() == 1)
                EvaluateTwoPort(twoPortWithChild->at(0), modelType);
            else
            {
                EvaluateTwoPort(twoPortWithChild->at(1), modelType);
                EvaluateTwoPort(twoPortWithChild->at(0), modelType);
            }

            for( size_t i = 2 ; i < twoPortWithChild->size() ; ++i )
                EvaluateTwoPort(twoPortWithChild->at(i), modelType);
        }
        else //connect diametrically
        {
            if(twoPortWithChild->size() == 1)
                EvaluateTwoPort(twoPortWithChild->at(0), modelType);
            else
            {
                EvaluateTwoPort(twoPortWithChild->at(1), modelType);
                SwitchIOType(mSubSystemStack.top().mIOType);
                EvaluateTwoPort(twoPortWithChild->at(0), modelType);
            }

            for( size_t i = 2 ; i < twoPortWithChild->size() ; ++i )
                EvaluateTwoPort(twoPortWithChild->at(i), modelType);
        }

        if(connectAccordinglyOrSerial)
            connect = "ConnectAccordingly";
        else //connect diametrically
            connect = "ConnectDiametrically";
    }
    else //modelType == ALGEBRAIC
    {
        mSubSystemStack.push( SubSystem(destination, IU) );

        if(twoPortWithChild->size() == 1)
            EvaluateTwoPort(twoPortWithChild->at(0), modelType);
        else
        {
            EvaluateTwoPort(twoPortWithChild->at(1), modelType);
            EvaluateTwoPort(twoPortWithChild->at(0), modelType);
        }

        for( size_t i = 2 ; i < twoPortWithChild->size() ; ++i )
            EvaluateTwoPort(twoPortWithChild->at(i), modelType);

        if(connectAccordinglyOrSerial) //serial
            connect = "ConnectAccordingly";
        else //parallel
            connect = "ConnectAlgebraic";
    }*/

    prhs.at( 0 ) = mxCreateString( destination );
    if ( additionalConnectFunction )
        if ( mexCallMATLAB( 0, plhs.data(), 1, prhs.data(), additionalConnectFunction ) != 0 )
            ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                            "PlaceTwoPortWithChild 2" );

    if ( mexCallMATLAB( 0, plhs.data(), 1, prhs.data(), connect ) != 0 )
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "PlaceTwoPortWithChild 3" );

    mSubSystemStack.pop();
}

template < typename Matrix, typename T >
void ElectricalSimulinkModel< Matrix, T >::GetObjectData( const object::Object< T > *object, mxArray *&rowPoints,
                                                          mxArray *&colPoints, mxArray *&dataMatrix ) const
{
    if ( dynamic_cast< const object::LookupObj2dWithState< T > * >( object ) )
    {
        const object::LookupObj2dWithState< T > *lookupObj2dWithState =
         static_cast< const object::LookupObj2dWithState< T > * >( object );
        const lookup::LookupType2D< T > *loopUpType = lookupObj2dWithState->GetLookup().GetLookupType().get();

        if ( dynamic_cast< const state::Soc * >( lookupObj2dWithState->GetStateRow() ) )
        {
            const size_t numberOfRows = loopUpType->GetPointsCol().size();
            rowPoints = mxCreateDoubleMatrix( 1, numberOfRows, mxREAL );
            for ( size_t i = 0; i < numberOfRows; ++i )
                mxGetPr( rowPoints )[i] = loopUpType->GetPointsCol().at( i );

            const size_t numberOfCols = loopUpType->GetPointsRow().size();
            colPoints = mxCreateDoubleMatrix( 1, numberOfCols, mxREAL );
            for ( size_t i = 0; i < numberOfCols; ++i )
                mxGetPr( colPoints )[i] = loopUpType->GetPointsRow().at( i );

            dataMatrix = mxCreateDoubleMatrix( numberOfRows, numberOfCols, mxREAL );
            // Matlab organizes its matrices in coloumn-major order
            for ( size_t j = 0; j < numberOfCols; ++j )
                for ( size_t i = 0; i < numberOfRows; ++i )
                    mxGetPr( dataMatrix )[i + j * numberOfRows] = loopUpType->GetLookup().at( i ).at( j );
        }
        else
        {
            const size_t numberOfRows = loopUpType->GetPointsRow().size();
            rowPoints = mxCreateDoubleMatrix( 1, numberOfRows, mxREAL );
            for ( size_t i = 0; i < numberOfRows; ++i )
                mxGetPr( rowPoints )[i] = loopUpType->GetPointsRow().at( i );

            const size_t numberOfCols = loopUpType->GetPointsCol().size();
            colPoints = mxCreateDoubleMatrix( 1, numberOfCols, mxREAL );
            for ( size_t i = 0; i < numberOfCols; ++i )
                mxGetPr( colPoints )[i] = loopUpType->GetPointsCol().at( i );

            dataMatrix = mxCreateDoubleMatrix( numberOfRows, numberOfCols, mxREAL );
            // Matlab organizes its matrices in coloumn-major order
            for ( size_t j = 0; j < numberOfCols; ++j )
                for ( size_t i = 0; i < numberOfRows; ++i )
                    mxGetPr( dataMatrix )[i + j * numberOfRows] = loopUpType->GetLookup().at( j ).at( i );
        }
    }
    else if ( dynamic_cast< const object::LookupObj1dWithState< T > * >( object ) )
    {
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorObject", 1 );
        const lookup::LookupType1D< T > *loopUpType =
         static_cast< const object::LookupObj1dWithState< T > * >( object )->GetLookup().GetLookupType().get();

        const size_t numberOfPoints = loopUpType->GetPoints().size();
        rowPoints = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
        for ( size_t i = 0; i < numberOfPoints; ++i )
            mxGetPr( rowPoints )[i] = loopUpType->GetPoints().at( i );

        dataMatrix = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
        for ( size_t i = 0; i < numberOfPoints; ++i )
            mxGetPr( dataMatrix )[i] = loopUpType->GetLookup().at( i );
    }
    else if ( dynamic_cast< const object::ConstObj< T > * >( object ) )
    {
        rowPoints = mxCreateDoubleScalar( 1.0 );
        colPoints = mxCreateDoubleScalar( 1.0 );
        dataMatrix = mxCreateDoubleScalar( object->GetValue() );
    }
    else
        ErrorFunction< misc::StrCont >( __FUNCTION__, __LINE__, __FILE__, "ErrorObject", 2 );
}

template < typename Matrix, typename T >
misc::StrCont ElectricalSimulinkModel< Matrix, T >::MakeDestination( const electrical::TwoPort< Matrix > *twoPort ) const
{
    return misc::StrCont( mSubSystemStack.top().mNameOfSubSystem ) + "/" + twoPort->GetName();
}

template < typename Matrix, typename T >
bool ElectricalSimulinkModel< Matrix, T >::IsIU() const
{
    return mSubSystemStack.top().mIOType == IU;
}

template < typename Matrix, typename T >
inline void ElectricalSimulinkModel< Matrix, T >::SwitchIOType( IOType &iOType ) const
{
    iOType = ( ( iOType == IU ) ? UI : IU );
}

}    // namespace visualizer

#endif    // _SYMBOLIC_
#endif    // _ELECTRICAL_SIMULINK_MODEL_
