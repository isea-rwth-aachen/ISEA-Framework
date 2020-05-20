#include "TestAgingState.h"

void TestAgingState::testCreation()
{
    state::AgingState agingState( 10.0, 20.0 );
    TS_ASSERT_EQUALS( agingState.GetCellAge(), 10.0 );
    TS_ASSERT_EQUALS( agingState.GetChargeThroughput(), 20.0 );
}

void TestAgingState::testValueChange()
{
    state::AgingState agingState{};
    agingState.SetCapacityFactor( -0.37, state::AgingStateSetFormat::DELTA );
    TS_ASSERT_DELTA( agingState.GetCapacityFactor(), 1 - 0.37, 0.00001 );
    agingState.SetCapacityFactor( 1.32, state::AgingStateSetFormat::ABSOLUT );
    TS_ASSERT_DELTA( agingState.GetCapacityFactor(), 1.32, 0.00001 );
    agingState.SetCapacityFactor( 0.95, state::AgingStateSetFormat::FACTOR );
    TS_ASSERT_DELTA( agingState.GetCapacityFactor(), 1.32 * 0.95, 0.00001 );
}