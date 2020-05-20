#include "time_series.h"
#include <sstream>

template class electrical::TimeSeries<double, electrical::EvalNoInterpolation>;
template class electrical::TimeSeries<double, electrical::EvalLinearInterpolation>;
