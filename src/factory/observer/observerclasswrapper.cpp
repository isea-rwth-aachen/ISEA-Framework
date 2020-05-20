/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : observerclasswrapper.cpp
* Creation Date : 10-08-2015
* Last Modified : So 08 Mai 2016 12:11:44 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "observerclasswrapper.h"

#include "../../misc/matrixInclude.h"

namespace factory
{

// template class Factory< electrical::TwoPort< myMatrixType >, ArgumentTypeObserver >;

template class ObserverClassWrapperTwoPort< myMatrixType, observer::DecimateFilterTwoPort >;
template class ObserverClassWrapperThermal< myMatrixType, observer::DecimateFilterThermal >;

#ifndef _DS1006
#ifndef _SYMBOLIC_
template class ObserverClassWrapperTwoPort< myMatrixType, observer::StdoutFilterTwoPort >;
template class ObserverClassWrapperTwoPort< myMatrixType, observer::CsvFilterTwoPort >;
template class ObserverClassWrapperTwoPort< myMatrixType, observer::MatlabFilterTwoPort >;
#ifdef _SQLITE_
template class ObserverClassWrapperTwoPort< myMatrixType, observer::SqliteFilterTwoPort >;
#endif /* _SQLITE_ */

template class ObserverClassWrapperThermal< double, observer::CsvFilterThermal >;
template class ObserverClassWrapperThermal< double, observer::MatlabFilterThermal >;
#endif /* _SYMBOLIC_ */
#endif /* _DS1006 */
}
