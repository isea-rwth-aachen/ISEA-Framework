#include "cartesian.h"

namespace geometry
{

template class geometry::Cartesian<double>;

template const Cartesian<double> operator+(const Cartesian<double> &lhs, const Cartesian<double> &rhs);
template const Cartesian<double> operator-(const Cartesian<double> &lhs, const Cartesian<double> &rhs);
template const Cartesian<double> operator*(const Cartesian<double> &lhs, double rhs);
template const Cartesian<double> operator/(const Cartesian<double> &lhs, double rhs);

}
