#ifndef _MATRIX_SIMULINK_MODEL_
#define _MATRIX_SIMULINK_MODEL_

#ifdef _SYMBOLIC_

#include "mex.h"
#include <boost/array.hpp>
#include <stack>

#include "../misc/StrCont.h"
#include "../misc/matrixInclude.h"
#include "../object/const_obj.h"
#include "../object/lookup_obj1d_with_state.h"
#include "../object/lookup_obj2d_with_state.h"
#include "../state/soc.h"

#include "../electrical/capacity.h"
#include "../electrical/cellelement.h"
#include "../electrical/electrical_simulation.h"
#include "../electrical/ohmicresistance.h"
#include "../electrical/parallelRCAlg.h"
#include "../electrical/parallelrc.h"
#include "../electrical/paralleltwoport.h"
#include "../electrical/serialtwoport.h"
#include "../electrical/twoport.h"
#include "../electrical/twoport_withchild.h"
#include "../electrical/voltagesource.h"
#include "../electrical/warburgCotanh.h"
#include "../electrical/warburgTanh.h"
#include "../electrical/zarc.h"
#include "../electrical/zarcalg.h"
#include "../system/dae_sys.h"
#include "../thermal/thermal_simulation.h"

#include "../exceptions/error_function.h"

namespace visualizer
{

template < typename Matrix, typename T >
class MatrixSimulinkModel
{
    struct CoolingDataForMex
    {
        CoolingDataForMex( const size_t &index, const thermal::TaylorData< T > &taylorData )
            : mIndex( index )
            , mTaylorData( taylorData )
        {
        }
        size_t mIndex;
        thermal::TaylorData< T > mTaylorData;
    };

    public:
    MatrixSimulinkModel( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, const char *filename,
                         const char *initialStateStruct );

    virtual ~MatrixSimulinkModel(){};

    void CreateMatrixModel();
    void LoadInitialState();
    void LoadObjectValues();

    private:
    void CreateElectricalModel();

    void CreateThermalModel();
    void CreateCoolings();
    void CreateThermalProbes();

    void PlaceTwoPortsAndObjects( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceElectricalElement( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceElectricalElementWithChildren( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceParallelRC( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceZarc( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceRMPHN( const electrical::TwoPort< Matrix > *twoPort );
    void PlaceSphericalDiffusion( const electrical::TwoPort< Matrix > *twoPort );

    void ObjectDataToDataVector( const object::Object< T > *object, mxArray *dataVector,
                                 const misc::StrCont &destination, size_t &index );
    void LookupDataToDataVector( const lookup::Lookup1D< T > &lookup1D, mxArray *dataVector,
                                 const misc::StrCont &destination, size_t &index );
    void GetObjectValues( const electrical::TwoPort< Matrix > *twoPort, mxArray *dataVector,
                          const misc::StrCont &destination, size_t &index );

    void GetObjectData( const object::Object< T > *object, mxArray *&rowPoints, mxArray *&colPoints, mxArray *&dataMatrix ) const;
    void GetLookup1DData( const lookup::Lookup1D< T > &lookup1D, mxArray *&points, mxArray *&dataMatrix ) const;
    inline misc::StrCont
    MakeDestination( const electrical::TwoPort< Matrix > *twoPort ) const;    // Should be restructured if Return Value
                                                                              // Optimization does not work

    void MakeMatrix( const Matrix &matrix, const char *destination, bool StateMatrix = false );
    void MakeStateResetMatrix( const Matrix &matrix, const char *destination, bool StateMatrix );
    void ConnectMatrix( const char *destination );
    void AddSeparator( const char *destination );
    void DeleteSeparator( const char *destination );

    std::stack< misc::StrCont > mSubSystemStack;
    misc::StrCont mFilename;         // name of the file that the model will be built in
    misc::StrCont mSimulinkModel;    // name of the block containing the actual model
    misc::StrCont mInitialStateStruct;
    std::vector< std::vector< boost::shared_ptr< ThermalState< T > > > > mThermalStates;
    boost::scoped_ptr< simulation::ElectricalSimulation< Matrix, T, false > > mElectricalSimulation;
    boost::scoped_ptr< simulation::ThermalSimulation< Matrix, T, false > > mThermalSimulation;
};

template < typename Matrix, typename T >
MatrixSimulinkModel< Matrix, T >::MatrixSimulinkModel( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode,
                                                       const char *filename, const char *initialStateStruct )
    : mFilename( filename )
    , mSimulinkModel( mFilename + "/Battery_MultiCell/Battery" )
    , mInitialStateStruct( initialStateStruct )
    , mElectricalSimulation( nullptr )
    , mThermalSimulation( nullptr )
{
    try
    {
        mElectricalSimulation.reset( new simulation::ElectricalSimulation< Matrix, T, false >( rootXmlNode, 0.0, 0.0, 0.0, nullptr ) );

        try
        {
            mThermalSimulation.reset(
             new simulation::ThermalSimulation< Matrix, T, false >( rootXmlNode, 0.0, 0.0, 0.0, nullptr,
                                                                    &mElectricalSimulation->mThermalStates, &mThermalStates ) );

            mexPrintf( "Thermal-electrical simulation initialized.\n" );
        }

        catch ( ... )
        {
            mexPrintf( "Electrical simulation initialized.\n" );
        }
    }

    catch ( ... )
    {
        try
        {
            mThermalSimulation.reset( new simulation::ThermalSimulation< Matrix, T, false >( rootXmlNode, 0.0, 0.0, 0.0,
                                                                                             nullptr, nullptr, nullptr ) );

            mexPrintf( "Thermal simulation initialized.\n" );
        }

        catch ( std::exception &e )
        {
            mexPrintf( e.what() );
        }

        catch ( ... )
        {
            mexPrintf( "Unknow Error.\n" );
        }
    }
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::CreateMatrixModel()
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 2 > addpathInput;
    addpathInput.at( 0 ) = mxCreateString( "./" );
    addpathInput.at( 1 ) = mxCreateString( "./MatrixModel/" );

    if ( mexCallMATLAB( 0, plhs, 2, addpathInput.data(), "addpath" ) != 0 )
    {
        mexPrintf( "The required pathes could not be added.\n" );
        return;
    }

    boost::array< mxArray *, 1 > bdcloseInput;
    bdcloseInput.at( 0 ) = mxCreateString( "all" );

    if ( mexCallMATLAB( 0, plhs, 1, bdcloseInput.data(), "bdclose" ) != 0 )
    {
        mexPrintf( "Older simulink systems could not be closed.\n" );
        return;
    }


    if ( mElectricalSimulation )
    {
        CreateElectricalModel();
        // ConnectAllMatrices();

        if ( mThermalSimulation )
            CreateThermalModel();

        else
        {
            misc::StrCont initialTemperaturesString( "[ " );
            for ( size_t i = 0; i < mElectricalSimulation->mCellElements.size(); ++i )
                initialTemperaturesString +=
                 mInitialStateStruct + ".CellElement_" + misc::StrCont( i + 1 ) + ".temperature ";
            initialTemperaturesString += "]";

            boost::array< mxArray *, 2 > noThermalModelInput;
            noThermalModelInput.at( 0 ) = mxCreateString( mSimulinkModel + "/Thermal" );
            noThermalModelInput.at( 1 ) = mxCreateString( initialTemperaturesString );


            if ( mexCallMATLAB( 0, plhs, 2, noThermalModelInput.data(), "CreateNoThermalModel" ) != 0 )
            {
                mexPrintf( "Error while passing cell temperatures." );
                return;
            }
        }
    }

    else if ( mThermalSimulation )
    {
        // CreateThermalModel();
        mexPrintf( "Thermal only simulation is not supported right now.\n" );
        return;
    }


    else
        return;

    boost::array< mxArray *, 1 > finishModelCreationInput;
    finishModelCreationInput.at( 0 ) = mxCreateString( mSimulinkModel );
    if ( mexCallMATLAB( 0, plhs, 1, finishModelCreationInput.data(), "FinishModelCreation" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel" );
    LoadInitialState();

    mexPrintf( "Simulation sucessfully created.\n" );

    boost::array< mxArray *, 1 > openSystem;
    openSystem.at( 0 ) = mxCreateString( mFilename );

    const int returnValue = mexCallMATLAB( 0, plhs, 1, openSystem.data(), "open_system" );
    if ( returnValue )
    {
        mexPrintf( "Error while opening system %s.\n", (const char *)mFilename );
        return;
    }
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::CreateElectricalModel()
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 4 > prhs;
    prhs.at( 0 ) = mxCreateString( mFilename );
    prhs.at( 1 ) = mxCreateDoubleScalar( mElectricalSimulation->mCellElements.size() );
    prhs.at( 2 ) = mxCreateDoubleScalar( mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount() );
    prhs.at( 3 ) = mxCreateDoubleScalar( mElectricalSimulation->mStateSystemGroup.mAlgStateSystem.GetEquationCount() );

    if ( mexCallMATLAB( 0, plhs, 4, prhs.data(), "InitializeMatrixModel" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 1 );


    mSubSystemStack.push( mSimulinkModel + misc::StrCont( "/ElectricalCircuit" ) );
    PlaceTwoPortsAndObjects( mElectricalSimulation->mRootTwoPort.get() );
    mSubSystemStack.pop();

    for ( size_t i = 0; i < mElectricalSimulation->mStateSystemGroup.GetStateCount(); ++i )
        mElectricalSimulation->mStateSystemGroup.mStateVector.coeffRef( i, 0 ) =
         ScalarUnit( misc::StrCont( "State" ) + misc::StrCont( i + 1 ) );

    mElectricalSimulation->mRootTwoPort->SetCurrent( ScalarUnit( "InputCurrent" ) );

    for ( size_t i = 0; i < mElectricalSimulation->mCellElements.size(); ++i )
        mElectricalSimulation->mCellElements.at( i )->SetCurrentFromActiveBalancer(
         ScalarUnit( misc::StrCont( "ActBal" ) + misc::StrCont( i + 1 ) ) );

    mElectricalSimulation->UpdateSystem();
    mElectricalSimulation->UpdateSystemValues();

    const Matrix &a_alg = mElectricalSimulation->mStateSystemGroup.mAlgStateSystem.GetEquationSystemAMatrix();
    Matrix a_alg1, a_alg2, invSystem;

    // TODO: FIX
    a_alg1 = a_alg.leftCols( mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount() );
    a_alg2 = a_alg.rightCols( mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount() );
    /*
    a_alg.VerticalSplitMatrix( a_alg1, , a_alg2 );
    */

    // mElectricalSimulation->mEqSystem->operator()()
    if ( mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount() > 0 )
    {
        auto sys = static_cast< systm::DifferentialAlgebraicSystem< Matrix > * >( mElectricalSimulation->mEqSystem.get() );
        auto matA = sys->GetA();
        auto matC = sys->GetC();

        Matrix tmp = matA * mElectricalSimulation->mStateSystemGroup.mStateVector.block(
                             0, 0, mElectricalSimulation->mStateSystemGroup.mStateVector.rows() - 1, 1 );
        tmp += matC;
        MakeMatrix( tmp, misc::StrCont( mSimulinkModel ) + misc::StrCont( "/MatrixAxC" ), true );
        if ( a_alg2.rows() != 0 && a_alg2.cols() != 0 )
        {
            // TODO: FIX!
            /*
            MakeStateResetMatrix( ( a_alg2.inverse() ) *
                                   (
            -mElectricalSimulation->mStateSystemGroup.mAlgStateSystem.GetEquationSystemCVector() - a_alg1 * x_alg ),
                                  misc::StrCont( mSimulinkModel ) + misc::StrCont( "/MatrixX_alg" ), true );
                                  */
        }
    }

    mElectricalSimulation->mRootTwoPort->CalculateStateDependentValues();

    // create matrix with current, voltage and power for root port
    myMatrixType current, voltage;
    myMatrixType totalValuesMatrix( 3, 1 );
    current = mElectricalSimulation->mRootTwoPort->GetCurrent() * mElectricalSimulation->mStateSystemGroup.mStateVector;
    voltage = *mElectricalSimulation->mRootTwoPort->GetVoltage() * mElectricalSimulation->mStateSystemGroup.mStateVector;
    totalValuesMatrix.coeffRef( 0, 0 ) = current.coeff( 0, 0 );
    totalValuesMatrix.coeffRef( 1, 0 ) = voltage.coeff( 0, 0 );
    totalValuesMatrix.coeffRef( 2, 0 ) = mElectricalSimulation->mRootTwoPort->GetPowerValue();
    MakeMatrix( totalValuesMatrix, misc::StrCont( mSimulinkModel ) + misc::StrCont( "/TotalValues" ) );
    prhs.at( 0 ) = mxCreateString( mSimulinkModel );
    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "ConnectTotalValuesMatrix" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 4 );

    // create matrices with curent, voltage and power for all cell elements
    myMatrixType cellCurrents( mElectricalSimulation->mCellElements.size(), 1 );
    myMatrixType cellVoltages( mElectricalSimulation->mCellElements.size(), 1 );
    myMatrixType cellPowers( mElectricalSimulation->mCellElements.size(), 1 );
    for ( size_t i = 0; i < mElectricalSimulation->mCellElements.size(); ++i )
    {
        current = mElectricalSimulation->mCellElements.at( i )->GetCurrent() * mElectricalSimulation->mStateSystemGroup.mStateVector;
        cellCurrents.coeffRef( i, 0 ) = current.coeff( 0, 0 );
        voltage = *mElectricalSimulation->mCellElements.at( i )->GetVoltage() * mElectricalSimulation->mStateSystemGroup.mStateVector;
        cellVoltages.coeffRef( i, 0 ) = voltage.coeff( 0, 0 );
        cellPowers.coeffRef( i, 0 ) = mElectricalSimulation->mCellElements.at( i )->GetPowerValue();
    }
    MakeMatrix( cellCurrents, misc::StrCont( mSimulinkModel ) + misc::StrCont( "/CellCurrentMatrices" ) );
    MakeMatrix( cellVoltages, misc::StrCont( mSimulinkModel ) + misc::StrCont( "/CellVoltageMatrices" ) );
    MakeMatrix( cellPowers, misc::StrCont( mSimulinkModel ) + misc::StrCont( "/PowerMatrix" ) );
    ConnectMatrix( misc::StrCont( mSimulinkModel ) + misc::StrCont( "/CellCurrentMatrices" ) );
    ConnectMatrix( misc::StrCont( mSimulinkModel ) + misc::StrCont( "/CellVoltageMatrices" ) );
    prhs.at( 0 ) = mxCreateString( mSimulinkModel );
    prhs.at( 1 ) = mxCreateDoubleScalar( mElectricalSimulation->mCellElements.size() );
    if ( mexCallMATLAB( 0, plhs, 2, prhs.data(), "ConnectPowerMatrix" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 2 );


    prhs.at( 0 ) = mxCreateString( mSimulinkModel );
    prhs.at( 1 ) = mxCreateDoubleScalar( mElectricalSimulation->mCellElements.size() );
    prhs.at( 2 ) = mxCreateDoubleScalar( mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount() );
    prhs.at( 3 ) = mxCreateDoubleScalar( mElectricalSimulation->mStateSystemGroup.mAlgStateSystem.GetEquationCount() );
    if ( mexCallMATLAB( 0, plhs, 4, prhs.data(), "MakeDefinesH" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 3 );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::CreateThermalModel()
{
    map< void *, size_t > pointerToIndexMap;
    pointerToIndexMap.insert( pair< void *, size_t >( static_cast< void * >( 0 ), 0 ) );    // TODO: static_cast<> here does not look elegant
    for ( size_t i = 0; i < mElectricalSimulation->mCellElements.size(); ++i )
        pointerToIndexMap.insert(
         pair< void *, size_t >( mElectricalSimulation->mCellElements.at( i )->GetThermalState().get(), i + 1 ) );

    vector< pair< size_t, double > > finiteVolumesByCellIndexAndFactor;
    finiteVolumesByCellIndexAndFactor.reserve( mThermalSimulation->mThermalSystem->GetOdeSystemSize() );
    BOOST_FOREACH ( const boost::shared_ptr< thermal::ThermalElement< T > > &thermalElement,
                    mThermalSimulation->mThermalSystem->GetThermalElements() )
    {
        const T factor = thermalElement->GetThermalStateFactor();
        state::ThermalState< T > *pointer = const_cast< state::ThermalState< T > * >( thermalElement->GetThermalState() );    // TODO: It is here necessary to use the const_cast of evil, because the declaration of map<>::find() does not work for pointers as key types
        const size_t index = pointerToIndexMap.find( pointer )->second;
        finiteVolumesByCellIndexAndFactor.push_back( pair< size_t, double >( index, factor ) );
    }

    const vector< T > &reciprocalsOfCapacity = mThermalSimulation->mThermalSystem->GetThermalElementFactors();
    const typename thermal::OdeSystemThermal< T >::MatrixT &conductivityMatrix =
     mThermalSimulation->mThermalSystem->GetA_th_Conductivity();

    mxArray **plhs = 0;
    boost::array< mxArray *, 5 > prhs;
    prhs.at( 0 ) = mxCreateString( mSimulinkModel + "/Thermal" );

    prhs.at( 1 ) = mxCreateDoubleMatrix( finiteVolumesByCellIndexAndFactor.size(), 2, mxREAL );
    misc::StrCont initialTemperaturesString( "[ " );
    // Matlab organizes its matrices in coloumn-major order
    for ( size_t i = 0; i < finiteVolumesByCellIndexAndFactor.size(); ++i )
    {
        size_t cellNumber = finiteVolumesByCellIndexAndFactor[i].first;
        mxGetPr( prhs.at( 1 ) )[i] = cellNumber;
        mxGetPr( prhs.at( 1 ) )[i + finiteVolumesByCellIndexAndFactor.size()] = finiteVolumesByCellIndexAndFactor[i].second;
        if ( cellNumber == 0 )
        {
            initialTemperaturesString +=
             misc::StrCont( mThermalSimulation->mThermalSystem->GetThermalElements().at( i )->GetTemperature() ) + " ";
        }
        else
        {
            initialTemperaturesString +=
             mInitialStateStruct + ".CellElement_" + misc::StrCont( cellNumber ) + ".temperature ";
        }
    }
    initialTemperaturesString += "]";
    prhs.at( 4 ) = mxCreateString( initialTemperaturesString );

    prhs.at( 2 ) = mxCreateDoubleMatrix( reciprocalsOfCapacity.size(), 1, mxREAL );
    // Matlab organizes its matrices in coloumn-major order
    for ( size_t i = 0; i < reciprocalsOfCapacity.size(); ++i )
        mxGetPr( prhs.at( 2 ) )[i] = reciprocalsOfCapacity.at( i );


    size_t rows = conductivityMatrix.nonZeros();
    prhs.at( 3 ) = mxCreateDoubleMatrix( rows, 3, mxREAL );

    size_t index = 0;
    for ( size_t i = 0; i < conductivityMatrix.rows(); ++i )
        for ( Eigen::SparseMatrix< double, Eigen::RowMajor >::InnerIterator it( conductivityMatrix, i ); it; ++it )
        {
            mxGetPr( prhs.at( 3 ) )[index] = i;
            mxGetPr( prhs.at( 3 ) )[index + rows] = it.col();
            mxGetPr( prhs.at( 3 ) )[index + rows * 2] = it.value();
            ++index;
        }

    if ( mexCallMATLAB( 0, plhs, 5, prhs.data(), "CreateThermalModel" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorThermalModel" );

    CreateCoolings();
    CreateThermalProbes();
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::CreateThermalProbes()
{
    const std::vector< boost::shared_ptr< thermal::ThermalElement< T > > > thermalElements =
     mThermalSimulation->mThermalSystem->GetThermalElements();
    size_t numberOfProbes = mThermalSimulation->mThermalProbes->size();
    boost::array< mxArray *, 2 > prhs;
    prhs[0] = mxCreateString( mSimulinkModel + "/Thermal" );
    prhs[1] = mxCreateDoubleMatrix( numberOfProbes, 1, mxREAL );
    auto finiteVolumesIndices = mxGetPr( prhs[1] );
    for ( size_t i = 0; i < numberOfProbes; ++i )
    {
        thermal::ThermalElement< T > *probeElement = ( *mThermalSimulation->mThermalProbes )[i].GetCorrespondingThermalElement();
        auto it = std::find_if( thermalElements.begin(), thermalElements.end(),
                                [probeElement]( const boost::shared_ptr< thermal::ThermalElement< T > > &element )
                                { return element.get() == probeElement; } );
        size_t index = std::distance( thermalElements.begin(), it );
        finiteVolumesIndices[i] = index + 1;
    }
    if ( mexCallMATLAB( 0, nullptr, 2, prhs.data(), "CreateThermalProbes" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorThermalModel" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::CreateCoolings()
{
    mxArray **plhs = 0;
    boost::array< mxArray *, 5 > prhs;
    prhs.at( 0 ) = mxCreateString( mSimulinkModel + "/Thermal/Coolings" );


    bool radiationActivated = false, convectionActivated = false;
    if ( mThermalSimulation->mThermalSystem->GetConvection().at( 0 ) )
        convectionActivated = true;
    if ( mThermalSimulation->mThermalSystem->GetRadiation() )
        radiationActivated = true;


    map< void *, size_t > pointerToIndexMap;
    pointerToIndexMap.insert( pair< void *, size_t >( static_cast< void * >( 0 ), 0 ) );    // TODO: static_cast<> here does not look elegant
    for ( size_t i = 0; i < mThermalSimulation->mCoolings.size(); ++i )
        pointerToIndexMap.insert( pair< void *, size_t >( mThermalSimulation->mCoolings.at( i ).get(), i + 1 ) );

    const vector< vector< thermal::TaylorData< T > > > &coolingDataVector =
     mThermalSimulation->mThermalSystem->GetCoolingDataVector();
    vector< vector< CoolingDataForMex > > coolingDataOutput( mThermalSimulation->mCoolings.size() + 1 );
    for ( size_t i = 0; i < coolingDataVector.size(); ++i )
        BOOST_FOREACH ( const thermal::TaylorData< T > &data, coolingDataVector.at( i ) )
            coolingDataOutput.at( pointerToIndexMap.find( data.mCooling )->second ).push_back( CoolingDataForMex( i, data ) );

    BOOST_FOREACH ( vector< CoolingDataForMex > &dataForOneCooling, coolingDataOutput )
    {
        if ( !dataForOneCooling.empty() )
        {
            prhs.at( 1 ) = mxCreateDoubleMatrix( dataForOneCooling.size(), 7, mxREAL );
            for ( size_t i = 0; i < dataForOneCooling.size(); ++i )
            {
                /*Matlab index 1*/ mxGetPr( prhs.at( 1 ) )[i] = dataForOneCooling.at( i ).mIndex;
                /*Matlab index 2*/ mxGetPr( prhs.at( 1 ) )[i + dataForOneCooling.size()] =
                 dataForOneCooling.at( i ).mTaylorData.mCharacteristicLength;
                /*Matlab index 3*/ mxGetPr( prhs.at( 1 ) )[i + dataForOneCooling.size() * 2] =
                 dataForOneCooling.at( i ).mTaylorData.mA_cool;
                /*Matlab index 4*/ mxGetPr( prhs.at( 1 ) )[i + dataForOneCooling.size() * 3] =
                 dataForOneCooling.at( i ).mTaylorData.mTempSurfLastStep;
                /*Matlab index 5*/ mxGetPr( prhs.at( 1 ) )[i + dataForOneCooling.size() * 4] =
                 dataForOneCooling.at( i ).mTaylorData.mLocation;
                /*Matlab index 6*/ mxGetPr( prhs.at( 1 ) )[i + dataForOneCooling.size() * 5] =
                 dataForOneCooling.at( i ).mTaylorData.mConductivity;
                /*Matlab index 7*/ mxGetPr( prhs.at( 1 ) )[i + dataForOneCooling.size() * 6] =
                 dataForOneCooling.at( i ).mTaylorData.mDistanceToGridVertex;
            }

            if ( !dataForOneCooling.at( 0 ).mTaylorData.mCooling )
            {
                prhs.at( 2 ) = mxCreateLogicalScalar( convectionActivated );
                prhs.at( 3 ) = mxCreateLogicalScalar( radiationActivated );
                if ( mexCallMATLAB( 0, plhs, 4, prhs.data(), "CreateDefaultCooling" ) != 0 )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorCoolingModel", 1 );
            }
            else if ( dynamic_cast< thermal::CoolingByLookUp< T > * >( dataForOneCooling.at( 0 ).mTaylorData.mCooling ) )
            {
                thermal::CoolingByLookUp< T > *coolingByLookUp =
                 static_cast< thermal::CoolingByLookUp< T > * >( dataForOneCooling.at( 0 ).mTaylorData.mCooling );
                const object::LookupObj1D< T > *lookUpObj =
                 static_cast< object::LookupObj1D< T > * >( coolingByLookUp->GetOffsetLookUp().get() );

                const std::vector< T > &measurementPoints = lookUpObj->GetLookup().GetLookupType()->GetPoints();
                const std::vector< T > &dataPoints = lookUpObj->GetLookup().GetLookup();
                assert( measurementPoints.size() == dataPoints.size() );


                const size_t numberOfPoints = measurementPoints.size();
                prhs.at( 2 ) = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
                prhs.at( 3 ) = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );

                for ( size_t i = 0; i < numberOfPoints; ++i )
                {
                    mxGetPr( prhs.at( 2 ) )[i] = measurementPoints.at( i );
                    mxGetPr( prhs.at( 3 ) )[i] = dataPoints.at( i );
                }


                if ( mexCallMATLAB( 0, plhs, 4, prhs.data(), "CreateCoolingByLookUp" ) != 0 )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorCoolingModel", 2 );
            }
            else if ( dynamic_cast< thermal::CoolingByConstantValue< T > * >( dataForOneCooling.at( 0 ).mTaylorData.mCooling ) )
            {
                thermal::CoolingByConstantValue< T > *coolingByConstantValue =
                 static_cast< thermal::CoolingByConstantValue< T > * >( dataForOneCooling.at( 0 ).mTaylorData.mCooling );
                prhs.at( 2 ) = mxCreateDoubleScalar( coolingByConstantValue->GetValue() );
                if ( mexCallMATLAB( 0, plhs, 3, prhs.data(), "CreateCoolingByConstantValue" ) != 0 )
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorCoolingModel", 3 );
            }
            else if ( !dynamic_cast< thermal::DirichletBoundaryCondition< T > * >( dataForOneCooling.at( 0 ).mTaylorData.mCooling ) )
            {
                ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPassThrough",
                                                     "Error parsing cooling, cooling subclass not recognized" );
            }
        }
    }


    const vector< typename thermal::OdeSystemThermal< T >::BoundarySourceData > &matrixDirichlet =
     mThermalSimulation->mThermalSystem->GetMatrixDirichlet();
    vector< pair< size_t, typename thermal::OdeSystemThermal< T >::BoundarySourceData > > dirchletData;
    for ( size_t i = 0; i < matrixDirichlet.size(); ++i )
    {
        const T a = matrixDirichlet.at( i ).mA_th;
        const T c = matrixDirichlet.at( i ).mC_th;
        if ( a != 0.0 || c != 0.0 )
        {
            pair< size_t, typename thermal::OdeSystemThermal< T >::BoundarySourceData > dirichlet;
            dirichlet.first = i;
            dirichlet.second.mA_th = a;
            dirichlet.second.mC_th = c;
            dirchletData.push_back( dirichlet );
        }
    }

    prhs.at( 1 ) = mxCreateDoubleMatrix( dirchletData.size(), 3, mxREAL );
    for ( size_t i = 0; i < dirchletData.size(); ++i )
    {
        mxGetPr( prhs.at( 1 ) )[i] = dirchletData.at( i ).first;
        mxGetPr( prhs.at( 1 ) )[i + dirchletData.size()] = dirchletData.at( i ).second.mA_th;
        mxGetPr( prhs.at( 1 ) )[i + dirchletData.size() * 2] = dirchletData.at( i ).second.mC_th;
    }
    if ( !dirchletData.empty() )
        if ( mexCallMATLAB( 0, plhs, 2, prhs.data(), "CreateDirichletConditions" ) != 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorCoolingModel", 4 );


    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "ConnectAllCoolings" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorCoolingModel", 5 );
}


template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::PlaceTwoPortsAndObjects( const electrical::TwoPort< Matrix > *twoPort )
{
    // Elements with only one object
    if ( dynamic_cast< const electrical::Capacity< Matrix > * >( twoPort ) ||
         dynamic_cast< const electrical::OhmicResistance< Matrix > * >( twoPort ) ||
         dynamic_cast< const electrical::VoltageSource< Matrix > * >( twoPort ) )
    {
        PlaceElectricalElement( twoPort );
    }
    // Elements with several objects
    else if ( dynamic_cast< const electrical::ParallelRC< Matrix > * >( twoPort ) ||
              dynamic_cast< const electrical::ParallelRCAlg< Matrix > * >( twoPort ) )
    {
        PlaceParallelRC( twoPort );
    }
    else if ( dynamic_cast< const electrical::SphericalDiffusion< Matrix > * >( twoPort ) )
    {
        PlaceSphericalDiffusion( twoPort );
    }
    else if ( dynamic_cast< const electrical::Zarc< Matrix > * >( twoPort ) ||
              dynamic_cast< const electrical::ZarcAlg< Matrix > * >( twoPort ) )
    {
        PlaceZarc( twoPort );
    }
    else if ( dynamic_cast< const electrical::Rmphn< Matrix > * >( twoPort ) )
    {
        PlaceRMPHN( twoPort );
    }
    // Elements with children
    else if ( dynamic_cast< const electrical::WarburgTanh< Matrix > * >( twoPort ) ||
              dynamic_cast< const electrical::WarburgCotanh< Matrix > * >( twoPort ) ||
              dynamic_cast< const electrical::ParallelTwoPort< Matrix > * >( twoPort ) ||
              dynamic_cast< const electrical::SerialTwoPort< Matrix > * >( twoPort ) ||
              dynamic_cast< const electrical::Cellelement< Matrix > * >( twoPort ) )
    {
        PlaceElectricalElementWithChildren( twoPort );
    }
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::PlaceElectricalElement( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 5 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateDoubleScalar( twoPort->GetID() );

    const electrical::ElectricalElement< Matrix > *electricalElement =
     static_cast< const electrical::ElectricalElement< Matrix > * >( twoPort );
    GetObjectData( electricalElement->GetElementObject(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );

    if ( mexCallMATLAB( 0, plhs, 5, prhs.data(), "MatrixCreateElectricalElement" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                             "PlaceElectricalElement" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::PlaceSphericalDiffusion( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 8 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateDoubleScalar( twoPort->GetID() );

    const electrical::SphericalDiffusion< Matrix > *parallelRC =
     static_cast< const electrical::SphericalDiffusion< Matrix > * >( twoPort );
    auto objects = parallelRC->GetObjects();
    GetObjectData( objects[0].get(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );
    GetObjectData( objects[1].get(), prhs.at( 5 ), prhs.at( 6 ), prhs.at( 7 ) );

    if ( mexCallMATLAB( 0, plhs, 8, prhs.data(), "MatrixCreateParallelRC" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "ParallelRC" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::PlaceParallelRC( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 8 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateDoubleScalar( twoPort->GetID() );

    const electrical::ParallelRC< Matrix > *parallelRC = static_cast< const electrical::ParallelRC< Matrix > * >( twoPort );
    GetObjectData( parallelRC->GetElementObject(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );
    GetObjectData( parallelRC->GetTauObject(), prhs.at( 5 ), prhs.at( 6 ), prhs.at( 7 ) );

    if ( mexCallMATLAB( 0, plhs, 8, prhs.data(), "MatrixCreateParallelRC" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "ParallelRC" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::PlaceRMPHN( const electrical::TwoPort< Matrix > *twoPort )
{
    boost::array< mxArray *, 1 > plhs;
    boost::array< mxArray *, 8 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateDoubleScalar( twoPort->GetID() );

    const electrical::Rmphn< Matrix > *rmphn = static_cast< const electrical::Rmphn< Matrix > * >( twoPort );
    auto objects = rmphn->GetObjects();
    GetObjectData( objects[0].get(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );
    GetObjectData( objects[1].get(), prhs.at( 5 ), prhs.at( 6 ), prhs.at( 7 ) );

    if ( mexCallMATLAB( 1, plhs.data(), 8, prhs.data(), "MatrixCreateRMPHN" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "RMPHN" );

    std::vector< char > stringbuffer( mxGetN( plhs.at( 0 ) ) + 1 );
    mxGetString( plhs[0], &stringbuffer[0], stringbuffer.size() );
    mSubSystemStack.push( misc::StrCont( &stringbuffer[0] ) );

    PlaceTwoPortsAndObjects( rmphn->at( 0 ) );

    mSubSystemStack.pop();
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::PlaceZarc( const electrical::TwoPort< Matrix > *twoPort )
{
    mxArray **plhs = 0;

    boost::array< mxArray *, 17 > prhs;
    prhs.at( 0 ) = mxCreateString( MakeDestination( twoPort ) );
    prhs.at( 1 ) = mxCreateDoubleScalar( twoPort->GetID() );

    const electrical::Zarc< Matrix > *zarc = static_cast< const electrical::Zarc< Matrix > * >( twoPort );
    GetObjectData( zarc->GetRObject(), prhs.at( 2 ), prhs.at( 3 ), prhs.at( 4 ) );
    GetObjectData( zarc->GetTauObject(), prhs.at( 5 ), prhs.at( 6 ), prhs.at( 7 ) );
    GetObjectData( zarc->GetPhiObject(), prhs.at( 8 ), prhs.at( 9 ), prhs.at( 10 ) );

    GetLookup1DData( zarc->GetLookupPhi2RFactorAussen(), prhs.at( 11 ), prhs.at( 12 ) );
    GetLookup1DData( zarc->GetLookupPhi2RFactorInnen(), prhs.at( 13 ), prhs.at( 14 ) );
    GetLookup1DData( zarc->GetLookupPhi2TauFactor(), prhs.at( 15 ), prhs.at( 16 ) );

    if ( mexCallMATLAB( 0, plhs, 17, prhs.data(), "MatrixCreateZarc" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing", "Zarc" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::PlaceElectricalElementWithChildren( const electrical::TwoPort< Matrix > *twoPort )
{
    misc::StrCont destination = MakeDestination( twoPort );
    boost::array< mxArray *, 1 > plhs;
    boost::array< mxArray *, 2 > prhs;

    const electrical::Cellelement< Matrix > *cellelement = dynamic_cast< const electrical::Cellelement< Matrix > * >( twoPort );
    bool isWarburg = dynamic_cast< const electrical::WarburgTanh< Matrix > * >( twoPort ) ||
                     dynamic_cast< const electrical::WarburgCotanh< Matrix > * >( twoPort );
    if ( cellelement )
    {
        prhs.at( 0 ) = mxCreateString( destination );
        prhs.at( 1 ) = mxCreateDoubleScalar( cellelement->GetSoc()->GetInitialCapacity() );
        if ( mexCallMATLAB( 1, plhs.data(), 2, prhs.data(), "MatrixCreateCellelement" ) != 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                                 "electrical element with child 1" );
    }
    else
    {
        prhs.at( 0 ) = mxCreateString( destination );
        prhs.at( 1 ) = mxCreateLogicalScalar( isWarburg );
        if ( mexCallMATLAB( 1, plhs.data(), 2, prhs.data(), "MatrixCreateTwoPortWithChild" ) != 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                                 "electrical element with child 2" );
    }

    std::vector< char > stringbuffer( mxGetN( plhs.at( 0 ) ) + 1 );
    mxGetString( plhs[0], &stringbuffer[0], stringbuffer.size() );
    destination = misc::StrCont( &stringbuffer[0] );

    mSubSystemStack.push( destination );
    const electrical::TwoPortWithChild< Matrix > *twoPortWithChild =
     static_cast< const electrical::TwoPortWithChild< Matrix > * >( twoPort );
    for ( size_t i = 0; i < twoPortWithChild->size(); ++i )
        PlaceTwoPortsAndObjects( twoPortWithChild->at( i ) );
    mSubSystemStack.pop();

    if ( cellelement )
    {
        prhs.at( 0 ) = mxCreateString( destination );
        if ( mexCallMATLAB( 0, plhs.data(), 1, prhs.data(), "MatrixConnectCellelement" ) != 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                                 "electrical element with child 3" );

        prhs.at( 0 ) = mxCreateString( mSimulinkModel + "/InitialSoC_Vector" );
        prhs.at( 1 ) = mxCreateString( mInitialStateStruct + ".CellElement_" +
                                       misc::StrCont( cellelement->mCellNumber + 1 ) + ".SoC" );
        if ( mexCallMATLAB( 0, plhs.data(), 2, prhs.data(), "MatrixSetInitialSoC" ) != 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                                 "electrical element with child 4" );
    }
    else if ( isWarburg )
    {
        prhs.at( 0 ) = mxCreateString( destination );
        if ( mexCallMATLAB( 0, plhs.data(), 1, prhs.data(), "MatrixConnectTwoPortWithChild" ) != 0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorPlacing",
                                                 "electrical element with child 5" );
    }
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::GetObjectData( const object::Object< T > *object, mxArray *&rowPoints,
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
        const object::LookupObj1dWithState< T > *lookupObj1dWithState =
         static_cast< const object::LookupObj1dWithState< T > * >( object );
        const lookup::LookupType1D< T > *lookUpType =
         static_cast< const object::LookupObj1dWithState< T > * >( object )->GetLookup().GetLookupType().get();

        const size_t numberOfPoints = lookUpType->GetPoints().size();
        if ( dynamic_cast< const state::Soc * >( lookupObj1dWithState->GetState() ) )
        {
            rowPoints = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
            for ( size_t i = 0; i < numberOfPoints; ++i )
                mxGetPr( rowPoints )[i] = lookUpType->GetPoints().at( i );
            colPoints = mxCreateDoubleScalar( 1.0 );
        }
        else
        {
            colPoints = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
            for ( size_t i = 0; i < numberOfPoints; ++i )
                mxGetPr( colPoints )[i] = lookUpType->GetPoints().at( i );
            rowPoints = mxCreateDoubleScalar( 1.0 );
        }

        dataMatrix = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
        for ( size_t i = 0; i < numberOfPoints; ++i )
            mxGetPr( dataMatrix )[i] = lookUpType->GetLookup().at( i );
    }
    else if ( dynamic_cast< const object::ConstObj< T > * >( object ) )
    {
        rowPoints = mxCreateDoubleScalar( 1.0 );
        colPoints = mxCreateDoubleScalar( 1.0 );
        dataMatrix = mxCreateDoubleScalar( object->GetValue() );
    }
    else
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorObject", "2" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::GetLookup1DData( const lookup::Lookup1D< T > &lookup1D, mxArray *&points, mxArray *&dataMatrix ) const
{
    const lookup::LookupType1D< T > *loopUpType = lookup1D.GetLookupType().get();

    const size_t numberOfPoints = loopUpType->GetPoints().size();
    points = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
    for ( size_t i = 0; i < numberOfPoints; ++i )
        mxGetPr( points )[i] = loopUpType->GetPoints().at( i );

    dataMatrix = mxCreateDoubleMatrix( 1, numberOfPoints, mxREAL );
    for ( size_t i = 0; i < numberOfPoints; ++i )
        mxGetPr( dataMatrix )[i] = loopUpType->GetLookup().at( i );
}

template < typename Matrix, typename T >
misc::StrCont MatrixSimulinkModel< Matrix, T >::MakeDestination( const electrical::TwoPort< Matrix > *twoPort ) const
{
    return mSubSystemStack.top() + "/" + twoPort->GetName();
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::MakeMatrix( const Matrix &matrix, const char *destination, bool StateMatrix )
{
    for ( size_t i = 0; i < matrix.rows(); ++i )
    {
        for ( size_t j = 0; j < matrix.cols(); ++j )
        {
            mxArray **plhs = 0;
            boost::array< mxArray *, 5 > prhs;
            prhs.at( 0 ) = mxCreateString( destination );
            prhs.at( 1 ) = mxCreateString( matrix.coeff( i, j ).GetString() );
            prhs.at( 2 ) = mxCreateDoubleScalar( i );
            prhs.at( 3 ) = mxCreateDoubleScalar( j );
            prhs.at( 4 ) = mxCreateDoubleScalar( mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount() );

            if ( !StateMatrix )
            {
                if ( mexCallMATLAB( 0, plhs, 4, prhs.data(), "MakeComponent" ) != 0 )
                {
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 1 );
                }
            }
            else
            {
                if ( mexCallMATLAB( 0, plhs, 5, prhs.data(), "MakeStateComponent" ) != 0 )
                {
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 1 );
                }
            }
        }
    }
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::MakeStateResetMatrix( const Matrix &matrix, const char *destination, bool StateMatrix )
{
    UNUSED( StateMatrix );
    for ( size_t i = 0; i < matrix.n_rows; ++i )
        for ( size_t j = 0; j < matrix.n_cols; ++j )
            if ( !matrix.coeff( i, j ).IsEmpty() )
            {
                mxArray **plhs = 0;
                boost::array< mxArray *, 6 > prhs;
                prhs.at( 0 ) = mxCreateString( destination );
                prhs.at( 1 ) = mxCreateString( matrix( i, j ).GetString() );
                prhs.at( 2 ) = mxCreateDoubleScalar( i );
                prhs.at( 3 ) = mxCreateDoubleScalar( j );
                prhs.at( 4 ) =
                 mxCreateDoubleScalar( mElectricalSimulation->mStateSystemGroup.mDglStateSystem.GetEquationCount() +
                                       mElectricalSimulation->mStateSystemGroup.mAlgStateSystem.GetEquationCount() );
                prhs.at( 5 ) = mxCreateDoubleScalar( mElectricalSimulation->mCellElements.size() );

                if ( mexCallMATLAB( 0, plhs, 6, prhs.data(), "MakeStateResetComponent" ) != 0 )
                {
                    ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 1 );
                }
            }
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::ConnectMatrix( const char *destination )
{
    mxArray **plhs = 0;
    boost::array< mxArray *, 1 > prhs;
    prhs.at( 0 ) = mxCreateString( destination );
    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "ConnectMatrix" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel", 1 );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::AddSeparator( const char *destination )
{
    mxArray **plhs = 0;
    boost::array< mxArray *, 1 > prhs;
    prhs.at( 0 ) = mxCreateString( destination );

    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "AddSeparator" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "ErrorPlacing"
                                             "Seperator" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::DeleteSeparator( const char *destination )
{
    mxArray **plhs = 0;
    boost::array< mxArray *, 1 > prhs;
    prhs.at( 0 ) = mxCreateString( destination );

    if ( mexCallMATLAB( 0, plhs, 1, prhs.data(), "DeleteSeparator" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "ErrorPlacing"
                                             " deleting Seperator" );
}

// write the initial SOCs and temperatures to the matlab workspace
template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::LoadInitialState()
{
    const size_t numberOfCells = this->mElectricalSimulation->mCellElements.size();
    mxArray *socArray = mxCreateDoubleMatrix( numberOfCells, 1, mxREAL );
    mxArray *temperatureArray = mxCreateDoubleMatrix( numberOfCells, 1, mxREAL );
    double *socDoubles = mxGetPr( socArray );
    double *temperatureDoubles = mxGetPr( temperatureArray );
    for ( size_t i = 0; i < numberOfCells; ++i )
    {
        electrical::Cellelement< Matrix > *cell = this->mElectricalSimulation->mCellElements[i].get();
        socDoubles[i] = cell->GetSoc()->GetValue();
        temperatureDoubles[i] = cell->GetThermalState()->GetValue();
    }
    mxArray **plhs = 0;
    boost::array< mxArray *, 3 > prhs{ mxCreateString( mInitialStateStruct ), socArray, temperatureArray };
    if ( mexCallMATLAB( 0, plhs, 3, prhs.data(), "LoadInitialStateToWorkspace" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel",
                                             "loading initial state" );
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::ObjectDataToDataVector( const object::Object< T > *object, mxArray *dataVector,
                                                               const misc::StrCont &destination, size_t &index )
{
    mxArray *rowPoints, *colPoints, *values;
    GetObjectData( object, rowPoints, colPoints, values );
    if ( mxGetDimensions( values )[0] == 1 )    // ConstObj
    {
        mxSetField( dataVector, index, "values", values );
    }
    else
    {
        mxSetField( dataVector, index, "rowPoints", rowPoints );
        mxSetField( dataVector, index, "colPoints", colPoints );
        mxSetField( dataVector, index, "values", values );
    }
    mxSetField( dataVector, index, "destination", mxCreateString( destination ) );
    index++;
}

template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::LookupDataToDataVector( const lookup::Lookup1D< T > &lookup1D, mxArray *dataVector,
                                                               const misc::StrCont &destination, size_t &index )
{
    mxSetField( dataVector, index, "destination", mxCreateString( destination ) );
    mxArray *rowPoints, *values;
    GetLookup1DData( lookup1D, rowPoints, values );
    mxSetField( dataVector, index, "rowPoints", rowPoints );
    mxSetField( dataVector, index, "values", values );
    index++;
}

// helper for LoadObjectValues
template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::GetObjectValues( const electrical::TwoPort< Matrix > *twoPort, mxArray *dataVector,
                                                        const misc::StrCont &destination, size_t &index )
{

    const electrical::TwoPortWithChild< Matrix > *twoportWithChild;
    const electrical::ParallelRC< Matrix > *parallelRC;
    const electrical::ParallelRCAlg< Matrix > *parallelRCAlg;
    const electrical::SphericalDiffusion< Matrix > *sphericalDiffusion;
    const electrical::Zarc< Matrix > *zarc;
    const electrical::ZarcAlg< Matrix > *zarcAlg;
    const electrical::Rmphn< Matrix > *rmphn;
    const electrical::ElectricalElement< Matrix > *electricalElement;

    if ( ( rmphn = dynamic_cast< const electrical::Rmphn< Matrix > * >( twoPort ) ) )
    {
        ObjectDataToDataVector( rmphn->GetObjects()[0].get(), dataVector, destination + "/Object", index );
        ObjectDataToDataVector( rmphn->GetObjects()[1].get(), dataVector, destination + "/Tau", index );
    }
    else if ( ( twoportWithChild = dynamic_cast< const electrical::TwoPortWithChild< Matrix > * >( twoPort ) ) )
    {
        for ( size_t i = 0; i < twoportWithChild->size(); ++i )
        {
            auto child = twoportWithChild->at( i );
            GetObjectValues( child, dataVector, destination + "/" + child->GetName() + "_Elem" + misc::StrCont( i + 1 ), index );
        }
    }
    else if ( ( parallelRC = dynamic_cast< const electrical::ParallelRC< Matrix > * >( twoPort ) ) )
    {
        ObjectDataToDataVector( parallelRC->GetElementObject(), dataVector, destination + "/Object", index );
        ObjectDataToDataVector( parallelRC->GetTauObject(), dataVector, destination + "/Tau", index );
    }
    else if ( ( parallelRCAlg = dynamic_cast< const electrical::ParallelRCAlg< Matrix > * >( twoPort ) ) )
    {
        ObjectDataToDataVector( parallelRCAlg->GetElementObject(), dataVector, destination + "/Object", index );
        ObjectDataToDataVector( parallelRCAlg->GetCObject(), dataVector, destination + "/Tau", index );
    }
    else if ( ( sphericalDiffusion = dynamic_cast< const electrical::SphericalDiffusion< Matrix > * >( twoPort ) ) )
    {
        ObjectDataToDataVector( sphericalDiffusion->GetObjects()[0].get(), dataVector, destination + "/Object", index );
        ObjectDataToDataVector( sphericalDiffusion->GetObjects()[1].get(), dataVector, destination + "/Tau", index );
    }
    else if ( ( zarc = dynamic_cast< const electrical::Zarc< Matrix > * >( twoPort ) ) )
    {
        ObjectDataToDataVector( zarc->GetRObject(), dataVector, destination + "/Resistance", index );
        ObjectDataToDataVector( zarc->GetTauObject(), dataVector, destination + "/Tau", index );
        ObjectDataToDataVector( zarc->GetPhiObject(), dataVector, destination + "/Phi", index );
        LookupDataToDataVector( zarc->GetLookupPhi2RFactorAussen(), dataVector, destination + "/Phi2RAussen", index );
        LookupDataToDataVector( zarc->GetLookupPhi2RFactorInnen(), dataVector, destination + "/Phi2RInnen", index );
        LookupDataToDataVector( zarc->GetLookupPhi2TauFactor(), dataVector, destination + "/Phi2Tau", index );
    }
    else if ( ( zarcAlg = dynamic_cast< const electrical::ZarcAlg< Matrix > * >( twoPort ) ) )
    {
        ObjectDataToDataVector( zarcAlg->GetRObject(), dataVector, destination + "/Resistance", index );
        ObjectDataToDataVector( zarcAlg->GetTauObject(), dataVector, destination + "/Tau", index );
        ObjectDataToDataVector( zarcAlg->GetPhiObject(), dataVector, destination + "/Phi", index );
        LookupDataToDataVector( zarcAlg->GetLookupPhi2RFactorAussen(), dataVector, destination + "/Phi2RAussen", index );
        LookupDataToDataVector( zarcAlg->GetLookupPhi2RFactorInnen(), dataVector, destination + "/Phi2RInnen", index );
        LookupDataToDataVector( zarcAlg->GetLookupPhi2TauFactor(), dataVector, destination + "/Phi2Tau", index );
    }
    else if ( ( electricalElement = dynamic_cast< const electrical::ElectricalElement< Matrix > * >( twoPort ) ) )
    {
        ObjectDataToDataVector( electricalElement->GetElementObject(), dataVector, destination + "/Object", index );
    }
}

// write the object values to the matlab workspace
template < typename Matrix, typename T >
void MatrixSimulinkModel< Matrix, T >::LoadObjectValues()
{
    auto rootPort = mElectricalSimulation->mRootTwoPort.get();
    size_t index = 0;
    mwSize arraySize[1] = { this->mElectricalSimulation->mNumberOfObjects };
    const char *fieldnames[4] = { "destination", "rowPoints", "colPoints", "values" };
    // const char** fieldnamePtr = &fieldnames[0];
    mxArray *data = mxCreateStructArray( 1, arraySize, 4, (const char **)fieldnames );
    GetObjectValues( rootPort, data, mSimulinkModel + "/ElectricalCircuit/" + rootPort->GetName() + "_Elem1", index );
    if ( mexCallMATLAB( 0, 0, 1, &data, "MatrixLoadObjectValues" ) != 0 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "ErrorMatrixModel",
                                             "loading object values" );
}

}    // namespace visualizer
#endif    // _SYMBOLIC_
#endif    // _MATRIX_SIMULINK_MODEL_
