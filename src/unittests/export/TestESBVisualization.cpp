/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestESBVisualization.cpp
* Creation Date : 21-02-2014
* Last Modified : Mo 19 Jun 2017 14:59:00 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestESBVisualization.h"

// STD
#include <fstream>

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

// ETC

#include "../../factory/factorybuilder.h"

#include "../../export/esbVisualizer.h"
#include "../../xmlparser/tinyxml2/xmlparserimpl.h"

void TestESBVisualization::testXmlNetwork()
{
#ifndef __NO_STRING__
    factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
    factory::Factory< electrical::TwoPort< myMatrixType >, factory::ArgumentTypeElectrical > *electricalFactory =
     factoryBuilder.BuildElectricalFactory();

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromFile( "./testconfig.xml" );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot()->GetElementChild( "RootElement" ) );
    boost::shared_ptr< electrical::TwoPort< myMatrixType > > rootport( electricalFactory->CreateInstance( rootparam ) );

    std::ofstream of( "esb.dot" );
    visualizer::EsbVisualizer< myMatrixType > testVisualizer( rootport.get(), &of );
#endif /* __NO_STRING__ */
}
