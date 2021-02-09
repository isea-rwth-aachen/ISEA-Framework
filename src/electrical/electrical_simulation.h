#ifndef _ELECTRICAL_SIMULATION_
#define _ELECTRICAL_SIMULATION_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../factory/factorybuilder.h"

#include "../system/dae_sys.h"

#include "../exceptions/error_proto.h"
#include "../factory/observer/createObserver.h"
#include "../observer/filter/csvfilter.h"
#include "../observer/filter/filter.h"
#include "../observer/observer.h"
#include "../observer/observerException.h"

namespace thermal
{
extern const double globalMaxValue;
}

class TestSimulation;

namespace simulation
{

template < typename Matrix, typename T, bool filterTypeChoice >
class ElectricalSimulation
{
    friend class ::TestSimulation;

    public:
    /**
     * @param[in] rootXmlNode Root node of xml-file to parse
     * @param[in] maxSimulationStepDuration Recalculation of equation system is triggered if maxSimulationStepDuration
     * seconds have been passed since last calculation, forcing a new simulation step
     * @param[in] simulationDuration Duration of the whole simulation in sec
     * @param[in] socStopCriterion Recalculation of equation system is triggered if the SoC of any cell element has
     * changed by socStopCriterion percent
     * @param[out] cells Returns all created cells if it is not null
     */
    ElectricalSimulation( const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, T maxSimulationStepDuration,
                          T simulationDuration, double socStopCriterion,
                          std::vector< boost::shared_ptr< ::electrical::TwoPort< Matrix > > > *cells,
                          factory::FactoryBuilder< Matrix, T > *factoryBuilder = nullptr, const std::string &uuid = "" );
    /// Updates electrical equation system
    void UpdateSystem();
    /// Updates physical quantities in electrical two ports
    void UpdateSystemValues();
    /// Returns true if current simulation step needs to end, if so it resets mDeltaTime for the next simulation step
    bool CheckLoopConditionAndSetDeltaTime( T currentChangeTime );
    /// Returns true if mTime has exceeded mSimulationDuration
    bool CheckIfSimulationTimeHasEnded();
    /// Gets the time passed since mLoopStartTime (mLoopStartTime is the start time of this simulation step)
    T GetCurrentSimulationStepTime() const;
    /// Executes actions needed at the end of a simulation step
    void FinshStep();
    /// Saves power and time of all cell elements into their thermal states
    void UpdateAllThermalStatesPowerDissipation();
    /// Saves power of all cell elements into their thermal states
    void UpdateAllThermalStatesPowerDissipationWithoutTime();
    /// Clears power dissipation of all thermal states
    void ResetAllThermalStatesPowerDissipation();
    /// Initializes stop criterions and sets the step start time
    void StartNewTimeStep();
    /// Returns true if SoC or power stop criterion is met
    bool IsStopCriterionFulfilled() const;
    /// Saves states if stop criterion of thermal simulation is triggered
    void SaveStatesForLaterReset();
    /// Resets states and SoCs to states saved at a certain point of time
    void ResetStatesToPointOfTime( T time );
    /// Resets all SoC states to their initial value
    void ResetSocStates();
    /// Resets all system states to zero
    void ResetSystemStates();
    /// Sets a maximul deviation in total power generation in percent. 0 means no stop criterion.
    void SetPowerStopCriterion( T maximumDeviation );

    void UpdateSimulationDuration( const T &simulationDuration ) { mSimulationDuration = simulationDuration; }

    void LoadCapacityForLaterReset();
    void SaveCapacityForLaterReset();
    std::vector< T > mSavedSocValues;
    std::vector< T > mSavedSocOffsets;

    // Internal data
    boost::shared_ptr< electrical::TwoPort< Matrix > > mRootTwoPort;
    std::vector< boost::shared_ptr< ::electrical::Cellelement< Matrix > > > mCellElements;
    std::vector< boost::shared_ptr< state::Soc > > mSocStates;
    std::vector< boost::shared_ptr< state::ThermalState< double > > > mThermalStates;
    // Electrical system with states
    boost::shared_ptr< systm::DifferentialAlgebraicSystem< Matrix > > mEqSystem;
    boost::shared_ptr< observer::TwoPortObserver< Matrix > > mObserver;
    // boost::scoped_ptr<systm::DifferentialAlgebraicSystem< Matrix > > mEqSystem;
    systm::StateSystemGroup< myMatrixType > mStateSystemGroup;
    // Simulation times
    T mTime;
    T mDeltaTime;
    T mSimulationDuration;
    size_t mNumberOfObjects;    // number of instances the object factory has created

    private:
    T mLastUnconstrainedDeltaTime;
    T mMaxSimulationStepDuration;
    T mStepStartTime;
    // Soc stop criterion
    double mSocStopCriterion;
    // Stop criterion if following a power profile
    double mPowerStopCriterion;
    std::vector< std::vector< T > > mSocValuesSteps;
    // If electrical is supposed to be resettable
    std::vector< Matrix > mStatesSteps;
    std::vector< T > mTimeSteps;
    size_t mNumberOfSteps;
    // Power value of the root port at the beginning of the time step
    T mInitialPowerValue;
};

}    // namespace simulation
#endif
