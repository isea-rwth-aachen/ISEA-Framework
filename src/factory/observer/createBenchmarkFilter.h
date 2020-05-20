/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : createBenchmarkFilter.h
* Creation Date : 08-10-2015
* Last Modified : Do 08 Okt 2015 14:41:34 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _CREATEBENMARKFILTER_
#define _CREATEBENMARKFILTER_

#include "../../observer/observer.h"
#include "../../observer/filter/benchmarkFilter.h"
#include "../../observer/filter/elementCounterFilter.h"

/// This function adds the benchmarkfilter and the element counterfilter to an observer
template < typename MatrixT, template < typename > class TConcrete >
void AddBenchmarkFilter( observer::Observer< MatrixT, TConcrete > *observer_ptr )
{
    observer_ptr->AddFilter( new observer::BenchmarkFilterTwoPort< MatrixT >() );
    observer_ptr->AddFilter( new observer::ElementCounterFilterTwoPort< MatrixT >() );
}
#endif /* _CREATEBENMARKFILTER_ */
