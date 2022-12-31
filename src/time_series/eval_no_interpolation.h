#ifndef _EVAL_NO_INTERPOLATION_
#define _EVAL_NO_INTERPOLATION_

#include <cmath>
#include <limits>
#include <vector>


#ifdef max
#pragma push_macro( "max" )
#undef max
#define RESTORE_MAX_MACRO
#endif

namespace electrical
{
using std::vector;

template < typename T = double >
class EvalNoInterpolation
{
    public:
    inline void CalculateValue( bool hasIndexChanged, const vector< T > &, const vector< T > &valuesData, const T &,
                                const size_t &index, T &value )
    {
        if ( hasIndexChanged )
            value = valuesData[index];
    }
    inline T GetTimeUntilMaxValueDeviation( const vector< T > &timesData, const vector< T > &valuesData, const T &,
                                            const T &value, const size_t &index, const T &maxDeltaValue ) const
    {
        for ( size_t maxDeviationIndex = index + 1; maxDeviationIndex < timesData.size(); ++maxDeviationIndex )
            if ( fabs( valuesData[maxDeviationIndex] - value ) >= maxDeltaValue )
                return timesData[maxDeviationIndex];
        return std::numeric_limits< T >::max();
    }
};

}    // namespace electrical

#ifdef RESTORE_MAX_MACRO
#pragma pop_macro( "size" )
#undef RESTORE_MAX_MACRO
#endif

#endif    // Include guard