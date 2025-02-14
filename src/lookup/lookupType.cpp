#include "lookupType.h"

template class lookup::LookupType1D<double>;
template class lookup::NearestNeighbour1D<double>;
template class lookup::Linear1DInterpolation<double>;
template class lookup::LookupType2D<double>;
template class lookup::NearestNeighbour2D<double>;
template class lookup::Linear2DInterpolation<double>;
template class lookup::LookupType3D< double >;
template class lookup::Linear3DInterpolation< double >;

