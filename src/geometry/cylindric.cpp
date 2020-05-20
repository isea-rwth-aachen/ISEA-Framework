#include "cylindric.h"

namespace geometry
{

template class geometry::Cylindric<double>;
template const Cartesian<double> CylToCart(const Cartesian<double> &emplacementVector, const Cylindric<double> &cyl);

template const Cylindric<double> operator+(const Cylindric<double> &lhs, const Cylindric<double> &rhs);
template const Cylindric<double> operator/(const Cylindric<double> &lhs, double rhs);

}
