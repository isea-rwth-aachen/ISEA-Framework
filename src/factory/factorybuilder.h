#ifndef FACTORYBUILDER_H_
#define FACTORYBUILDER_H_

#include "electrical/electricalclasswrapper.h"
#include "object/objectclasswrapper.h"
#include "state/stateclasswrapper.h"
#include "thermal/blockclasswrapper.h"
#include "thermal/coolingblockclasswrapper.h"
#include "thermal/coolingclasswrapper.h"
#include "thermal/materialclasswrapper.h"
#include "thermal/thermal_factory.h"

namespace factory
{

/// builds the default version of all factories
template < typename MatrixT, typename ValueT >
struct FactoryBuilder
{
    virtual Factory< state::State, ArgumentTypeState > *BuildStateFactory();
    virtual Factory< object::Object< double >, ArgumentTypeObject< double > > *BuildObjectFactory();
    virtual Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *BuildElectricalFactory();
    virtual Factory< thermal::ThermalBlock< double >, ArgumentTypeThermalBlock< double > > *BuildBlockFactory();
    virtual Factory< thermal::Cooling< double > > *BuildCoolingFactory();
    virtual Factory< thermal::Material< double > > *BuildMaterialFactory();
    virtual Factory< thermal::CoolingBlock< double >, ArgumentTypeCoolingBlock< double > > *BuildCoolingBlockFactory();
    virtual thermal::ThermalFactory< double > *BuildThermalFactory();

    boost::scoped_ptr< Factory< state::State, ArgumentTypeState > > mStateFactory;
    boost::scoped_ptr< Factory< object::Object< double >, ArgumentTypeObject< double > > > mObjectFactory;
    boost::scoped_ptr< Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > > mElectricalFactory;
    boost::scoped_ptr< Factory< thermal::ThermalBlock< double >, ArgumentTypeThermalBlock< double > > > mBlockFactory;
    boost::scoped_ptr< Factory< thermal::Cooling< double > > > mCoolingFactory;
    boost::scoped_ptr< Factory< thermal::Material< double > > > mMaterialFactory;
    boost::scoped_ptr< Factory< thermal::CoolingBlock< double >, ArgumentTypeCoolingBlock< double > > > mCoolingBlockFactory;
    boost::scoped_ptr< thermal::ThermalFactory< double > > mThermalFactory;
};

} /* namespace factory */

#endif /* ELECTRICALFACTORYBUILDER_H_ */
