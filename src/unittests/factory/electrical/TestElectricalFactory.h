/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestElectricalFactory.h
* Creation Date : 09-03-2016
* Last Modified : Fr 11 Mär 2016 10:55:45 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTELECTRICALFACTORY_
#define _TESTELECTRICALFACTORY_
#include <cxxtest/TestSuite.h>

// BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "../../../states/soc.h"
#include "../../../states/thermal_state.h"

#include "../../../system/dae_sys.h"
#include "../../../system/stateSystemGroup.h"

#include "../../../electrical/capacity.h"
#include "../../../electrical/cellelement.h"
#include "../../../electrical/constantphaseelement.h"
#include "../../../electrical/inductance.h"
#include "../../../electrical/ohmicresistance.h"
#include "../../../electrical/parallelrc.h"
#include "../../../electrical/paralleltwoport.h"
#include "../../../electrical/serialtwoport.h"
#include "../../../electrical/voltagesource.h"
#include "../../../electrical/warburgCotanh.h"
#include "../../../electrical/warburgTanh.h"
#include "../../../electrical/zarc.h"

#include "../../../factory/factorybuilder.h"

#include "../../../xmlparser/tinyxml2/xmlparserimpl.h"


class TestElectricalFactory : public CxxTest::TestSuite
{
    public:
    template < typename MatrixT, template < typename > class TConcrete >
    void StatePropagationTest1() const;

    void TestStateInjection1Lookup() const;
    void TestStateInjectionForParallelRCLookup() const;
    void TestStateInjectionForZarcLookup() const;

    void TestFactoryInjection1Lookup() const;
    void TestFactoryInjection2Lookup() const;
    void TestFactroyInjectionForWarburgCotanh() const;
    void TestFactroyInjectionForWarburgTanh() const;

    void TestAgingStateCreation() const;

    std::string CreateConfigurationForTwoPort() const;
    std::string CreateConfigurationForTwoPort( const std::string &twoPortName, const std::string &objName ) const;

    void TestFactroyCellElementReversibleHeat() const;

    private:
    protected:
};

template < typename MatrixT, template < typename > class TConcrete >
void TestElectricalFactory::StatePropagationTest1() const
{
#ifndef _SYMBOLIC_
    factory::FactoryBuilder< MatrixT, ScalarUnit > factoryBuilder;
    factory::Factory< ::state::State, factory::ArgumentTypeState > *stateFactory = factoryBuilder.BuildStateFactory();
    factory::Factory< object::Object< double >, factory::ArgumentTypeObject< double > > *objectFactory =
     factoryBuilder.BuildObjectFactory();

    std::string name = this->CreateConfigurationForTwoPort();

    boost::scoped_ptr< xmlparser::XmlParser > parser( new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem( name.c_str() );

    boost::shared_ptr< xmlparser::XmlParameter > rootparam( parser->GetRoot() );
    boost::scoped_ptr< factory::ArgumentTypeObject< double > > argObject( new factory::ArgumentTypeObject< double > );
    auto elecData = boost::make_shared< ElectricalDataStruct< double > >();
    argObject->mElectricalDataStruct = elecData;
    boost::shared_ptr< object::Object< double > > obj( objectFactory->CreateInstance( rootparam.get(), argObject.get() ) );

    TConcrete< MatrixT > test( obj, true, elecData );

    for ( size_t i = 1; i < 10; ++i )
    {
        auto current = static_cast< double >( i );
        elecData->mCurrentValue = current;
        TS_ASSERT_DELTA( test.GetCurrentValue(), current, 0.000001 );
        auto valueR = test.mObject->GetValue();
        TS_ASSERT_DELTA( valueR, current, 0.000001 );
    }
#endif
}

#endif /* _TESTELECTRICALFACTORY_ */
