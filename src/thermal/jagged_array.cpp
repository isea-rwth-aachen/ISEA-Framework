#include "jagged_array.h"
#include "thermal_structs.h"

namespace thermal
{

template class JaggedArray<IndexedValue<double> >;
template class JaggedArray<TaylorData<double> >;

}
