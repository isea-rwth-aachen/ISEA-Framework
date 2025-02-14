/*
 * electricalclasswrapper.cpp
 *
 *  Created on: 17.09.2013
 *      Author: chris
 */

#include "electricalclasswrapper.h"

#include "../../misc/matrixInclude.h"

namespace factory
{

template class Factory< electrical::TwoPort< myMatrixType >, ArgumentTypeElectrical >;

template class ElectricalClassWrapper< myMatrixType, electrical::Capacity >;
template class ElectricalClassWrapper< myMatrixType, electrical::OhmicResistance >;
template class ElectricalClassWrapper< myMatrixType, electrical::VoltageSource >;
template class ElectricalClassWrapper< myMatrixType, electrical::VoltageSource_OneState >;
template class ElectricalClassWrapper< myMatrixType, electrical::VoltageSource_PreisachEverett >;
template class ElectricalClassWrapper< myMatrixType, electrical::VoltageSource_PreisachDiscrete >;
template class ElectricalClassWrapper< myMatrixType, electrical::ParallelRC >;
template class ElectricalClassWrapper< myMatrixType, electrical::ParallelRCAlg >;
template class ElectricalClassWrapper< myMatrixType, electrical::Cellelement >;
template class ElectricalClassWrapper< myMatrixType, electrical::Zarc >;
// template class ElectricalClassWrapper< myMatrixType, electrical::ZarcAlg >;
template class ElectricalClassWrapper< myMatrixType, electrical::ParallelTwoPort >;
template class ElectricalClassWrapper< myMatrixType, electrical::SerialTwoPort >;
template class ElectricalClassWrapper< myMatrixType, electrical::WarburgTanh >;
template class ElectricalClassWrapper< myMatrixType, electrical::WarburgCotanh >;
}
