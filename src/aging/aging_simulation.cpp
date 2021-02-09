#include "aging_simulation.h"
#include "../electrical/electrical_simulation.h"
#include "../factory/factorybuilder_for_aging.h"
#include "../factory/observer/createObserver.h"
#include "../thermal/thermal_simulation.h"

#ifdef BUILD_AGING_SIMULATION

namespace simulation
{

template < typename Matrix, typename T, bool filterTypeChoice >
AgingSimulation< Matrix, T, filterTypeChoice >::AgingSimulation(
 const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode,
 boost::shared_ptr< simulation::ElectricalSimulation< Matrix, T, filterTypeChoice > > electricalSimulation,
 boost::shared_ptr< simulation::ThermalSimulation< Matrix, T, filterTypeChoice > > thermalSimulation,
 const std::vector< boost::shared_ptr< electrical::TwoPort< Matrix > > > & /*cells*/, double agingStepTime,
 factory::FactoryBuilderForAging< Matrix, T > *factoryBuilder, const std::string &uuid )
    : mTime( 0.0 )
    , mElectricalSimulation( electricalSimulation )
    , mThermalSimulation( thermalSimulation )
    , mAgingStepTime( agingStepTime )
    , mCycleLength( electricalSimulation->mSimulationDuration )
    , mLastAgingDataTime( 0.0 )
    , mCollectAgingData( true )
{
    mCellAgingTwoPorts = factoryBuilder->mAgingTwoPortFactory->GetObjectsOfClass( "CellElement" );
    mAgingTwoPorts.reserve( factoryBuilder->mAgingTwoPortFactory->GetNumberOfObjects() );
    for ( const auto &cellAgingTp : mCellAgingTwoPorts )
    {
        mAgingTwoPorts.push_back( cellAgingTp );
        for ( const auto &child : cellAgingTp->GetChildren() )
        {
            mAgingTwoPorts.push_back( child );
        }
    }

    mAgingObserver =
     CreateAgingObserver< std::vector< boost::shared_ptr< aging::AgingTwoPort< Matrix > > >, Matrix, filterTypeChoice >(
      mAgingTwoPorts, rootXmlNode.get(), uuid );
}

template < typename Matrix, typename T, bool filterTypeChoice >
void AgingSimulation< Matrix, T, filterTypeChoice >::CollectCellAgingData()
{
    if ( !mCollectAgingData )
        return;

    double timestep = mTime - mLastAgingDataTime;
    for ( auto &agingTwoPort : mCellAgingTwoPorts )
    {
        agingTwoPort->CollectData( timestep );
    }
    mLastAgingDataTime = mTime;
}

template < typename Matrix, typename T, bool filterTypeChoice >
void AgingSimulation< Matrix, T, filterTypeChoice >::ResetToPointInTime( double time )
{
    for ( auto &agingTwoPort : mCellAgingTwoPorts )
    {
        agingTwoPort->ResetToPointInTime( time );
    }
}

template < typename Matrix, typename T, bool filterTypeChoice >
void AgingSimulation< Matrix, T, filterTypeChoice >::CalculateAging( double timestep, bool scaleToAgingStep )
{
    double scaleFactor = 1.0;
    if ( scaleToAgingStep )
        scaleFactor = mAgingStepTime * 3600 * 24 / timestep;

    for ( auto &agingTwoPort : mCellAgingTwoPorts )
    {
        agingTwoPort->CalculateAging( timestep, scaleFactor );
    }
}

template < typename Matrix, typename T, bool filterTypeChoice >
void AgingSimulation< Matrix, T, filterTypeChoice >::CalculateAging( double timestep, double scaleFactor )
{
    for ( auto &agingTwoPort : mCellAgingTwoPorts )
    {
        agingTwoPort->CalculateAging( timestep, scaleFactor );
    }
}

template < typename Matrix, typename T, bool filterTypeChoice >
void AgingSimulation< Matrix, T, filterTypeChoice >::ResetThElStates()
{
    mElectricalSimulation->ResetSystemStates();
    mElectricalSimulation->ResetSocStates();
    mElectricalSimulation->UpdateSystemValues();
    mThermalSimulation->ResetThermalElementTemperatures();
    mThermalSimulation->UpdateAllThermalStatesTemperatures();
}

template < typename Matrix, typename T, bool filterTypeChoice >
void AgingSimulation< Matrix, T, filterTypeChoice >::SetCollectAgingData( bool collectAgingData )
{
    mCollectAgingData = collectAgingData;
    if ( mCollectAgingData )
        mLastAgingDataTime = mTime;
}

template < typename Matrix, typename T, bool filterTypeChoice >
double AgingSimulation< Matrix, T, filterTypeChoice >::GetAgingStepTime() const
{
    return mAgingStepTime;
}
}    // namespace simulation

template class simulation::AgingSimulation< myMatrixType, double, true >;
template class simulation::AgingSimulation< myMatrixType, double, false >;
#endif
