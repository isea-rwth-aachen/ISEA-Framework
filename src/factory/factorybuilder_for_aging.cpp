#ifdef BUILD_AGING_SIMULATION

#include "factorybuilder_for_aging.h"

namespace factory
{

template < typename MatrixT, typename ValueT >
Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >* FactoryBuilderForAging< MatrixT, ValueT >::BuildElectricalFactory()
{
    if ( this->mElectricalFactory )
        return this->mElectricalFactory.get();

    FactoryBuilder< MatrixT, ValueT >::BuildElectricalFactory();
    if ( !this->mAgingTwoPortFactory )
        this->BuildAgingTwoPortFactory();

    // replace wrappers for Cellelement and ElectrodeElement so that they call the AgingTwoPort factory
    Factory< state::State, ArgumentTypeState >* stateFactory = this->mStateFactory.get();
    Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory = this->mObjectFactory.get();
    Factory< electrical::TwoPort< MatrixT >, ArgumentTypeElectrical >* electricalFactory = this->mElectricalFactory.get();
    Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* agingTwoPortFactory =
     this->mAgingTwoPortFactory.get();

    electricalFactory->RemoveWrapper( "CellElement" );
    electricalFactory->RemoveWrapper( "AnodeElement" );
    electricalFactory->RemoveWrapper( "CathodeElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapperForAging< MatrixT, electrical::Cellelement >( electricalFactory, objectFactory,
                                                                                                           stateFactory, agingTwoPortFactory ),
                                   "CellElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapperForAging< MatrixT, electrical::AnodeElement >(
                                    electricalFactory, objectFactory, stateFactory, agingTwoPortFactory ),
                                   "AnodeElement" );
    electricalFactory->AddWrapper( new ElectricalClassWrapperForAging< MatrixT, electrical::CathodeElement >(
                                    electricalFactory, objectFactory, stateFactory, agingTwoPortFactory ),
                                   "CathodeElement" );
    return electricalFactory;
}

template < typename MatrixT, typename ValueT >
Factory< aging::AgingBase, ArgumentTypeAging >* FactoryBuilderForAging< MatrixT, ValueT >::BuildAgingFactory()
{
    if ( this->mAgingFactory )
        return mAgingFactory.get();

    if ( !this->mStateFactory )
        this->BuildStateFactory();
    if ( !this->mObjectFactory )
        this->BuildObjectFactory();

    this->mAgingFactory.reset( new Factory< aging::AgingBase, ArgumentTypeAging >() );
    Factory< state::State, ArgumentTypeState >* stateFactory = this->mStateFactory.get();
    Factory< object::Object< ValueT >, ArgumentTypeObject< ValueT > >* objectFactory = this->mObjectFactory.get();
    Factory< aging::AgingBase, ArgumentTypeAging >* agingFactory = this->mAgingFactory.get();

    agingFactory->AddWrapper( new AgingClassWrapper< aging::CalendarianAging >( objectFactory, stateFactory ),
                              "CalendarianAging" );
    agingFactory->AddWrapper( new AgingClassWrapper< aging::CalendarianAging2 >( objectFactory, stateFactory ),
                              "CalendarianAging2" );
    agingFactory->AddWrapper( new AgingClassWrapper< aging::CyclicalAging >( objectFactory, stateFactory ),
                              "CyclicalAging" );
    agingFactory->AddWrapper( new AgingClassWrapper< aging::CyclicalAging2 >( objectFactory, stateFactory ),
                              "CyclicalAging2" );
    agingFactory->AddWrapper( new AgingClassWrapper< aging::AnodeOverhang >( objectFactory, stateFactory ),
                              "AnodeOverhang" );
    agingFactory->AddWrapper( new AgingClassWrapper< aging::GenericAging >( objectFactory, stateFactory ),
                              "GenericAging" );
    agingFactory->AddWrapper( new AgingClassWrapper< aging::AgingLAM >( objectFactory, stateFactory ), "LAM" );
    agingFactory->AddWrapper( new AgingClassWrapper< aging::AgingLLI >( objectFactory, stateFactory ), "LLI" );

    return agingFactory;
}

template < typename MatrixT, typename ValueT >
Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >*
FactoryBuilderForAging< MatrixT, ValueT >::BuildAgingTwoPortFactory()
{
    if ( this->mAgingTwoPortFactory )
        return this->mAgingTwoPortFactory.get();

    if ( !this->mAgingFactory )
        this->BuildAgingFactory();
    if ( !this->mStateFactory )
        this->BuildStateFactory();

    this->mAgingTwoPortFactory.reset( new Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >() );
    Factory< state::State, ArgumentTypeState >* stateFactory = this->mStateFactory.get();
    Factory< aging::AgingBase, ArgumentTypeAging >* agingFactory = this->mAgingFactory.get();
    Factory< aging::AgingTwoPort< MatrixT >, ArgumentTypeAgingTwoPort >* agingTwoPortFactory =
     this->mAgingTwoPortFactory.get();

    agingTwoPortFactory->AddWrapper( new AgingTwoPortClassWrapper< MatrixT, aging::AgingTwoPort >( agingTwoPortFactory,
                                                                                                   agingFactory, stateFactory ),
                                     "CellElement" );
    agingTwoPortFactory->AddWrapper( new AgingTwoPortClassWrapper< MatrixT, aging::AgingTwoPort >( agingTwoPortFactory,
                                                                                                   agingFactory, stateFactory ),
                                     "AnodeElement" );
    agingTwoPortFactory->AddWrapper( new AgingTwoPortClassWrapper< MatrixT, aging::AgingTwoPort >( agingTwoPortFactory,
                                                                                                   agingFactory, stateFactory ),
                                     "CathodeElement" );
    return agingTwoPortFactory;
}

template class FactoryBuilderForAging< myMatrixType, ScalarUnit >;

}    // namespace factory


#endif