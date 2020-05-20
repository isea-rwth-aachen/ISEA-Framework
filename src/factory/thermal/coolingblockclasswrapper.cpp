#include "coolingblockclasswrapper.h"

namespace factory
{

template class Factory< thermal::CoolingBlock<double>, ArgumentTypeCoolingBlock<double> >;

template class CoolingBlockClassWrapper< double, thermal::CoolingBlock >;

}
