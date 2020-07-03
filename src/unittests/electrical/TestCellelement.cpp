#include "TestCellelement.h"

#include "../../aging/aging_twoport.h"
#include "../../aging/generic_aging.h"
#include "../../electrical/ohmicresistance.h"
#include "../../object/const_obj.h"

void TestCellelement::TestCapacityCalculation()
{
#ifdef BUILD_AGING_SIMULATION
    auto soc = boost::make_shared< state::Soc >( 30.0, 30.0, 55.0 );
    auto thermalState = boost::make_shared< state::ThermalState< double > >( 20.0 );
    {
        auto cell = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, soc );
        boost::scoped_ptr< systm::StateSystemGroup< myMatrixType > > stateSystemGroup( new systm::StateSystemGroup< myMatrixType >() );
        auto genericAging = boost::make_shared< aging::GenericAging >( 1.0 );
        auto agingState = boost::make_shared< state::AgingState >();
        auto resistance =
         boost::make_shared< electrical::OhmicResistance< myMatrixType > >( new object::ConstObj< double >( 10.0 ) );
        aging::AgingTwoPort< myMatrixType > agingTp( {genericAging}, cell, cell, agingState );

        cell->AddChild( resistance );
        cell->SetSystem( stateSystemGroup.get() );
        stateSystemGroup->Initialize();
        cell->SetInitialCurrent( 1.0 );
        cell->UpdateStateSystemGroup();
        agingTp.CalculateAging( 0.0, 0.0 );
        TS_ASSERT_DELTA( cell->GetCapacity< state::SocGetFormat::AH >(), 30.0, 0.0001 );

        genericAging->SetCapacityFactor( 0.8 );
        genericAging->SetResistanceFactor( 1.5 );
        agingTp.CalculateAging( 0.0, 0.0 );
        TS_ASSERT_DELTA( cell->GetCapacity< state::SocGetFormat::AH >(), 30.0 * 0.8, 0.0001 );
        TS_ASSERT_DELTA( resistance->GetValue(), 15.0, 0.0001 );
    }
    {
        auto anodeSoc = boost::make_shared< state::Soc >( 20.0, 20.0, 55.0, 0, 120 );
        auto anodeElement = boost::make_shared< electrical::AnodeElement< myMatrixType > >( anodeSoc, nullptr );
        auto anode = boost::make_shared< electrical::ParallelTwoPort< myMatrixType > >();
        // anode voltage sources need negative values, the values are normally inverted in the factory
        auto anodeOcvObject =
         boost::make_shared< object::LookupObj1dWithState< double > >( std::vector< double >( {-1.0, -0.4} ),
                                                                       std::vector< double >( {0, 120} ), anodeSoc );
        auto anodeOcv = boost::make_shared< electrical::VoltageSource< myMatrixType > >( anodeOcvObject );
        anodeElement->AddChild( anodeOcv );
        anode->AddChild( anodeElement );

        auto cathodeSoc = boost::make_shared< state::Soc >( 20.0, 20.0, 55.0 );
        auto cathodeElement = boost::make_shared< electrical::CathodeElement< myMatrixType > >( cathodeSoc, nullptr );
        auto cathode = boost::make_shared< electrical::ParallelTwoPort< myMatrixType > >();
        auto cathodeOcvObject =
         boost::make_shared< object::LookupObj1dWithState< double > >( std::vector< double >( {3.0, 3.7} ),
                                                                       std::vector< double >( {0, 100} ), cathodeSoc );
        auto cathodeOcv = boost::make_shared< electrical::VoltageSource< myMatrixType > >( cathodeOcvObject );
        cathodeElement->AddChild( cathodeOcv );
        cathode->AddChild( cathodeElement );

        auto cell = boost::make_shared< electrical::Cellelement< myMatrixType > >( thermalState, soc );
        boost::scoped_ptr< systm::StateSystemGroup< myMatrixType > > stateSystemGroup( new systm::StateSystemGroup< myMatrixType >() );

        auto genericAgingAnode = boost::make_shared< aging::GenericAging >( 1.0 );
        auto agingStateAnode = boost::make_shared< state::AgingState >();
        auto agingTpAnode =
         boost::make_shared< aging::AgingTwoPort< myMatrixType > >( std::vector< boost::shared_ptr< aging::AgingBase > >{genericAgingAnode},
                                                                    anodeElement, cell, agingStateAnode );

        auto genericAgingCathode = boost::make_shared< aging::GenericAging >( 1.0 );
        auto agingStateCathode = boost::make_shared< state::AgingState >();
        auto agingTpCathode =
         boost::make_shared< aging::AgingTwoPort< myMatrixType > >( std::vector< boost::shared_ptr< aging::AgingBase > >{genericAgingCathode},
                                                                    cathodeElement, cell, agingStateCathode );

        auto agingStateCell = boost::make_shared< state::AgingState >();
        aging::AgingTwoPort< myMatrixType > agingTpCell( {}, cell, cell, agingStateCell );
        agingTpCell.SetChildren( {agingTpAnode, agingTpCathode} );

        cell->SetElectrodes( anode, cathode );
        cell->AddElectrodeElement( anodeElement );
        cell->AddElectrodeElement( cathodeElement );
        cell->AddChild( anode );
        cell->AddChild( cathode );
        cell->SetSystem( stateSystemGroup.get() );
        stateSystemGroup->Initialize();
        cell->SetInitialCurrent( 1.0 );
        cell->UpdateStateSystemGroup();
        agingTpCell.CalculateAging( 0.0, 0.0 );
        TS_ASSERT_DELTA( cell->GetCapacity< state::SocGetFormat::AH >(), 20.0, 0.01 );

        genericAgingAnode->SetCapacityFactor( 0.9 );
        genericAgingCathode->SetCapacityFactor( 0.9 );
        agingTpCell.CalculateAging( 0.0, 0.0 );
        TS_ASSERT_DELTA( cell->GetCapacity< state::SocGetFormat::AH >(), 20.0 * 0.9, 0.01 );
    }
#endif
}
