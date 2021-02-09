#ifndef _ODE_SYSTEM_THERMAL_
#define _ODE_SYSTEM_THERMAL_

#include "../exceptions/error_proto.h"
#include "blocks/elements/thermal_element.h"
#include "boundaryConditions/cooling.h"
#include "boundaryConditions/default_convection.h"
#include "thermal_structs.h"
#include <boost/shared_ptr.hpp>
#include <cmath>
#include <list>
#include <map>
#include <vector>

#include <eigen3/Eigen/Sparse>

class TestOdeSystemThermal;
class TestSimulation;

namespace thermal
{
using namespace thermal;
using boost::shared_ptr;
using std::list;
using std::vector;

/// OdeSystemThermal gathers data and provides time derivatives for the equation solver
template < typename T = double >
class OdeSystemThermal
{
    friend class ::TestOdeSystemThermal;
    friend class ::TestSimulation;

#ifndef _MATLAB_
#endif
    public:
    struct BoundarySourceData
    {
        ALIGNED_TO_64_BYTE T mA_th;    // Slope
        ALIGNED_TO_64_BYTE T mC_th;    // Offset
    };

    using MatrixT = Eigen::SparseMatrix< T, Eigen::RowMajor >;

    /**
     * @param[in] dirichletDataVector Vector containing the data for all dirichlet boundary conditions
     * @param[in] coolingDataVector Vector containing the data for all coolings
     * @param[in] defaultConvection Vector with default convections mapped by Location enum
     * @param[in] thermalStates,unconnectedThermalStates,materials,coolings These need to be present during simulation
     * and may be stored here if desired
     */
    OdeSystemThermal(
     vector< shared_ptr< ThermalElement< T > > > &thermalElements, vector< vector< IndexedValue< T > > > &conductivityMatrix,
     vector< vector< TaylorData< T > > > &coolingDataVector, const vector< vector< TaylorData< T > > > &dirichletDataVector,
     const vector< shared_ptr< DefaultConvection< T > > > &defaultConvection, const shared_ptr< Radiation< T > > defaultRadiation,
     T airTemperature, vector< shared_ptr< state::ThermalState< T > > > thermalStates,
     vector< shared_ptr< state::ThermalState< T > > > unconnectedThermalStates = vector< shared_ptr< state::ThermalState< T > > >(),
     vector< shared_ptr< Material< T > > > materials = vector< shared_ptr< Material< T > > >(),
     vector< shared_ptr< Cooling< T > > > coolings = vector< shared_ptr< Cooling< T > > >() );
    virtual ~OdeSystemThermal();
    /// Needs to be called every time before equation solver interface function is used, updates mC_th_*, mA_th_* and
    /// optionally sets air temperature
    void Update( T time, T dt );
    /// Equation solver interface function, called multiple times during equation solving
    void operator()( const vector< T > &x, vector< T > &dxdt, const double /* t */ );
    /// One iteration of loop used in Update()
    void UpdateLoop( size_t i, T time, T dt );
    void GetTemperatureVector( vector< T > &temperatureVector ) const;
    void SetTemperatureVector( const vector< T > &temperatureVector );
    void ResetAirTemperature( T newAirTemperature );
    inline size_t GetOdeSystemSize() const;
    const vector< shared_ptr< ThermalElement< T > > > &GetThermalElements() const;
    const MatrixT &GetA_th_Conductivity() const;
    const vector< T > &GetThermalElementFactors() const;
    const vector< shared_ptr< DefaultConvection< T > > > &GetConvection() const;
    const shared_ptr< Radiation< T > > &GetRadiation() const;
    const vector< BoundarySourceData > &GetMatrixDirichlet() const;
    const vector< vector< TaylorData< T > > > &GetCoolingDataVector() const;
    double GetAirTemperature() const { return mAirTemperature; };
    // Provisional Hack
    void SetDirichletBoundaryCondition( vector< size_t > dirichletIndices, T dirichletTemperature, T dirichletConductivity );

    private:
    vector< shared_ptr< ThermalElement< T > > > mThermalElements;
    MatrixT mA_th_Conductivity;
    vector< vector< TaylorData< T > > > mCoolingDataVector;
    const vector< shared_ptr< DefaultConvection< T > > > mConvection;
    const shared_ptr< Radiation< T > > mRadiation;
    T mAirTemperature;

    vector< BoundarySourceData > mMatrixBoundarySource;    ///<Stores values for heat generation and cooling created by Update()
    vector< BoundarySourceData > mMatrixDirichlet;    ///<Stores values for dirichlet boundary condition
    vector< T > mThermalElementFactors;               ///<Stores reciprocals of the total capacity of thermal elements
    const size_t mOdeSystemSize;

    vector< shared_ptr< state::ThermalState< T > > > mThermalStates;
    vector< shared_ptr< state::ThermalState< T > > > mUnconnectedThermalStates;
    vector< shared_ptr< Material< T > > > mMaterials;
    vector< shared_ptr< Cooling< T > > > mCoolings;
};


template < typename T >
OdeSystemThermal< T >::OdeSystemThermal( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                         vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                         vector< vector< TaylorData< T > > > &coolingDataVector,
                                         const vector< vector< TaylorData< T > > > &dirichletDataVector,
                                         const vector< shared_ptr< DefaultConvection< T > > > &defaultConvection,
                                         const shared_ptr< Radiation< T > > defaultRadiation, T airTemperature,
                                         vector< shared_ptr< state::ThermalState< T > > > thermalStates,
                                         vector< shared_ptr< state::ThermalState< T > > > unconnectedThermalStates,
                                         vector< shared_ptr< Material< T > > > materials,
                                         vector< shared_ptr< Cooling< T > > > coolings )
    : mConvection( defaultConvection )
    , mRadiation( defaultRadiation )
    , mAirTemperature( airTemperature )
    , mMatrixBoundarySource( thermalElements.size() )
    , mMatrixDirichlet( thermalElements.size() )
    , mOdeSystemSize( thermalElements.size() )
    , mThermalStates( thermalStates )
    , mUnconnectedThermalStates( unconnectedThermalStates )
    , mMaterials( materials )
    , mCoolings( coolings )
{
    if ( thermalElements.size() != conductivityMatrix.size() || thermalElements.size() != coolingDataVector.size() ||
         thermalElements.size() != dirichletDataVector.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "thermalElementEqualSize" );

    mThermalElements.swap( thermalElements );
    mCoolingDataVector.swap( coolingDataVector );

    size_t conductivitySize = conductivityMatrix.size();
    std::vector< size_t > rowEntries;
    rowEntries.reserve( conductivitySize );
    for ( size_t row = 0; row < conductivitySize; ++row )
    {
        rowEntries.push_back( conductivityMatrix[row].size() );
    }
    mA_th_Conductivity.resize( conductivitySize, conductivitySize );
    if ( conductivitySize > 0 )    // calling reserve causes a segfault if coductivitySize == 0, which happens in the unittests
        mA_th_Conductivity.reserve( rowEntries );
    for ( size_t row = 0; row < conductivitySize; ++row )
    {
        for ( const IndexedValue< T > &iv : conductivityMatrix[row] )
        {
            mA_th_Conductivity.insert( row, iv.mIndex ) = iv.mValue;
        }
    }
    mA_th_Conductivity.makeCompressed();

    for ( size_t i = 0; i < mMatrixDirichlet.size(); ++i )
    {
        mMatrixDirichlet.at( i ).mA_th = 0.0;
        mMatrixDirichlet.at( i ).mC_th = 0.0;
        if ( !dirichletDataVector.at( i ).empty() )
        {
            BOOST_FOREACH ( const TaylorData< T > &data, dirichletDataVector.at( i ) )
            {
                const T conductivity = data.mConductivity * data.mA_cool / data.mDistanceToGridVertex;
                mMatrixDirichlet.at( i ).mA_th += -conductivity;
                mMatrixDirichlet.at( i ).mC_th += data.mTempSurfLastStep * conductivity;
            }
        }
    }

    mThermalElementFactors.resize( mOdeSystemSize );
    for ( size_t i = 0; i < mOdeSystemSize; ++i )
        mThermalElementFactors[i] =
         1.0 / ( mThermalElements.at( i )->GetVolume() * mThermalElements.at( i )->GetMaterial()->GetDensity() *
                 mThermalElements.at( i )->GetMaterial()->GetSpecificCapacity() );

    for ( size_t i = 0; i < mCoolingDataVector.size(); ++i )
        for ( typename vector< TaylorData< T > >::iterator it = mCoolingDataVector.at( i ).begin();
              it != mCoolingDataVector.at( i ).end(); ++it )
            it->mTempSurfLastStep = airTemperature;

    Update( 0.0, 0.0 );
}

template < typename T >
OdeSystemThermal< T >::~OdeSystemThermal()
{
}

template < typename T >
void OdeSystemThermal< T >::Update( T time, T dt )
{
    for ( size_t i = 0; i < mOdeSystemSize; ++i )
        UpdateLoop( i, time, dt );
}

template < typename T >
void OdeSystemThermal< T >::operator()( const vector< T > &x, vector< T > &dxdt, const double /* t */ )
{
    Eigen::Map< const Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > > xMap( &x[0], x.size(), 1 );
    Eigen::Map< Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > > dxdtMap( &dxdt[0], x.size(), 1 );
    dxdtMap.noalias() = mA_th_Conductivity * xMap;
    for ( size_t i = 0; i < mOdeSystemSize; ++i )
    {
        T cooling = mMatrixBoundarySource[i].mA_th * x[i];
        cooling += mMatrixBoundarySource[i].mC_th;
        dxdt[i] += cooling;
        dxdt[i] *= mThermalElementFactors[i];
        mThermalElements[i]->SetCoolingValue( cooling );
    }
}

template < typename T >
void OdeSystemThermal< T >::UpdateLoop( size_t i, T time, T dt )
{
    mMatrixBoundarySource[i].mA_th = mMatrixDirichlet[i].mA_th;
    mMatrixBoundarySource[i].mC_th = mMatrixDirichlet[i].mC_th + mThermalElements[i]->GetPowerDissipation( time, dt );
    const T temperature = mThermalElements[i]->GetTemperature();


    for ( typename vector< TaylorData< T > >::iterator it = mCoolingDataVector[i].begin(); it != mCoolingDataVector[i].end(); ++it )
    {
        Linearization< T > offsetSlope( 0.0, 0.0 );
        Linearization< T > alphaSlope( 0.0, 0.0 );
        if ( it->mCooling )
        {
            offsetSlope.Add( it->mCooling->GetOffsetSlope( it->mTempSurfLastStep, mAirTemperature ) );
        }
        else
        {
            if ( temperature > mAirTemperature )
            {
                if ( mRadiation )
                {
                    auto lin = mRadiation->GetOffsetSlope( it->mTempSurfLastStep, mAirTemperature );
                    lin.mOffset *= it->mEmissivity;
                    lin.mSlope *= it->mEmissivity;
                    offsetSlope.Add( lin );
                }
                if ( mConvection[it->mLocation] )
                {
                    if ( !mConvection[it->mLocation]->isForced() )
                    {
                        offsetSlope.Add( mConvection[it->mLocation]->GetOffsetSlope( it->mTempSurfLastStep, it->mCharacteristicLength,
                                                                                     mAirTemperature ) );
                    }
                    else
                    {
                        alphaSlope.Add( mConvection[it->mLocation]->GetOffsetSlope( it->mTempSurfLastStep, it->mCharacteristicLength,
                                                                                    mAirTemperature ) );
                    }
                }
            }
        }

        const T helpFactor =
         it->mA_cool * it->mConductivity / ( it->mDistanceToGridVertex * offsetSlope.mSlope + it->mConductivity );
        const T slope = helpFactor * ( offsetSlope.mSlope + alphaSlope.mSlope );
        const T offset =
         helpFactor * ( offsetSlope.mOffset - offsetSlope.mSlope * it->mTempSurfLastStep - alphaSlope.mOffset * mAirTemperature );
        mMatrixBoundarySource[i].mA_th -= slope;
        mMatrixBoundarySource[i].mC_th -= offset;

        it->mTempSurfLastStep =
         ( it->mConductivity / it->mDistanceToGridVertex * temperature + slope * it->mTempSurfLastStep - offset ) /
         ( slope + it->mConductivity / it->mDistanceToGridVertex );
    }
}

template < typename T >
void OdeSystemThermal< T >::GetTemperatureVector( vector< T > &temperatureVector ) const
{
    temperatureVector.resize( mOdeSystemSize );
    for ( size_t i = 0; i < mThermalElements.size(); ++i )
        temperatureVector[i] = mThermalElements[i]->GetTemperature();
}

template < typename T >
void OdeSystemThermal< T >::SetTemperatureVector( const vector< T > &temperatureVector )
{
    for ( size_t i = 0; i < mOdeSystemSize; ++i )
        mThermalElements[i]->SetTemperature( temperatureVector[i] );
}

template < typename T >
void OdeSystemThermal< T >::ResetAirTemperature( T newAirTemperature )
{
    mAirTemperature = newAirTemperature;
}

template < typename T >
size_t OdeSystemThermal< T >::GetOdeSystemSize() const
{
    return mOdeSystemSize;
}

template < typename T >
const vector< shared_ptr< ThermalElement< T > > > &OdeSystemThermal< T >::GetThermalElements() const
{
    return mThermalElements;
}

template < typename T >
const typename OdeSystemThermal< T >::MatrixT &OdeSystemThermal< T >::GetA_th_Conductivity() const
{
    return mA_th_Conductivity;
}

template < typename T >
const vector< T > &OdeSystemThermal< T >::GetThermalElementFactors() const
{
    return mThermalElementFactors;
}

template < typename T >
const vector< shared_ptr< DefaultConvection< T > > > &OdeSystemThermal< T >::GetConvection() const
{
    return mConvection;
}

template < typename T >
const shared_ptr< Radiation< T > > &OdeSystemThermal< T >::GetRadiation() const
{
    return mRadiation;
}

template < typename T >
const vector< typename OdeSystemThermal< T >::BoundarySourceData > &OdeSystemThermal< T >::GetMatrixDirichlet() const
{
    return mMatrixDirichlet;
}

template < typename T >
const vector< vector< TaylorData< T > > > &OdeSystemThermal< T >::GetCoolingDataVector() const
{
    return mCoolingDataVector;
}
}    // namespace thermal
#endif
