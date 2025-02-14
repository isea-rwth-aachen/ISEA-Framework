/*
 * objectclasswrapper.cpp
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#include "objectclasswrapper.h"

namespace factory
{
template class Factory< object::Object< double >, ArgumentTypeObject<double> >;

template class ObjectClassWrapper< double, object::ConstObj >;
template class ObjectClassWrapper< double, object::LookupObj1D >;
template class ObjectClassWrapper< double, object::LookupObj2D >;
template class ObjectClassWrapper< double, object::LookupObj3D >;
template class ObjectClassWrapper< double, object::LookupObj1dWithState >;
template class ObjectClassWrapper< double, object::LookupObj2dWithState >;
template class ObjectClassWrapper< double, object::LookupObj3dWithState >;
} /* namespace factory */
