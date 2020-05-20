/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestChoiceOfSystem.cpp
* Creation Date : 09-09-2014
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestChoiceOfSystem.h"

void TestChoiceSystem::TestChoiceOfSystem()
{
    // Generate Elements which make up a pure algebraic system
    boost::scoped_ptr< electrical::ParallelTwoPort<> > parallel( new electrical::ParallelTwoPort<>() );
    
    boost::shared_ptr < electrical::SerialTwoPort<> > s1( new electrical::SerialTwoPort<>() );
    boost::shared_ptr < electrical::SerialTwoPort<> > s2( new electrical::SerialTwoPort<>() );

    parallel->AddChild( s1 );
    parallel->AddChild( s2 );

    s1->AddChild(new electrical::OhmicResistance<>(new object::ConstObj<double>( 2 )));
    s2->AddChild(new electrical::OhmicResistance<>(new object::ConstObj<double>( 2 )));

    double Current = 4.0;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    parallel->SetSystem(&stateSystemGroup);
    stateSystemGroup.Initialize();
    parallel->SetInitialCurrent(Current);
    parallel->UpdateStateSystemGroup();

    boost::shared_ptr< systm::GeneralizedSystem< myMatrixType > > test( factory::ChooseElectricEquationSystemType(&stateSystemGroup) );

    TS_ASSERT_EQUALS(strcmp(test->GetName(), "LinAlgSystem"), 0);


    // Generate Elements which make up a differential algebraic system

    boost::scoped_ptr< electrical::ParallelTwoPort<> > parallelDae( new electrical::ParallelTwoPort<>() );
    
    boost::shared_ptr < electrical::SerialTwoPort<> > s1Dae( new electrical::SerialTwoPort<>() );
    boost::shared_ptr < electrical::SerialTwoPort<> > s2Dae( new electrical::SerialTwoPort<>() );

    parallelDae->AddChild( s1Dae );
    parallelDae->AddChild( s2Dae );

    s1Dae->AddChild(new electrical::OhmicResistance<>(new object::ConstObj<double>( 2 )));
    s2Dae->AddChild(new electrical::Capacity<>(new object::ConstObj<double>( 2 )));

    systm::StateSystemGroup< myMatrixType > stateSystemGroupDae;
    parallelDae->SetSystem(&stateSystemGroupDae);
    stateSystemGroupDae.Initialize();
    parallelDae->SetInitialCurrent(Current);
    parallelDae->UpdateStateSystemGroup();

    boost::shared_ptr< systm::GeneralizedSystem< myMatrixType > > testDae( factory::ChooseElectricEquationSystemType(&stateSystemGroupDae) );

    TS_ASSERT_EQUALS(strcmp(testDae->GetName(), "DifferentialAlgebraicSystem"), 0);

}
