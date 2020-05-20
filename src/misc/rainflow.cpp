#include "rainflow.h"

#include <cmath>

namespace rainflow
{

typedef std::vector< double >::const_iterator valuesIt;
typedef std::vector< std::vector< double >::const_iterator >::iterator reversalsIt;

// rainflow implementation according to ASTM E1049-85
std::vector< Cycle > countCycles( const std::vector< double > &values )
{
    if ( values.size() < 2 )
        return std::vector< Cycle >();

    // find all reversals
    std::vector< valuesIt > reversals;

    // the first and last point are always considered reversals
    reversals.push_back( values.begin() );
    // maximums and minimums must alternate, so we always know what type of extremum to look for
    bool lookingForMaximum;
    if ( values[0] <= values[1] )
        lookingForMaximum = true;
    else
        lookingForMaximum = false;

    for ( auto it = values.begin() + 1; it != values.end() - 1; ++it )
    {
        if ( lookingForMaximum && *( it + 1 ) < *it )
        {
            reversals.push_back( it );
            lookingForMaximum = false;
        }
        else if ( !lookingForMaximum && *( it + 1 ) > *it )
        {
            reversals.push_back( it );
            lookingForMaximum = true;
        }
    }

    // the first and last point are always considered reversals
    reversals.push_back( values.end() - 1 );

    // start of the actual rainflow algorithm
    // discarded reversals are marked by setting their entry to values.end()
    std::vector< Cycle > cycles;
    auto S = reversals.begin();
    auto lastRead = reversals.begin();

    while ( lastRead != reversals.end() )
    {
        // try to construct X and Y
        std::pair< reversalsIt, reversalsIt > X{reversals.end(), reversals.end()};
        std::pair< reversalsIt, reversalsIt > Y{reversals.end(), reversals.end()};

        // go backwards from the last read reversal to find three non-discarded reversals
        // std::reverse_interator returns a reverse interator to the previous element, so that end() and rbegin() point
        // to the same element. Because we want a reverse iterator to the same element, we have to call the function
        // with lastRead+1. For the same reason, the reverse iterator has to be incremented before getting the base
        // iterator.
        for ( auto it = std::reverse_iterator< reversalsIt >( lastRead + 1 ); it != reversals.rend(); ++it )
        {
            if ( *it != values.end() )
            {
                if ( X.second == reversals.end() )
                {
                    X.second = ( it + 1 ).base();
                }
                else if ( X.first == reversals.end() )
                {
                    X.first = ( it + 1 ).base();
                    Y.second = ( it + 1 ).base();
                }
                else
                {
                    Y.first = ( it + 1 ).base();
                    break;
                }
            }
        }

        // less than three non-discarded reversals
        if ( Y.first == reversals.end() )
        {
            ++lastRead;
            continue;
        }

        std::pair< double, double > valuesX{**X.first, **X.second};
        std::pair< double, double > valuesY{**Y.first, **Y.second};
        double rangeX = std::abs( valuesX.first - valuesX.second );
        double rangeY = std::abs( valuesY.first - valuesY.second );

        if ( rangeX < rangeY )
        {
            ++lastRead;
            continue;
        }

        double mean = ( valuesY.first + valuesY.second ) / 2;
        if ( S == Y.first || S == Y.second )    // Y includes S
        {
            cycles.push_back( Cycle{0.5, rangeY, mean, std::distance( values.begin(), *Y.first ),
                                    std::distance( values.begin(), *Y.second )} );
            *Y.first = values.end();
            S = Y.second;
        }
        else
        {
            cycles.push_back( Cycle{1.0, rangeY, mean, std::distance( values.begin(), *Y.first ),
                                    std::distance( values.begin(), *Y.second )} );
            *Y.first = values.end();
            *Y.second = values.end();
        }
    }

    // out of data, count each remaining range as 0.5 cycles
    auto itPrev = reversals.end();
    for ( auto it = reversals.begin(); it != reversals.end(); ++it )
    {
        if ( *it != values.end() )    // reversal not discarded
        {
            if ( itPrev != reversals.end() )
            {
                std::pair< double, double > rangeValues = {**itPrev, **it};
                double range = std::abs( rangeValues.first - rangeValues.second );
                double mean = ( rangeValues.first + rangeValues.second ) / 2;
                cycles.push_back( Cycle{0.5, range, mean, std::distance( values.begin(), *itPrev ),
                                        std::distance( values.begin(), *it )} );
            }
            itPrev = it;
        }
    }

    return cycles;
}
}
