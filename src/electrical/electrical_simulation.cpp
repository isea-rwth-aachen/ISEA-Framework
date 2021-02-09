#include "electrical_simulation.h"
#include "../misc/matrixInclude.h"

namespace simulation
{

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

    // Create electrical system
    mRootTwoPort = electricalFactory->CreateInstance( rootXmlNode->GetElementChild( "RootElement" ) );
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

    // Evaluate Options node
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = rootXmlNode->GetElementChild( "Options" );

    // SocStopCriterion
    if ( optionsNode->HasElementDirectChild( "SocStopCriterionInPercent" ) )
    {
        mSocStopCriterion = optionsNode->GetElementDoubleValue( "SocStopCriterionInPercent" );
        if ( mSocStopCriterion <= 0.0 )
            ErrorFunction< std::runtime_error >( __FUNCTION__, __LINE__, __FILE__, "SocStopCriterionInPercentNegative" );
    }

    // Give out cells if wanted
    if ( cells )
        *cells = electricalFactory->GetObjectsOfClass( "Cellelement" );

    if ( cells )
    {
        mObserver = CreateTwoPortObserver< std::vector< boost::shared_ptr< ::electrical::TwoPort< Matrix > > >, Matrix, filterTypeChoice >(
         cells, rootXmlNode.get(), 0, 0, 0, 0, 0, mRootTwoPort, uuid );
    }
    else
    {
        mObserver = CreateTwoPortObserver< std::vector< boost::shared_ptr< electrical::TwoPort< Matrix > > >, Matrix, filterTypeChoice >(
         0, rootXmlNode.get(), 0, 0, 0, 0, 0, mRootTwoPort, uuid );
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
#if defined( _ARMADILLO_ ) && defined( SPARSE_MATRIX_FORMAT )
    mStateSystemGroup.ResetSystem();
#endif
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
bool ElectricalSimulation< Matrix, T, filterTypeChoice >::CheckIfSimulationTimeHasEnded()
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
bool ElectricalSimulation< Matrix, T, filterTypeChoice >::IsStopCriterionFulfilled() const
{
    bool isStop = false;
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
        mSavedSocValues[i] = mSocStates[i]->template GetValue< state::SocGetFormat::AS >();
        mSavedSocOffsets[i] = mSocStates[i]->template GetOffset< state::SocGetFormat::AS >();
    }
}

template < typename Matrix, typename T, bool filterTypeChoice >
void ElectricalSimulation< Matrix, T, filterTypeChoice >::LoadCapacityForLaterReset()
{
    for ( size_t i = 0; i < mSocStates.size(); ++i )
    {
        mSocStates[i]->template SetOffset< state::SocSetFormat::ABSOLUT >( mSavedSocOffsets[i] );
        mSocStates[i]->template SetStoredEnergy< state::SocSetFormat::ABSOLUT >( mSavedSocValues[i] );
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

}    // namespace simulation

template class simulation::ElectricalSimulation< myMatrixType, double, true >;
template class simulation::ElectricalSimulation< myMatrixType, double, false >;