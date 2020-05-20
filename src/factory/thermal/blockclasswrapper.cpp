#include "blockclasswrapper.h"

namespace factory
{

template class Factory< thermal::ThermalBlock<double>, ArgumentTypeThermalBlock<double> >;

template class BlockClassWrapper< double, thermal::RectangularBlock >;
template class BlockClassWrapper< double, thermal::QuadraticCellBlock >;
template class BlockClassWrapper< double, thermal::Supercap >;

}
