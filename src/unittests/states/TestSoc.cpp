/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSoc.cpp
* Creation Date : 21-02-2014
* Last Modified : Di 12 Aug 2014 18:47:05 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestSoc.h"
#include <limits>

#include "../../states/soc.h"

void TestSoc::testSocCreation()
{
    double capacity = 20.0;
    double status = 20.0;
    electrical::state::Soc test( capacity /* [Ah] */, capacity, status /*[%]*/ );
}

void TestSoc::testSocOperations()
{
    double capacity = 20.0;
    double status = 20.0;


    electrical::state::Soc test( capacity /* [Ah] */, capacity, status /*[%]*/ );
    TS_ASSERT_DELTA( test.GetActualCapacity< ::electrical::state::SocGetFormat::AS >(), capacity * 3600, 0.000001 );
    TS_ASSERT_DELTA( test.GetValue(), status, 0.00001 );
    TS_ASSERT_DELTA( test.mActualCapacity * test.mActualSoc, status / 100 * capacity * 3600, 0.000001 );
    TS_ASSERT_DELTA( test.mActualCapacity, capacity * 3600, 0.000001 );
}

void TestSoc::testCapacityChange()
{
    double initialCapacity = 10.0;
    double actualCapacity = 8.0;
    double socValue = 50.0;
    double storedEnergy = socValue * actualCapacity;

    electrical::state::Soc socState( initialCapacity, actualCapacity, socValue );
    TS_ASSERT_DELTA( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::AH >(), initialCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::AS >(), initialCapacity * 3600, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::AH >(), actualCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::AS >(), actualCapacity * 3600, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::AH >(), socValue * actualCapacity / 100, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::AS >(), socValue * actualCapacity * 3600 / 100, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::PERCENT >(), socValue, 0.0001 );

    actualCapacity += 1.0;
    socState.SetCapacity< ::electrical::state::SocSetFormat::DELTA >( 1.0 );
    TS_ASSERT_DELTA( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::AH >(), initialCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::AH >(), actualCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::PERCENT >(), storedEnergy / actualCapacity, 0.0001 );

    actualCapacity = 3.0;
    socState.SetCapacity< ::electrical::state::SocSetFormat::ABSOLUT >( actualCapacity );
    TS_ASSERT_DELTA( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::AH >(), initialCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::AH >(), actualCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::PERCENT >(), storedEnergy / actualCapacity, 0.0001 );

    actualCapacity = 1.2 * initialCapacity;
    socState.SetCapacity< ::electrical::state::SocSetFormat::FACTOR >( 1.2 );
    TS_ASSERT_DELTA( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::AH >(), initialCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::AH >(), actualCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::PERCENT >(), storedEnergy / actualCapacity, 0.0001 );

    actualCapacity = 0.75 * initialCapacity;
    socState.SetCapacityFactor< ::electrical::state::SocSetFormat::ABSOLUT >( 0.75 );
    TS_ASSERT_DELTA( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::AH >(), initialCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::AH >(), actualCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::PERCENT >(), storedEnergy / actualCapacity, 0.0001 );

    actualCapacity += 0.2 * initialCapacity;
    socState.SetCapacityFactor< ::electrical::state::SocSetFormat::DELTA >( 0.2 );
    TS_ASSERT_DELTA( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::AH >(), initialCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::AH >(), actualCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetValue< ::electrical::state::SocGetFormat::PERCENT >(), storedEnergy / actualCapacity, 0.0001 );
    TS_ASSERT_DELTA( socState.GetActualCapacity< ::electrical::state::SocGetFormat::PERCENT >(),
                     100 * actualCapacity / initialCapacity, 0.0001 );

    TS_ASSERT_THROWS( socState.SetCapacityFactor< ::electrical::state::SocSetFormat::FACTOR >( 1.0 ), std::runtime_error& );
    TS_ASSERT_THROWS( socState.GetInitialCapacity< ::electrical::state::SocGetFormat::PERCENT >(), std::runtime_error& );
}
