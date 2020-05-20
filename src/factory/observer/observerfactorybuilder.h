/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : observerfactorybuilder.h
* Creation Date : 10-08-2015
* Last Modified : Do 15 Jun 2017 15:23:54 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _OBSERVERFACTORYBUILDER_
#define _OBSERVERFACTORYBUILDER_

// ETC
#include "../../misc/macros.h"
#include "../../misc/matrixInclude.h"
#include "../../observer/observer.h"
#include "observerclasswrapper.h"
namespace factory
{
/// Add MatlabFilter as default
template < typename MatrixT, bool matlabSupport >
void AddExternalFilterThermal( Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* fct )
{
#ifndef _SYMBOLIC_
#ifndef _DS1006
    fct->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::MatlabFilterThermal >, "MatlabFilter" );
    fct->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::BenchmarkFilterThermal >, "BenchmarkFilter" );
    fct->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::ElementCounterFilterThermal >,
                     "ElementCounterFilter" );
#endif /* _DS1006 */
#endif /* _SYMBOLIC */
}

/// Workaround function for matlab S-Functions
template <>
void AddExternalFilterThermal< double, false >(
 Factory< observer::Filter< double, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* fct );


/// Add MatlabFilter as default
template < typename MatrixT, bool matlabSupport >
void AddExternalFilterTwoPort(
 Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >* fct )
{
    UNUSED( fct );
#ifndef _SYMBOLIC_
#ifndef _DS1006
    fct->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::MatlabFilterTwoPort >, "MatlabFilter" );
#ifdef _SQLITE_
    fct->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::SqliteFilterTwoPort >, "SqliteFilter" );
#endif
    fct->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::BenchmarkFilterTwoPort >, "BenchmarkFilter" );
    fct->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::ElementCounterFilterTwoPort >,
                     "ElementCounterFilter" );
#endif /* _DS1006 */
#endif /* _SYMBOLIC_ */
}

/// Workaround function for matlab S-Functions
template <>
void AddExternalFilterTwoPort< myMatrixType, false >(
 Factory< observer::Filter< myMatrixType, electrical::TwoPort, observer::PreparationType< myMatrixType > >, ArgumentTypeObserver >* fct );

/// Builds a Factory for ::observer::filter
template < typename MatrixT, bool matlabSupport >
Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >*
BuildObserverFactoryTwoPort()
{
    Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >* observerFactory =
     new Factory< observer::Filter< MatrixT, electrical::TwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >();

#ifndef _SYMBOLIC_
#ifndef _DS1006
    observerFactory->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::CsvFilterTwoPort >, "CSVFilter" );
    observerFactory->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::StdoutFilterTwoPort >,
                                 "StdoutFilter" );
#endif /* _DS1006 */
#endif /* _SYMBOLIC_ */
    observerFactory->AddWrapper( new ObserverClassWrapperTwoPort< MatrixT, observer::DecimateFilterTwoPort >,
                                 "DecimateFilter" );
    AddExternalFilterTwoPort< MatrixT, matlabSupport >( observerFactory );
    return observerFactory;
}

/// Builds a Factory for ::observer::filter
template < typename MatrixT, bool matlabSupport >
Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >*
BuildObserverFactoryThermal()
{
    Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >* observerFactory =
     new Factory< observer::Filter< MatrixT, thermal::ThermalElement, observer::ThermalPreperation >, ArgumentTypeObserver >();

#ifndef _SYMBOLIC_
#ifndef _DS1006
    observerFactory->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::CsvFilterThermal >, "CSVFilter" );
    observerFactory->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::ElementCounterFilterThermal >,
                                 "ElementCounterFilter" );
#endif /* _DS1006 */
#endif /* _SYMBOLIC_ */
    observerFactory->AddWrapper( new ObserverClassWrapperThermal< MatrixT, observer::DecimateFilterThermal >,
                                 "DecimateFilter" );
    AddExternalFilterThermal< MatrixT, matlabSupport >( observerFactory );
    return observerFactory;
}

#ifdef BUILD_AGING_SIMULATION

/// Builds a Factory for ::observer::filter
template < typename MatrixT, bool matlabSupport >
Factory< observer::Filter< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >*
BuildObserverFactoryAging()
{
    Factory< observer::Filter< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >* observerFactory =
     new Factory< observer::Filter< MatrixT, aging::AgingTwoPort, observer::PreparationType< MatrixT > >, ArgumentTypeObserver >();

    observerFactory->AddWrapper( new ObserverClassWrapperAging< MatrixT, observer::XMLFilterAging >, "XMLFilter" );
#ifndef _DS1006

    observerFactory->AddWrapper( new ObserverClassWrapperAging< MatrixT, observer::MatlabFilterAging >,
                                 "MatlabFilter" );

#endif

    return observerFactory;
}

#endif

} /* namespace factory */
#endif /* _OBSERVERFACTORYBUILDER_ */
