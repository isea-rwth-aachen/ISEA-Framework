/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestObjectFactory.cpp
* Creation Date : 28-11-2016
* Last Modified : Fr 02 Dez 2016 11:29:58 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestObjectFactory.h"

#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include "../../../factory/factorybuilder.h"
#include "../../../factory/object/objectclasswrapper.h"
#include "../../../misc/matrixInclude.h"
#include "../../../xmlparser/tinyxml2/xmlparserimpl.h"


std::string CreateLookup( std::string attribute, std::string count = "1" )
{
    return "\n\n<?xml version='1.0'?>\n"
           "<Object class='MultiObj' Operation='" +
           attribute +
           "'>\n"
           "    <Children count='" +
           count +
           "'>\n"
           "	<c1 class='ConstObj'> <Value> 10</Value></c1>\n"
           "	<c2 class='ConstObj'> <Value> 20</Value></c2>\n"
           "	<c3 class='ConstObj'> <Value> 30</Value></c3>\n"
           "    </Children>\n"
           "</Object>\n";
}

void TestObjectFactories::TestObjectFactoriesCreation()
{
    std::string ops[3] = {"Add", "Divide", "Multiply"};
    std::vector< double > result = {60, 0.016667, 6000};
    boost::shared_ptr< factory::ArgumentTypeObject< double > > nullelement;
    std::vector< boost::shared_ptr< factory::ArgumentTypeObject< double > > > multiplier{
     nullelement, boost::make_shared< factory::ArgumentTypeObject< double > >( 10 ),
     boost::make_shared< factory::ArgumentTypeObject< double > >( 0.10 ),
     boost::make_shared< factory::ArgumentTypeObject< double > >( 0.02 )};


    for ( size_t j = 0; j < multiplier.size(); ++j )
    {

        for ( size_t i = 0; i < 3; ++i )
        {
            auto xml = CreateLookup( ops[i] );

            factory::FactoryBuilder< myMatrixType, double > factoryBuilder;
            factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > >* objectFactory =
             factoryBuilder.BuildObjectFactory();

            boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
            parser->ReadFromMem( xml.c_str() );
            auto xmlNode = parser->GetRoot();
            auto object = objectFactory->CreateInstance( xmlNode, multiplier.at( j ).get() );
            if ( multiplier.at( j ).get() == nullptr )
            {
                TS_ASSERT_DELTA( result.at( i ), object->GetValue(), 0.0001 );
            }
            else
            {
                TS_ASSERT_DELTA( multiplier.at( j )->mObjectFactor * result.at( i ), object->GetValue(), 0.0001 );
            }
        }
    }
}