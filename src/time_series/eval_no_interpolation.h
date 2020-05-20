#ifndef _EVAL_NO_INTERPOLATION_
#define _EVAL_NO_INTERPOLATION_

#include <vector>
#include <limits>
#include <cmath>



namespace electrical
{
    using std::vector;

template<typename T = double>
class EvalNoInterpolation
{
public:
    inline void CalculateValue(bool hasIndexChanged
                             , const vector<T> &
                             , const vector<T> &valuesData
                             , const T &
                             , const size_t &index
                             , T &value)
    {
        if(hasIndexChanged)
            value = valuesData[index];
    }
    inline T GetTimeUntilMaxValueDeviation(const vector<T> &timesData
                                           , const vector<T> &valuesData
                                           , const T &
                                           , const T &value
                                           , const size_t &index
                                           , const T& maxDeltaValue) const
    {
        for(size_t maxDeviationIndex = index + 1; maxDeviationIndex < timesData.size(); ++maxDeviationIndex)
            if(fabs(valuesData[maxDeviationIndex] - value) >= maxDeltaValue)
                return timesData[maxDeviationIndex];
        return std::numeric_limits<T>::max();
    }
};

} //namespace
#endif //Include guard