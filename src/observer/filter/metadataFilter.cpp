#include "metadataFilter.h"

namespace observer
{
template class MetadataFilter< myMatrixType, electrical::TwoPort, PreparationType< myMatrixType > >;
template class MetadataFilter< double, thermal::ThermalElement, ThermalPreperation >;
template class MetadataFilter< myMatrixType, aging::AgingTwoPort, PreparationType< myMatrixType > >;
}    // namespace observer
