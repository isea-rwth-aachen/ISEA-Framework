#ifndef _RAINFLOW_
#define _RAINFLOW_

#include <vector>

namespace rainflow
{

struct Cycle
{
    Cycle( double cycleConut, double depth, double meanValue,
           std::iterator_traits< std::vector< double >::iterator >::difference_type startIndex,
           std::iterator_traits< std::vector< double >::iterator >::difference_type endIndex )
        : mCycleCount( cycleConut )
        , mDepth( depth )
        , mMeanValue( meanValue )
        , mStartIndex( startIndex )
        , mEndIndex( endIndex )
    {
    }
    double mCycleCount;
    double mDepth;
    double mMeanValue;
    std::iterator_traits< std::vector< double >::iterator >::difference_type mStartIndex;
    std::iterator_traits< std::vector< double >::iterator >::difference_type mEndIndex;
};

std::vector< Cycle > countCycles( const std::vector< double > &values );
}

#endif
