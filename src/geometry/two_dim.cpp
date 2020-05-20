#include "two_dim.h"

namespace geometry
{

template class geometry::TwoDim<double>;

template const TwoDim<double> operator+(const TwoDim<double> &lhs, const TwoDim<double> &rhs);
template const TwoDim<double> operator-(const TwoDim<double> &lhs, const TwoDim<double> &rhs);
template const TwoDim<double> operator/(const TwoDim<double> &lhs, double rhs);

}
