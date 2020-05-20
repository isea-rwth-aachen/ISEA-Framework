#include "thermal_structs.h"

namespace thermal
{

template struct TaylorData<double>;
template struct IndexedValue<double>;
template IndexedValue<double> MakeIndexedValue<double>(const size_t &index, const double &value);
template struct IndexedArea<double>;
template struct Linearization<double>;

template struct IndexIs<double>;

const double globalMaxValue = 1000000000.0;
const double globalMinValue = - globalMaxValue;

}
