#include "TestRainflow.h"

#include <algorithm>

void TestRainflow::TestCycleCounting()
{
    TestRainflow::AssertCorrectResult( {}, {} );
    TestRainflow::AssertCorrectResult( {3}, {} );
    TestRainflow::AssertCorrectResult( {0, 5}, {rainflow::Cycle( 0.5, 5.0, 2.5, 0, 1 )} );
    TestRainflow::AssertCorrectResult( {7, -1}, {rainflow::Cycle( 0.5, 8.0, 3.0, 0, 1 )} );
    TestRainflow::AssertCorrectResult( {3, 7, 5, 11, -2, 6},
                                       {rainflow::Cycle( 1.0, 2.0, 6.0, 1, 2 ), rainflow::Cycle( 0.5, 8.0, 7.0, 0, 3 ),
                                        rainflow::Cycle( 0.5, 13.0, 4.5, 3, 4 ), rainflow::Cycle( 0.5, 8.0, 2.0, 4, 5 )} );
    TestRainflow::AssertCorrectResult(
     {81.4724, 90.5792, 12.6987, 91.3376, 63.2359, 9.754, 27.8498, 54.6882, 95.7507, 96.4889, 15.7613, 97.0593, 95.7167,
      48.5376, 80.028, 14.1886, 42.1761, 91.5736, 79.2207, 95.9492},
     {rainflow::Cycle( 0.5, 9.1068, 86.0258, 0, 1 ), rainflow::Cycle( 0.5, 77.8805, 51.639, 1, 2 ),
      rainflow::Cycle( 0.5, 78.6389, 52.0181, 2, 3 ), rainflow::Cycle( 0.5, 81.5836, 50.5458, 3, 5 ),
      rainflow::Cycle( 1, 80.7276, 56.1251, 9, 10 ), rainflow::Cycle( 1, 31.4904, 64.2828, 13, 14 ),
      rainflow::Cycle( 1, 12.3529, 85.3971, 17, 18 ), rainflow::Cycle( 0.5, 87.3053, 53.4066, 5, 11 ),
      rainflow::Cycle( 0.5, 82.8707, 55.6239, 11, 15 ), rainflow::Cycle( 0.5, 81.7606, 55.0689, 15, 19 )} );
}

void TestRainflow::AssertCorrectResult( const std::vector< double > &values, const std::vector< rainflow::Cycle > &expectedCycles )
{
    std::vector< rainflow::Cycle > countedCycles = rainflow::countCycles( values );
    TS_ASSERT( std::equal( expectedCycles.begin(), expectedCycles.end(), countedCycles.begin(),
                           []( const rainflow::Cycle &left, const rainflow::Cycle &right )
                           {
        return CyclesAreEqual( left, right );
    } ) );
}

bool TestRainflow::CyclesAreEqual( const rainflow::Cycle &left, const rainflow::Cycle &right, double epsilon )
{
    if ( left.mCycleCount != right.mCycleCount )
        return false;
    if ( std::abs( left.mDepth - right.mDepth ) > epsilon )
        return false;
    if ( std::abs( left.mMeanValue - right.mMeanValue ) > epsilon )
        return false;
    if ( left.mStartIndex != right.mStartIndex )
        return false;
    if ( left.mEndIndex != right.mEndIndex )
        return false;

    return true;
}
