/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestMultiObj.cpp
* Creation Date : 25-11-2016
* Last Modified : Fr 25 Nov 2016 14:21:53 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestMultiObj.h"

#include <boost/make_shared.hpp>

#include "../../object/const_obj.h"
#include "../../object/multi_obj.h"

void TestMultiObj::testMultiObjectGetValue()
{
    typedef object::ConstObj< double > DataT;
    std::vector< boost::shared_ptr< object::Object< double > > > testVec;
    for ( size_t i = 1; i < 10; ++i )
    {
        testVec.push_back( boost::make_shared< DataT >( static_cast< double >( i ) ) );
    }
    auto x = object::AdderObj< double >( testVec );
    auto y = object::MultiplyObj< double >( testVec );
    auto z = object::DivideObj< double >( testVec );

    std::vector< double > result = {45.0, 362880, 2.7557e-06};

    TS_ASSERT_DELTA( result[0], x.GetValue(), 0.000001 );
    TS_ASSERT_DELTA( result[1], y.GetValue(), 0.000001 );
    TS_ASSERT_DELTA( result[2], z.GetValue(), 0.000001 );

    double factor = 3.5;
    x.SetFactor( factor );
    y.SetFactor( factor );
    z.SetFactor( factor );

    TS_ASSERT_DELTA( result[0] * factor, x.GetValue(), 0.000001 );
    TS_ASSERT_DELTA( result[1] * factor, y.GetValue(), 0.000001 );
    TS_ASSERT_DELTA( result[2] * factor, z.GetValue(), 0.000001 );
}
