#ifndef _AGING_SIMULATION_
#define _AGING_SIMULATION_

#ifdef BUILD_AGING_SIMULATION

#include "../electrical/twoport.h"
#include "../xmlparser/tinyxml2/xmlparameterimpl.h"
#include "../xmlparser/tinyxml2/xmlparserimpl.h"
#include <boost/shared_ptr.hpp>

namespace aging
{
template < typename MatrixType >
class AgingTwoPort;
}

namespace factory
{
template < typename MatrixT, typename ValueT >
struct FactoryBuilderForAging;
}

namespace state
{
class AgingState;
}

namespace observer
{
template < typename T >
class AgingObserver;
}

namespace simulation
{
template < typename Matrix, typename T, bool filterTypeChoice >
class ElectricalSimulation;
template < typename Matrix, typename T, bool filterTypeChoice >
class ThermalSimulation;

template < typename Matrix, typename T, bool filterTypeChoice >
class AgingSimulation
{

    public:
    AgingSimulation( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode,
                     boost::shared_ptr< simulation::ElectricalSimulation< Matrix, T, filterTypeChoice > > electricalSimulation,
                     boost::shared_ptr< simulation::ThermalSimulation< Matrix, T, filterTypeChoice > > thermalSimulation,
                     const std::vector< boost::shared_ptr< electrical::TwoPort< Matrix > > > &cells, double agingStepTime,
                     factory::FactoryBuilderForAging< Matrix, T > *factoryBuilder, const std::string &uuid = "" );
    std::vector< boost::shared_ptr< state::AgingState > > GetAgingStates() const;
    void ResetObservers();
    void ResetThElStates();
    void CollectCellAgingData();
    void ResetToPointInTime( double time );
    void CalculateAging( double timestep, bool scaleToAgingStep = true );
    void CalculateAging( double timestep, double scaleFactor );
    void SetCollectAgingData( bool collectAgingData );
    double GetAgingStepTime() const;
    T mTime;
    boost::shared_ptr< observer::AgingObserver< Matrix > > mAgingObserver;
    boost::shared_ptr< simulation::ElectricalSimulation< Matrix, T, filterTypeChoice > > mElectricalSimulation;
    boost::shared_ptr< simulation::ThermalSimulation< Matrix, T, filterTypeChoice > > mThermalSimulation;
    /// all aging twoports
    std::vector< boost::shared_ptr< aging::AgingTwoPort< Matrix > > > mAgingTwoPorts;
    /// only the aging twoports that work on a cell
    std::vector< boost::shared_ptr< aging::AgingTwoPort< Matrix > > > mCellAgingTwoPorts;

    private:
    T mAgingStepTime;
    T mCycleLength;
    T mSocStopCriterion;
    T mThermalStateStopCriterion;
    T mLastAgingDataTime;
    bool mCollectAgingData;
};

}    // namespace simulation
#endif
#endif
