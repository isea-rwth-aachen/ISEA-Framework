/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : elementCounterFilter.h
* Creation Date : 08-10-2015
* Last Modified : Do 08 Okt 2015 14:49:29 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ELEMENTCOUNTERFILTER_
#define _ELEMENTCOUNTERFILTER_

#ifndef _DS1006
#ifndef __NO_STRING__
// STD
#include <iostream>

// ETC
#include "../observer.h"
#include "../observerException.h"
#include "filter.h"

#include "../../electrical/twoport.h"
#include "../../thermal/blocks/elements/thermal_element.h"


namespace observer
{
/// ElementCounterFilterBase counts the number of observed elements
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class ElementCounterFilterBase : public Filter< T, TConcrete, ArgumentType >
{
    typedef Filter< T, TConcrete, ArgumentType > FilterT;

    public:
    ElementCounterFilterBase()
        : FilterT(){};
    virtual ~ElementCounterFilterBase() { std::cout << "Observed Elements: " << mObservedElements << std::endl; };

    private:
    protected:
    size_t mObservedElements = 0;    // C++11
};

/// Empty class, needs to be filled by template class specialization
template < typename T, template < typename > class TConcrete, typename ArgumentType = PreparationType< T > >
class ElementCounterFilter : public ElementCounterFilterBase< T, TConcrete, ArgumentType >
{
};

/// Class specialization for thermal::ThermalElement
template < typename T >
class ElementCounterFilter< T, thermal::ThermalElement, ThermalPreperation >
    : public ElementCounterFilterBase< T, thermal::ThermalElement, ThermalPreperation >
{
    typedef ElementCounterFilterBase< T, thermal::ThermalElement, ThermalPreperation > FilterT;

    public:
    ElementCounterFilter()
        : FilterT(){};
    virtual void PrepareFilter( ThermalPreperation& prepData ) { this->mObservedElements = prepData.mAreas->size(); };
    virtual ~ElementCounterFilter(){};
};

/// Class specialization for electrical::TwoPort
template < typename T >
class ElementCounterFilter< T, electrical::TwoPort, PreparationType< T > >
    : public ElementCounterFilterBase< T, electrical::TwoPort, PreparationType< T > >
{
    typedef ElementCounterFilterBase< T, electrical::TwoPort, PreparationType< T > > FilterT;

    public:
    ElementCounterFilter()
        : FilterT(){};
    virtual void PrepareFilter( PreparationType< T >& prepData )
    {
        this->mObservedElements = prepData.mNumberOfElements;
    };
    virtual ~ElementCounterFilter(){};
};


template < typename T >
using ElementCounterFilterTwoPort = ElementCounterFilter< T, electrical::TwoPort, PreparationType< T > >;

template < typename T >
using ElementCounterFilterThermal = ElementCounterFilter< T, thermal::ThermalElement, ThermalPreperation >;
}    // namespace observer
#endif /* __NO_STRING__ */
#endif /* _DS1006 */
#endif /* _ELEMENTCOUNTERFILTER_ */
