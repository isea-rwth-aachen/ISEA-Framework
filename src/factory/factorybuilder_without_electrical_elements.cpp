#ifdef BUILD_AGING_SIMULATION

#include "factorybuilder_without_electrical_elements.h"

namespace factory
{

/// build a factory that only build CellElements, SerialTwoports, ParallelTwoports, VoltageSources and
/// ElectrodeElements. Used for aging simulation without the electrical model.
template < typename MatrixT, typename ValueT >
Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >*
FactoryBuilderWithoutElectricalElements< MatrixT, ValueT >::BuildElectricalFactory()
{
    if ( this->mElectricalFactory )
        return this->mElectricalFactory.get();

    if ( !this->mStateFactory )
        this->BuildStateFactory();
    if ( !this->mObjectFactory )
        this->BuildObjectFactory();
    if ( !this->mAgingTwoPortFactory )
        this->BuildAgingTwoPortFactory();

    this->mElectricalFactory.reset( new Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >() );
    Factory< state::State, ArgumentTypeState >* stateFactory = this->mStateFactory.get();
    Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory = this->mObjectFactory.get();
    Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* agingTwoPortFactory =
     this->mAgingTwoPortFactory.get();
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >* electricalFactory = this->mElectricalFactory.get();

    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::ParallelTwoPort >( electricalFactory, objectFactory,
                                                                                                       stateFactory ),
                                   "ParallelTwoPort" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::SerialTwoPort >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "SerialTwoPort" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::VoltageSource >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "VoltageSource" );

    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::VoltageSource_OneState >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "VoltageSource_OneState" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::VoltageSource_PreisachEverett >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "VoltageSource_PreisachEverett" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, electrical::VoltageSource_PreisachDiscrete >( electricalFactory, objectFactory,
                                                                                                     stateFactory ),
                                   "VoltageSource_PreisachDiscrete" );
    electricalFactory->AddWrapper( new ElectricalClassWrapperForAging< MatrixT, electrical::Cellelement >( electricalFactory, objectFactory,
                                                                                                           stateFactory, agingTwoPortFactory ),
                                   "CellElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapperForAging< MatrixT, electrical::AnodeElement >(
                                    electricalFactory, objectFactory, stateFactory, agingTwoPortFactory ),
                                   "AnodeElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapperForAging< MatrixT, electrical::CathodeElement >(
                                    electricalFactory, objectFactory, stateFactory, agingTwoPortFactory ),
                                   "CathodeElement" );

    // these wrappers all return an empty element
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "ParallelRCAlg" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "ParallelRC" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "ZarcElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "ZarcAlgElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "OhmicResistance" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "Capacity" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "WarburgTanh" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "WarburgCotanh" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "SphericalDiffusion" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "Rmphn" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "ConstantPhaseElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapper< MatrixT, None >( electricalFactory, objectFactory, stateFactory ),
                                   "Inductance" );

    return electricalFactory;
}

template class FactoryBuilderWithoutElectricalElements< myMatrixType, ScalarUnit >;

}    // namespace factory

#endif