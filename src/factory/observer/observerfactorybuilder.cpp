/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : observerfactorybuilder.cpp
* Creation Date : 10-08-2015
* Last Modified : Fr 09 Okt 2015 16:41:47 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "observerfactorybuilder.h"

namespace factory
{


template <>
void AddExternalFilterThermal< double, false >(
 Factory< observer::Filter< double, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* fct )
{
    UNUSED( fct );
};

template <>
void AddExternalFilterTwoPort< myMatrixType, false >(
 Factory< observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >, ArgumentTypeObserver >* fct )
{
    UNUSED( fct );
};


} /* namespace factory */
