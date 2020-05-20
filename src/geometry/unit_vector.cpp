#include "unit_vector.h"

namespace geometry
{

template class UnitVector<double>;

template const UnitVector<double> operator-(const UnitVector<double> &rhs);

}