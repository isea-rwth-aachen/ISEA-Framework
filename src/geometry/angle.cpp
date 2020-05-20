#include "angle.h"

namespace geometry
{

template class Angle<double>;

template const Angle<double> operator+(const Angle<double> &lhs, const Angle<double> &rhs);
template const Angle<double> operator-(const Angle<double> &lhs, const Angle<double> &rhs);
template const Angle<double> operator/(const Angle<double> &lhs, double rhs);

}
