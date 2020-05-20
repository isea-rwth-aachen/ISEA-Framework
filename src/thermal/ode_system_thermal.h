#ifndef _ODE_SYSTEM_THERMAL_
#define _ODE_SYSTEM_THERMAL_

#include <vector>
#include <map>
#include <list>
#include <cmath>
#include <boost/shared_ptr.hpp>
#include "blocks/elements/thermal_element.h"
#include "boundaryConditions/default_convection.h"
#include "boundaryConditions/cooling.h"
#include "thermal_structs.h"
#include "../exceptions/error_proto.h"

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
/*    struct UpdateLoopFunctor: public threading::ThreadedForLoop::LoopFunctorInterface
    {
        UpdateLoopFunctor(OdeSystemThermal<T> &thermalSystem, T time, T dt)
            : mThermalSystem(thermalSystem), mTime(time), mDt(dt) {}
        void Iterate(size_t i)
        {
            mThermalSystem.UpdateLoop(i, mTime, mDt);
        }
        OdeSystemThermal<T> &mThermalSystem;
        T mTime;
        T mDt;
    };
    struct DxdtLoopFunctor: public threading::ThreadedForLoop::LoopFunctorInterface
    {
        DxdtLoopFunctor(OdeSystemThermal<T> &thermalSystem, const vector<T> &x, vector<T> &dxdt)
            : mThermalSystem(thermalSystem), mX(x), mDxdt(dxdt) {}
        void Iterate(size_t i)
        {
            mThermalSystem.DxdtLoop(i, mX, mDxdt);
        }
        OdeSystemThermal<T> &mThermalSystem;
        const vector<T> &mX;
        vector<T> &mDxdt;
    };*/
#endif
    public:
    struct BoundarySourceData
    {
        ALIGNED_TO_64_BYTE T mA_th;    // Slope
        ALIGNED_TO_64_BYTE T mC_th;    // Offset
    };
    /**
     * @param[in] dirichletDataVector Vector containing the data for all dirichlet boundary conditions
     * @param[in] coolingDataVector Vector containing the data for all coolings
     * @param[in] defaultConvection Vector with default convections mapped by Location enum
     * @param[in] thermalStates,unconnectedThermalStates,materials,coolings These need to be present during simulation
     * and may be stored here if desired
     */
    OdeSystemThermal( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                      vector< vector< IndexedValue< T > > > &conductivityMatrix, vector< vector< TaylorData< T > > > &coolingDataVector,
                      const vector< vector< TaylorData< T > > > &dirichletDataVector,
                      const vector< shared_ptr< DefaultConvection< T > > > &defaultConvection,
                      const shared_ptr< Radiation< T > > defaultRadiation, T airTemperature,
                      vector< shared_ptr< ::state::ThermalState< T > > > thermalStates,
                      vector< shared_ptr< ::state::ThermalState< T > > > unconnectedThermalStates =
                       vector< shared_ptr< ::state::ThermalState< T > > >(),
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
    /// One iteration of loop used in operator()
    void DxdtLoop( size_t i, const vector< T > &x, vector< T > &dxdt );
    void GetTemperatureVector( vector< T > &temperatureVector ) const;
    void SetTemperatureVector( const vector< T > &temperatureVector );
    void ResetAirTemperature( T newAirTemperature );
    inline size_t GetOdeSystemSize() const;
    const vector< shared_ptr< ThermalElement< T > > > &GetThermalElements() const;
    const vector< vector< IndexedValue< T > > > &GetA_th_Conductivity() const;
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
    vector< vector< IndexedValue< T > > > mA_th_Conductivity;
    vector< vector< TaylorData< T > > > mCoolingDataVector;
    const vector< shared_ptr< DefaultConvection< T > > > mConvection;
    const shared_ptr< Radiation< T > > mRadiation;
    T mAirTemperature;

    vector< BoundarySourceData > mMatrixBoundarySource;    ///<Stores values for heat generation and cooling created by Update()
    vector< BoundarySourceData > mMatrixDirichlet;    ///<Stores values for dirichlet boundary condition
    vector< T > mThermalElementFactors;               ///<Stores reciprocals of the total capacity of thermal elements
    const size_t mOdeSystemSize;

    vector< shared_ptr< ::state::ThermalState< T > > > mThermalStates;
    vector< shared_ptr< ::state::ThermalState< T > > > mUnconnectedThermalStates;
    vector< shared_ptr< Material< T > > > mMaterials;
    vector< shared_ptr< Cooling< T > > > mCoolings;

#ifndef _MATLAB_
// Threading
// shared_ptr<threading::ThreadedForLoop> mThreadedForLoop;
#endif
};


template < typename T >
OdeSystemThermal< T >::OdeSystemThermal( vector< shared_ptr< ThermalElement< T > > > &thermalElements,
                                         vector< vector< IndexedValue< T > > > &conductivityMatrix,
                                         vector< vector< TaylorData< T > > > &coolingDataVector,
                                         const vector< vector< TaylorData< T > > > &dirichletDataVector,
                                         const vector< shared_ptr< DefaultConvection< T > > > &defaultConvection,
                                         const shared_ptr< Radiation< T > > defaultRadiation, T airTemperature,
                                         vector< shared_ptr< ::state::ThermalState< T > > > thermalStates,
                                         vector< shared_ptr< ::state::ThermalState< T > > > unconnectedThermalStates,
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
#ifndef _MATLAB_
//    , mThreadedForLoop(new threading::ThreadedForLoop())
#endif
{
    if ( thermalElements.size() != conductivityMatrix.size() || thermalElements.size() != coolingDataVector.size() ||
         thermalElements.size() != dirichletDataVector.size() )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "thermalElementEqualSize" );

    mThermalElements.swap( thermalElements );
    mA_th_Conductivity.swap( conductivityMatrix );
    mCoolingDataVector.swap( coolingDataVector );

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
    //#ifndef _MATLAB_
    //    UpdateLoopFunctor loopFunctor(*this, time, dt);
    //    mThreadedForLoop->DoLoop(loopFunctor, mOdeSystemSize);
    //#else
    for ( size_t i = 0; i < mOdeSystemSize; ++i )
        UpdateLoop( i, time, dt );
    //#endif
}

template < typename T >
void OdeSystemThermal< T >::operator()( const vector< T > &x, vector< T > &dxdt, const double /* t */ )
{
    //#ifndef _MATLAB_
    //    DxdtLoopFunctor loopFunctor(*this, x, dxdt);
    //    mThreadedForLoop->DoLoop(loopFunctor, mOdeSystemSize);
    //#else
    for ( size_t i = 0; i < mOdeSystemSize; ++i )
        DxdtLoop( i, x, dxdt );
    //#endif
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
void OdeSystemThermal< T >::DxdtLoop( size_t i, const vector< T > &x, vector< T > &dxdt )
{
    dxdt[i] = 0.0;

    for ( typename vector< IndexedValue< T > >::const_iterator it = mA_th_Conductivity[i].begin();
          it != mA_th_Conductivity[i].end(); ++it )
    {
        // TODO: Given that mA_th_Conductivity is a symmetric matrix, consider making it a half matrix and optimize
        // its
        // evaluation here
        dxdt[i] += it->mValue * x[it->mIndex];
    }

    const T cooling = mMatrixBoundarySource[i].mA_th * x[i] + mMatrixBoundarySource[i].mC_th;

    dxdt[i] += cooling;
    dxdt[i] *= mThermalElementFactors[i];
    mThermalElements[i]->SetCoolingValue( cooling );
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
const vector< vector< IndexedValue< T > > > &OdeSystemThermal< T >::GetA_th_Conductivity() const
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
}
#endif
