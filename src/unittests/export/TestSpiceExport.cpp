/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestSpiceExport.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 19 Jun 2017 14:59:30 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestSpiceExport.h"

#include "../../misc/matrixInclude.h"

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <sstream>

#include "../../factory/factorybuilder.h"

#include "../../export/esbVisualizer.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

#include "../../export/spiceExport.h"
#include "../../system/system.h"

void TestSpiceExport::testStreamConstructor()
{
#ifndef __NO_STRING__

    factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical >* electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromFile( "./testconfig.xml" );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::TwoPort< myMatrixType > > rootPort( electricalFactory->CreateInstance( rootparam ) );

    double Current = -2.05;

    systm::StateSystemGroup< myMatrixType > stateSystemGroup;
    rootPort->SetSystem( &stateSystemGroup );
    stateSystemGroup.Initialize();
    rootPort->SetInitialCurrent( Current );
    rootPort->UpdateStateSystemGroup();

    systm::ConstantStepDglSystemSolver< myMatrixType > solver( &stateSystemGroup, 0.001 );
    solver.Solve();
    rootPort->CalculateStateDependentValues();

    std::stringstream outstream;
    SpiceExport< myMatrixType > test( rootPort.get(), &outstream );

#endif /* __NO_STRING__ */
}
