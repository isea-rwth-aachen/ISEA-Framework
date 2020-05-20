#ifndef FACTORYBUILDER_FOR_AGING_H_
#define FACTORYBUILDER_FOR_AGING_H_

#ifdef BUILD_AGING_SIMULATION

#include "aging/aging_class_wrapper.h"
#include "aging/aging_twoport_class_wrapper.h"
#include "factorybuilder.h"

namespace factory
{

/// Changes the electrical factory so that aging twoport are created for Cellelements and ElectrodeElements
template < typename MatrixT, typename ValueT >
struct FactoryBuilderForAging : public FactoryBuilder< MatrixT, ValueT >
{
    virtual Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >* BuildElectricalFactory();
    virtual Factory< aging::AgingBase, ArgumentTypeAging >* BuildAgingFactory();
    virtual Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* BuildAgingTwoPortFactory();

    boost::scoped_ptr< Factory< aging::AgingBase, ArgumentTypeAging > > mAgingFactory;
    boost::scoped_ptr< Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort > > mAgingTwoPortFactory;
};

} /* namespace factory */

#endif /* BUILD_AGING_SIMULATION */

#endif /* ELECTRICALFACTORYBUILDER_H_ */
