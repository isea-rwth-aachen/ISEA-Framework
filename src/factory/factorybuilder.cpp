#include "factorybuilder.h"
#include "../misc/matrixInclude.h"

namespace factory
{

/// Builds a factory for state
template < typename MatrixT, typename ValueT >
Factory< state::State, ArgumentTypeState > *FactoryBuilder< MatrixT, ValueT >::BuildStateFactory()
{
    if ( mStateFactory )
        return mStateFactory.get();

    mStateFactory.reset( new Factory< state::State, ArgumentTypeState >() );
    Factory< state::State, ArgumentTypeState > *stateFactory = this->mStateFactory.get();

    stateFactory->AddWrapper( new StateClassWrapper< state::Soc >( stateFactory ), "Soc" );
    stateFactory->AddWrapper( new StateClassWrapper< state::SurfaceSoc >( stateFactory ), "SurfaceSoc" );
    stateFactory->AddWrapper( new StateClassWrapper< state::ThermalState< double > >( stateFactory ),
                              "ThermalState" );
    stateFactory->AddWrapper( new StateClassWrapper< state::ValueStateWrapper< ScalarUnit > >( stateFactory ),
                              "ElectricalState" );
    stateFactory->AddWrapper( new StateClassWrapper< state::AgingState >( stateFactory ), "AgingState" );

    return stateFactory;
}

template < typename MatrixT, typename ValueT >
Factory< object::Object< double >, ArgumentTypeObject< double > > *FactoryBuilder< MatrixT, ValueT >::BuildObjectFactory()
{
    if ( mObjectFactory )
        return mObjectFactory.get();

    if ( !mStateFactory )
        BuildStateFactory();

    mObjectFactory.reset( new Factory< object::Object< double >, ArgumentTypeObject< double > >() );
    Factory< state::State, ArgumentTypeState > *stateFactory = this->mStateFactory.get();
    Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory = this->mObjectFactory.get();

    objectFactory->AddWrapper( new ObjectClassWrapper< double, object::ConstObj >( stateFactory, objectFactory ),
                               "ConstObj" );
    objectFactory->AddWrapper( new ObjectClassWrapper< double, object::LookupObj1D >( stateFactory, objectFactory ),
                               "LookupObj1D" );
    objectFactory->AddWrapper( new ObjectClassWrapper< double, object::LookupObj2D >( stateFactory, objectFactory ),
                               "LookupObj2D" );
    objectFactory->AddWrapper( new ObjectClassWrapper< double, object::LookupObj1dWithState >( stateFactory, objectFactory ),
                               "LookupObj1dWithState" );
    objectFactory->AddWrapper( new ObjectClassWrapper< double, object::LookupObj2dWithState >( stateFactory, objectFactory ),
                               "LookupObj2dWithState" );
    objectFactory->AddWrapper( new ObjectClassWrapper< double, object::MultiObj >( stateFactory, objectFactory ),
                               "MultiObj" );
    return objectFactory;
}

/// Builds a Factory for ::electrical
template < typename MatrixT, typename ValueT >
Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *FactoryBuilder< MatrixT, ValueT >::BuildElectricalFactory()
{
    if ( mElectricalFactory )
        return mElectricalFactory.get();

    if ( !mStateFactory )
        BuildStateFactory();
    if ( !mObjectFactory )
        BuildObjectFactory();

    mElectricalFactory.reset( new Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >() );
    Factory< state::State, ArgumentTypeState > *stateFactory = this->mStateFactory.get();
    Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory = this->mObjectFactory.get();
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical > *electricalFactory = this->mElectricalFactory.get();

    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ParallelRCAlg >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "ParallelRCAlg" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ParallelRC >( electricalFactory,
                                                                                                  objectFactory, stateFactory ),
                                   "ParallelRC" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ParallelTwoPort >( electricalFactory, objectFactory,
                                                                                                       stateFactory ),
                                   "ParallelTwoPort" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::SerialTwoPort >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "SerialTwoPort" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Cellelement >( electricalFactory,
                                                                                                   objectFactory, stateFactory ),
                                   "CellElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::AnodeElement >( electricalFactory, objectFactory,
                                                                                                    stateFactory ),
                                   "AnodeElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::CathodeElement >( electricalFactory, objectFactory,
                                                                                                      stateFactory ),
                                   "CathodeElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Zarc >( electricalFactory, objectFactory, stateFactory ),
                                   "ZarcElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ZarcAlg >( electricalFactory, objectFactory, stateFactory ),
                                   "ZarcAlgElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::OhmicResistance >( electricalFactory, objectFactory,
                                                                                                       stateFactory ),
                                   "OhmicResistance" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Capacity >( electricalFactory, objectFactory, stateFactory ),
                                   "Capacity" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::VoltageSource >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "VoltageSource" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::WarburgTanh >( electricalFactory,
                                                                                                   objectFactory, stateFactory ),
                                   "WarburgTanh" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::WarburgCotanh >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "WarburgCotanh" );

    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::SphericalDiffusion >( electricalFactory, objectFactory,
                                                                                                          stateFactory ),
                                   "SphericalDiffusion" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Rmphn >( electricalFactory, objectFactory, stateFactory ),
                                   "Rmphn" );

    // UNSUPPORTED
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ConstantPhaseElement >( electricalFactory, objectFactory,
                                                                                                            stateFactory ),
                                   "ConstantPhaseElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::Inductance >( electricalFactory,
                                                                                                  objectFactory, stateFactory ),
                                   "Inductance" );

    return electricalFactory;
}

/// Builds a Factory for ::thermal::ThermalBlock
template < typename MatrixT, typename ValueT >
Factory< thermal::ThermalBlock< double >, ArgumentTypeThermalBlock< double > > *FactoryBuilder< MatrixT, ValueT >::BuildBlockFactory()
{
    if ( mBlockFactory )
        return mBlockFactory.get();

    if ( !mMaterialFactory )
        this->BuildMaterialFactory();

    mBlockFactory.reset( new Factory< thermal::ThermalBlock< double >, ArgumentTypeThermalBlock< double > >() );
    Factory< thermal::Material< double > > *materialFactory = mMaterialFactory.get();
    Factory< thermal::ThermalBlock< double >, ArgumentTypeThermalBlock< double > > *blockFactory = mBlockFactory.get();

    blockFactory->AddWrapper( new BlockClassWrapper< double, thermal::RectangularBlock >( materialFactory ),
                              "RectangularBlock" );
    blockFactory->AddWrapper( new BlockClassWrapper< double, thermal::QuadraticCellBlock >( materialFactory ),
                              "QuadraticCellBlock" );
    blockFactory->AddWrapper( new BlockClassWrapper< double, thermal::Supercap >( materialFactory ), "Supercap" );
    blockFactory->AddWrapper( new BlockClassWrapper< double, thermal::TriangularPrismBlock >( materialFactory ),
                              "TriangularPrismBlock" );
    blockFactory->AddWrapper( new BlockClassWrapper< double, thermal::HexagonalCellBlock >( materialFactory ),
                              "HexagonalCellBlock" );

    return blockFactory;
}

/// Builds a Factory for ::thermal::Cooling
template < typename MatrixT, typename ValueT >
Factory< thermal::Cooling< double > > *FactoryBuilder< MatrixT, ValueT >::BuildCoolingFactory()
{
    if ( mCoolingFactory )
        return mCoolingFactory.get();

    if ( !mObjectFactory )
        this->BuildObjectFactory();

    mCoolingFactory.reset( new Factory< thermal::Cooling< double > >() );
    Factory< object::Object< double >, ArgumentTypeObject< double > > *objectFactory = mObjectFactory.get();
    Factory< thermal::Cooling< double > > *coolingFactory = mCoolingFactory.get();

    coolingFactory->AddWrapper( new CoolingClassWrapper< double, thermal::DirichletBoundaryCondition >( objectFactory ),
                                "DirichletBoundaryCondition" );
    coolingFactory->AddWrapper( new CoolingClassWrapper< double, thermal::CoolingByLookUp >( objectFactory ),
                                "CoolingByLookUp" );
    coolingFactory->AddWrapper( new CoolingClassWrapper< double, thermal::CoolingByConstantValue >( objectFactory ),
                                "CoolingByConstantValue" );
    coolingFactory->AddWrapper( new CoolingClassWrapperThermalIsolation< double >( objectFactory ),
                                "ThermalIsolation" );

    return coolingFactory;
}

/// Builds a Factory for thermal::Material
template < typename MatrixT, typename ValueT >
Factory< thermal::Material< double > > *FactoryBuilder< MatrixT, ValueT >::BuildMaterialFactory()
{
    if ( mMaterialFactory )
        return mMaterialFactory.get();

    mMaterialFactory.reset( new Factory< thermal::Material< double > >() );
    Factory< thermal::Material< double > > *materialFactory = mMaterialFactory.get();

    materialFactory->AddWrapper( new MaterialClassWrapperIsotropic< double >(), "Material" );
    materialFactory->AddWrapper( new MaterialClassWrapperCartesian< double >(), "MaterialCartesian" );
    materialFactory->AddWrapper( new MaterialClassWrapperCylindric< double >(), "MaterialCylindric" );
    return materialFactory;
}

/// Builds a Factory for ::thermal::CoolingBlock
template < typename MatrixT, typename ValueT >
Factory< thermal::CoolingBlock< double >, ArgumentTypeCoolingBlock< double > > *
FactoryBuilder< MatrixT, ValueT >::BuildCoolingBlockFactory()
{
    if ( mCoolingBlockFactory )
        return mCoolingBlockFactory.get();

    if ( !mCoolingFactory )
        BuildCoolingFactory();

    mCoolingBlockFactory.reset( new Factory< thermal::CoolingBlock< double >, ArgumentTypeCoolingBlock< double > >() );
    Factory< thermal::Cooling< double > > *coolingFactory = mCoolingFactory.get();
    Factory< thermal::CoolingBlock< double >, ArgumentTypeCoolingBlock< double > > *coolingBlockFactory =
     mCoolingBlockFactory.get();

    coolingBlockFactory->AddWrapper( new CoolingBlockClassWrapper< double, thermal::CoolingHorizontalPlane >( coolingFactory ),
                                     "CoolingHorizontalPlane" );
    coolingBlockFactory->AddWrapper( new CoolingBlockClassWrapper< double, thermal::CoolingVerticalPlane >( coolingFactory ),
                                     "CoolingVerticalPlane" );
    coolingBlockFactory->AddWrapper( new CoolingBlockClassWrapper< double, thermal::CoolingPrismatic >( coolingFactory ),
                                     "CoolingPrismatic" );

    return coolingBlockFactory;
}

template < typename MatrixT, typename ValueT >
thermal::ThermalFactory< double > *FactoryBuilder< MatrixT, ValueT >::BuildThermalFactory()
{
    if ( mThermalFactory )
        return mThermalFactory.get();

    if ( !mMaterialFactory )
        BuildMaterialFactory();
    if ( !mBlockFactory )
        BuildBlockFactory();
    if ( !mCoolingFactory )
        BuildCoolingFactory();
    if ( !mCoolingBlockFactory )
        BuildCoolingBlockFactory();

    mThermalFactory.reset( new thermal::ThermalFactory< double >( mMaterialFactory.get(), mBlockFactory.get(),
                                                                  mCoolingFactory.get(), mCoolingBlockFactory.get() ) );
    return mThermalFactory.get();
}

template class FactoryBuilder< myMatrixType, ScalarUnit >;
// needed for systemTo... standalones
#ifdef _SYMBOLIC_
template class FactoryBuilder< myMatrixType, double >;
#endif

}    // namespace factory