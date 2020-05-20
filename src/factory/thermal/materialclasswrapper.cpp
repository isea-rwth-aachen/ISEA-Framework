#include "materialclasswrapper.h"

namespace factory
{

template class Factory< thermal::Material<double> >;

template class MaterialClassWrapperIsotropic< double>;
template class MaterialClassWrapperCartesian< double>;
template class MaterialClassWrapperCylindric< double>;

}
