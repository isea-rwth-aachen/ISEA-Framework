#ifndef _ELECTRICAL_SIMULATION_
#define _ELECTRICAL_SIMULATION_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../factory/factorybuilder.h"

#include "../system/dae_sys.h"

#include "../electrical/electrical_simulation.h"
#include "../exceptions/error_proto.h"
#include "../factory/factorybuilder_for_aging.h"
#include "../factory/observer/createObserver.h"
#include "../observer/filter/csvfilter.h"
#include "../observer/filter/filter.h"
#include "../observer/observer.h"
#include "../observer/observerException.h"
#include "../thermal/thermal_simulation.h"

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
    bool CheckIfSimulationTimeHasEnded() const;
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
    ///  Checks true if any stop criterion is met
    bool IsStopCriterionFulfilled() const;
    /// Returns true if SoC or power stop criterion is met
    bool IsStopThermalCriterionFulfilled() const;
    /// Returns true if voltage limits of cell elements are exceeded
    bool IsStopElectricalCriterionFulfilled() const;
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
    /// Set limit for cell voltage. Steps in the current profile are skipped if the limits are exceeded.
    void SetVoltageStopCriterion( T minimumVoltage, T maximumVoltage );

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
    // Voltage stop criterion
    double mMinVoltageStopCriterion;
    double mMaxVoltageStopCriterion;

    size_t mVoltageLimitDelay;
    size_t mVoltageLimitCount;

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

template < typename Matrix, typename T, bool filterTypeChoice >
ElectricalSimulation< Matrix, T, filterTypeChoice >::ElectricalSimulation(
 const boost::shared_ptr< xmlparser::XmlParameter > &rootXmlNode, T maxSimulationStepDuration, T simulationDuration,
 double socStopCriterion, std::vector< boost::shared_ptr< ::electrical::TwoPort< Matrix > > > *cells,
 factory::FactoryBuilder< Matrix, T > *factoryBuilder, const std::string &uuid )
    : mTime( 0.0 )
    , mDeltaTime( maxSimulationStepDuration )
    , mSimulationDuration( simulationDuration )
    , mLastUnconstrainedDeltaTime( maxSimulationStepDuration )
    , mMaxSimulationStepDuration( maxSimulationStepDuration )
    , mStepStartTime( 0.0 )
    , mSocStopCriterion( socStopCriterion )
    , mMinVoltageStopCriterion( nan( "" ) )
    , mMaxVoltageStopCriterion( nan( "" ) )
    , mVoltageLimitDelay( 4 )
    , mPowerStopCriterion( 0.0 )
    , mNumberOfSteps( 0 )
{
    // if no FactoryBuilder is passed, create a local one in this scope
    boost::scoped_ptr< factory::FactoryBuilder< Matrix, T > > scopedFactoryBuilder;
    if ( !factoryBuilder )
    {
        scopedFactoryBuilder.reset( new factory::FactoryBuilder< Matrix, T >() );
        factoryBuilder = scopedFactoryBuilder.get();
    }

    factory::Factory< state::State, factory::ArgumentTypeState > *stateFactory = factoryBuilder->BuildStateFactory();
    factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > *objectFactory =
     factoryBuilder->BuildObjectFactory();
    factory::Factory< electrical::TwoPort< Matrix >, factory::ArgumentTypeElectrical > *electricalFactory =
     factoryBuilder->BuildElectricalFactory();

    // Evaluate Options node
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = rootXmlNode->GetElementChild( "Options" );
    bool observeAllElectricalElements = false;
    if ( optionsNode->HasElementDirectChild( "ObserveAllElectricalElements" ) )
        observeAllElectricalElements = optionsNode->GetElementBoolValue( "ObserveAllElectricalElements" );

    factory::ArgumentTypeElectrical factoryArg;
    factoryArg.mMakeAllECMElementsObservable = observeAllElectricalElements;
    mRootTwoPort = electricalFactory->CreateInstance( rootXmlNode->GetElementChild( "RootElement" ), &factoryArg );

    if ( cells )
    {
        *cells = electricalFactory->GetObjectsOfClass( "Cellelement" );
        std::vector< boost::shared_ptr< electrical::TwoPort< Matrix > > > observableTwoports = *cells;
        FindObservableTwoports( observableTwoports, mRootTwoPort );
        mObserver = CreateTwoPortObserver< std::vector< boost::shared_ptr< ::electrical::TwoPort< Matrix > > >, Matrix, filterTypeChoice >(
         &observableTwoports, rootXmlNode.get(), 0, 0, 0, 0, 0, mRootTwoPort, uuid );
    }
    else
    {
        mObserver = CreateTwoPortObserver< std::vector< boost::shared_ptr< electrical::TwoPort< Matrix > > >, Matrix, filterTypeChoice >(
         0, rootXmlNode.get(), 0, 0, 0, 0, 0, mRootTwoPort, uuid );
    }

    // Create electrical system
#ifdef _SYMBOLIC_
    mRootTwoPort->SetID( 0 );
    size_t iDCounter = 1;

    std::vector< boost::shared_ptr< electrical::TwoPort< Matrix > > > electricalElements;
    electricalFactory->GetObjects( electricalElements );
    for ( size_t i = 0; i < electricalElements.size(); ++i )
        if ( electricalElements.at( i ).get() != mRootTwoPort.get() )
            electricalElements.at( i )->SetID( iDCounter++ );

    try
    {
        BOOST_FOREACH ( boost::shared_ptr< electrical::TwoPort< Matrix > > &elem, electricalFactory->GetObjectsOfClass( "WarburgCotanh" ) )
        {
            SerialTwoPort< Matrix > *serialTwoPort = dynamic_cast< SerialTwoPort< Matrix > * >( elem.get() );
            for ( size_t i = 0; i < serialTwoPort->size(); ++i )
                serialTwoPort->at( i )->SetID( iDCounter++ );
        }
    }
    catch ( ... )
    {
    }
    try
    {
        BOOST_FOREACH ( boost::shared_ptr< electrical::TwoPort< Matrix > > &elem, electricalFactory->GetObjectsOfClass( "WarburgTanh" ) )
        {
            SerialTwoPort< Matrix > *serialTwoPort = dynamic_cast< SerialTwoPort< Matrix > * >( elem.get() );
            for ( size_t i = 0; i < serialTwoPort->size(); ++i )
                serialTwoPort->at( i )->SetID( iDCounter++ );
        }
    }
    catch ( ... )
    {
    }
    try
    {
        BOOST_FOREACH ( boost::shared_ptr< electrical::TwoPort< Matrix > > &elem, electricalFactory->GetObjectsOfClass( "Rmphn" ) )
        {
            SerialTwoPort< Matrix > *serialTwoPort = dynamic_cast< SerialTwoPort< Matrix > * >( elem.get() );
            for ( size_t i = 0; i < serialTwoPort->size(); ++i )
                serialTwoPort->at( i )->SetID( iDCounter++ );
        }
    }
    catch ( ... )
    {
    }
    for ( size_t i = 0; i < this->mStateSystemGroup.GetStateCount(); ++i )
        this->mStateSystemGroup.mStateVector.coeffRef( i, 0 ) = ScalarUnit( misc::StrCont( "State" ) + misc::StrCont( i + 1 ) );

#endif
    mNumberOfObjects = objectFactory->GetNumberOfObjects();
    mRootTwoPort->SetSystem( &mStateSystemGroup );
    mStateSystemGroup.Initialize();
#ifndef _SYMBOLIC_
    mRootTwoPort->SetInitialCurrent( 0.0 );
#else
    mRootTwoPort->SetInitialCurrent( ScalarUnit( "InputCurrent" ) );
#endif
    mRootTwoPort->UpdateStateSystemGroup();
    mEqSystem = boost::make_shared< systm::DifferentialAlgebraicSystem< Matrix > >( &mStateSystemGroup );

    // Store thermal states, SoC states and cell elements from electrical state factory
    mThermalStates.reserve( stateFactory->GetObjectsOfClass( "ThermalState" ).size() );
    BOOST_FOREACH ( const boost::shared_ptr< state::State > &thermalStateFromFactory, stateFactory->GetObjectsOfClass( "ThermalState" ) )
        mThermalStates.push_back( boost::static_pointer_cast< state::ThermalState< double >, state::State >( thermalStateFromFactory ) );

    const size_t numberOfSocObject = stateFactory->GetObjectsOfClass( "Soc" ).size();
    mSocStates.reserve( numberOfSocObject );
    mSavedSocValues.resize( numberOfSocObject );
    mSavedSocOffsets.resize( numberOfSocObject );

    BOOST_FOREACH ( const boost::shared_ptr< state::State > &socStateFromFactory,
                    stateFactory->GetObjectsOfClass( "Soc" ) )
        mSocStates.push_back( boost::static_pointer_cast< state::Soc, state::State >( socStateFromFactory ) );

    mCellElements.reserve( electricalFactory->GetObjectsOfClass( "CellElement" ).size() );
    BOOST_FOREACH ( const boost::shared_ptr< ::electrical::TwoPort< Matrix > > &twoPortFromFactory,
                    electricalFactory->GetObjectsOfClass( "CellElement" ) )
    {
        if ( twoPortFromFactory->IsCellelement() )
            mCellElements.push_back(
             boost::static_pointer_cast< ::electrical::Cellelement< Matrix >, ::electrical::TwoPort< Matrix > >( twoPortFromFactory ) );
        else
        {
            ::electrical::ParallelTwoPort< Matrix > *cellFromFactory =
             static_cast< ::electrical::ParallelTwoPort< Matrix > * >( twoPortFromFactory.get() );
            for ( size_t i = 0; i < cellFromFactory->GetParallelChildren(); ++i )
                mCellElements.push_back(
                 boost::static_pointer_cast< ::electrical::Cellelement< Matrix > >( cellFromFactory->shared_at( i ) ) );
        }
    }

    // SocStopCriterion
    if ( optionsNode->HasElementDirectChild( "SocStopCriterionInPercent" ) )
    {
        mSocStopCriterion = optionsNode->GetElementDoubleValue( "SocStopCriterionInPercent" );
        if ( mSocStopCriterion <= 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SocStopCriterionInPercentNegative" );
    }
    // VoltageStopCriterion
    if ( optionsNode->HasElementDirectChild( "MinimalOperationalCellVoltageV" ) )
    {
        mMinVoltageStopCriterion = optionsNode->GetElementDoubleValue( "MinimalOperationalCellVoltageV" );
        if ( mMinVoltageStopCriterion < 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "MinimalVoltageCriterionIsNegative" );
    }
    if ( optionsNode->HasElementDirectChild( "MaximalOperationalCellVoltageV" ) )
    {
        mMaxVoltageStopCriterion = optionsNode->GetElementDoubleValue( "MaximalOperationalCellVoltageV" );
        if ( mMaxVoltageStopCriterion < 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "MinimalVoltageCriterionIsNegative" );
    }
    if ( optionsNode->HasElementDirectChild( "VerificationDelayForVoltageLimitSteps" ) )
    {
        mVoltageLimitDelay = optionsNode->GetElementDoubleValue( "VerificationDelayForVoltageLimitSteps" );
        if ( mVoltageLimitDelay < 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                                 "VerificationDelayForVoltageLimitSteps" );
    }
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::UpdateSystem()
{
    // Current must be set before this function
    mRootTwoPort->UpdateStateSystemGroup();
    mEqSystem->PrepareEquationSystem();
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::UpdateSystemValues()
{
    mRootTwoPort->CalculateStateDependentValues();
}

template < typename Matrix, typename T, bool filterTypeChoice >
bool ElectricalSimulation< Matrix, T, filterTypeChoice >::CheckLoopConditionAndSetDeltaTime( T currentChangeTime )
{
    if ( mTime < mStepStartTime + mMaxSimulationStepDuration && mTime < currentChangeTime )
    {
        mLastUnconstrainedDeltaTime = mDeltaTime;
        mDeltaTime = std::min( currentChangeTime - mTime, mLastUnconstrainedDeltaTime );
        mDeltaTime = std::min( mStepStartTime + mMaxSimulationStepDuration - mTime, mDeltaTime );
        return true;
    }

    return false;
}

template < typename Matrix, typename T, bool filterTypeChoice >
bool ElectricalSimulation< Matrix, T, filterTypeChoice >::CheckIfSimulationTimeHasEnded() const
{
    return mTime >= mSimulationDuration;
}

template < typename Matrix, typename T, bool filterTypeChoice >
T ElectricalSimulation< Matrix, T, filterTypeChoice >::GetCurrentSimulationStepTime() const
{
    return mTime - mStepStartTime;
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::FinshStep()
{
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::UpdateAllThermalStatesPowerDissipation()
{
    BOOST_FOREACH ( boost::shared_ptr< electrical::Cellelement< Matrix > > &elem, mCellElements )
        elem->UpdateThermalStatePowerDissipation( mTime );
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::UpdateAllThermalStatesPowerDissipationWithoutTime()
{
    BOOST_FOREACH ( boost::shared_ptr< electrical::Cellelement< Matrix > > &elem, mCellElements )
        elem->UpdateThermalStatePowerDissipation();
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::ResetAllThermalStatesPowerDissipation()
{
    BOOST_FOREACH ( boost::shared_ptr< electrical::Cellelement< Matrix > > &elem, mCellElements )
        elem->ResetThermalStatePowerDissipation();
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::StartNewTimeStep()
{
    ResetAllThermalStatesPowerDissipation();
    mStepStartTime = mTime;
    mNumberOfSteps = 0;
    SaveStatesForLaterReset();
#ifndef _SYMBOLIC_
    mInitialPowerValue = mRootTwoPort->GetCurrentValue() * mRootTwoPort->GetVoltageValue();
#endif
}


template < typename Matrix, typename T, bool filterTypeChoice >
bool ElectricalSimulation< Matrix, T, filterTypeChoice >::IsStopElectricalCriterionFulfilled() const
{
    bool isStop = false;
#ifndef _SYMBOLIC_
    if ( mNumberOfSteps > mVoltageLimitDelay && !isnan( mMinVoltageStopCriterion ) && !isnan( mMaxVoltageStopCriterion ) )
    {
        // Check voltage
        for ( size_t i = 0; i < mCellElements.size(); ++i )
        {
            double cellVoltage = mCellElements[i]->GetVoltageValue();
            if ( ( cellVoltage > mMaxVoltageStopCriterion || cellVoltage < mMinVoltageStopCriterion ) &&
                 mRootTwoPort->GetCurrentValue() != 0 )
            {
                isStop = true;
                break;
            }
        }
    }
#endif
    return isStop;
}


template < typename Matrix, typename T, bool filterTypeChoice >
bool ElectricalSimulation< Matrix, T, filterTypeChoice >::IsStopThermalCriterionFulfilled() const
{
    bool isStop = false;

    // Check SOC Difference
    for ( size_t i = 0; i < mSocStates.size(); ++i )
    {
        if ( fabs( mSocValuesSteps[0][i] - mSocStates[i]->GetValue() ) > mSocStopCriterion )
        {
            isStop = true;
            break;
        }
    }


#ifndef _SYMBOLIC_
    if ( mPowerStopCriterion > 0.0 )
    {
        double power = mRootTwoPort->GetCurrentValue() * mRootTwoPort->GetVoltageValue();
        double powerDeviation = ( std::abs( mInitialPowerValue - power ) / mInitialPowerValue ) - 1;
        if ( powerDeviation > mPowerStopCriterion )
            isStop = true;
    }
#endif

    return isStop;
}

template < typename Matrix, typename T, bool filterTypeChoice >
bool ElectricalSimulation< Matrix, T, filterTypeChoice >::IsStopCriterionFulfilled() const
{
    return ( IsStopElectricalCriterionFulfilled() || IsStopThermalCriterionFulfilled() );
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::SaveStatesForLaterReset()
{
    if ( mTimeSteps.size() == mNumberOfSteps )
        mTimeSteps.resize( mNumberOfSteps + 1 );
    if ( mStatesSteps.size() == mNumberOfSteps )
        mStatesSteps.resize( mNumberOfSteps + 1 );
    if ( mSocValuesSteps.size() == mNumberOfSteps )
    {
        mSocValuesSteps.resize( mNumberOfSteps + 1 );
        mSocValuesSteps[mNumberOfSteps].resize( mSocStates.size() );
    }

    mTimeSteps[mNumberOfSteps] = mTime;
    mStatesSteps[mNumberOfSteps] = mStateSystemGroup.mStateVector;
    for ( size_t i = 0; i < mSocStates.size(); ++i )
        mSocValuesSteps[mNumberOfSteps][i] = mSocStates[i]->GetValue();

    mNumberOfSteps++;
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::ResetStatesToPointOfTime( T time )
{
    if ( time <= mTimeSteps[0] || time > mTimeSteps[mNumberOfSteps - 1] || mNumberOfSteps < 2 )
        ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__,
                                             "errorResetStatesToPointOfTimeExecution" );
    if ( time == mTimeSteps[mNumberOfSteps - 1] )    // No need to reset if thermal stop criterion is triggered after
                                                     // full step time time has been simulated
        return;

    // Get index, which is the index for the vectors mStateSystemGroup and mSocValuesSteps before time
    size_t index = 0;
    for ( size_t i = 1; i < mNumberOfSteps; ++i )
        if ( mTimeSteps[i] > time )
        {
            index = i - 1;
            break;
        }

    // Calculate factor1 and factor2, which are used to scale linearly between the point of time for index and (index+1)
    const T deltaTime = mTimeSteps[index + 1] - mTimeSteps[index];
    const T factor1 = ( mTimeSteps[index + 1] - time ) / deltaTime;
    const T factor2 = ( time - mTimeSteps[index] ) / deltaTime;

    // Reset time, states and SoCs
    mTime = time;
    mStateSystemGroup.mStateVector = mStatesSteps[index] * factor1 + mStatesSteps[index] * factor2;
    for ( size_t i = 0; i < mSocStates.size(); ++i )
    {
        const T newSocValue = mSocValuesSteps[index][i] * factor1 + mSocValuesSteps[index + 1][i] * factor2;
        const T deltaCapacityASec =
         ( newSocValue - mSocValuesSteps[mNumberOfSteps - 1][i] ) / 100.0 * mSocStates[i]->GetActualCapacity();
        mSocStates[i]->SetStoredEnergy< state::SocSetFormat::DELTA >( deltaCapacityASec );
    }
    UpdateSystemValues();
    UpdateSystem();

    BOOST_FOREACH ( boost::shared_ptr< state::ThermalState< double > > &thermalState, mThermalStates )
        thermalState->ResetPowerDissipationToTime( time );
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::SaveCapacityForLaterReset()
{
    for ( size_t i = 0; i < mSocStates.size(); ++i )
    {
        mSavedSocValues[i] = mSocStates[i]->template GetValue< state::SocGetFormat::PERCENT >();
        mSavedSocOffsets[i] = mSocStates[i]->template GetOffset< state::SocGetFormat::AS >();
    }
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::LoadCapacityForLaterReset()
{
    for ( size_t i = 0; i < mSocStates.size(); ++i )
    {
        mSocStates[i]->template SetOffset< state::SocSetFormat::ABSOLUT >( mSavedSocOffsets[i] );
        mSocStates[i]->template SetStoredEnergy< state::SocSetFormat::FACTOR >( mSavedSocValues[i] / 100 );
    }
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::ResetSocStates()
{
    for ( size_t i = 0; i < mCellElements.size(); ++i )
        mCellElements[i]->ResetSocValue();
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::ResetSystemStates()
{
    mStateSystemGroup.ResetStates();
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::SetPowerStopCriterion( T maximumDeviation )
{
    mPowerStopCriterion = maximumDeviation / 100;
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::SetVoltageStopCriterion( T minimumVoltage, T maximumVoltage )
{
    mMinVoltageStopCriterion = minimumVoltage;
    mMaxVoltageStopCriterion = maximumVoltage;
}

}    // namespace simulation
#endif
