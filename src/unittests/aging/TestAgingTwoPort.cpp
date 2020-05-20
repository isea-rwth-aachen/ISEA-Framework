#include "TestAgingTwoPort.h"
#include "../../aging/aging_twoport.h"
#include "../../aging/generic_aging.h"

void TestAgingTwoPort::testFactorCalculation()
{
#ifdef BUILD_AGING_SIMULATION
    auto thermalState = boost::shared_ptr< state::ThermalState< double > >( new state::ThermalState< double >( 20.0 ) );
    auto socState = boost::make_shared< electrical::state::Soc >( 10.0, 10.0, 50.0 );
    auto cell = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, socState );
    auto aging1 = boost::make_shared< aging::GenericAging >( 1 );
    auto aging2 = boost::make_shared< aging::GenericAging >( 1 );
    aging::AgingTwoPort< myMatrixType > agingTwoport( {aging1, aging2}, cell, cell );

    assertAllVariables( agingTwoport.GetAgingState(), 1, 0, 1 );
    agingTwoport.CalculateAging( 0.0, 0.0 );
    assertAllVariables( agingTwoport.GetAgingState(), 1, 0, 1 );

    double factor1 = 1.3;
    double factor2 = 1.65;
    double offset = 10000;
    double resistanceFactor = 1.1;

    aging1->SetCapacityFactor( factor1 );
    aging2->SetCapacityFactor( factor2 );
    aging2->SetSocOffset( offset );
    aging2->SetResistanceFactor( resistanceFactor );

    agingTwoport.CalculateAging( 0.0, 0.0 );
    assertAllVariables( agingTwoport.GetAgingState(), 1 + ( factor1 - 1 ) + ( factor2 - 1 ), offset, resistanceFactor );
#endif
}

void TestAgingTwoPort::assertAllVariables( const boost::shared_ptr< state::AgingState > &agingState,
                                           double capacityFactor, double socOffset, double resistanceFactor )
{
    TS_ASSERT_DELTA( agingState->GetCapacityFactor(), capacityFactor, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetSocOffset(), socOffset, 0.0001 );
    TS_ASSERT_DELTA( agingState->GetResistanceFactor(), resistanceFactor, 0.0001 );
}