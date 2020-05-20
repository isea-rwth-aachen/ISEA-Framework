#ifndef FACTORYBUILDER_WITHOUT_ELECTRICAL_ELEMENTS_H_
#define FACTORYBUILDER_WITHOUT_ELECTRICAL_ELEMENTS_H_

#ifdef BUILD_AGING_SIMULATION

#include "factorybuilder_for_aging.h"

namespace factory
{

/// builds factories for a simulation without the complete electrical model.
/// the wrappers for all twoports without children are replaced by wrappers that return nothing
template < typename MatrixT, typename ValueT >
struct FactoryBuilderWithoutElectricalElements : public FactoryBuilderForAging< MatrixT, ValueT >
{
    virtual Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >* BuildElectricalFactory();
};

} /* namespace factory */

#endif /* BUILD_AGING_SIMULATION */
#endif