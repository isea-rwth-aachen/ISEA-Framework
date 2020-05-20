/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestStateSystem.h
* Creation Date : 03-11-2012
* Last Modified : Mo 24 Mar 014 16:31:47 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTSTATESYSTEM_
#define _TESTSTATESYSTEM_
#include <cxxtest/TestSuite.h>

#include "../../misc/matrixInclude.h"

#include "../../electrical/serialtwoport.h"
#include "../../electrical/paralleltwoport.h"
#include "../../object/const_obj.h"



template< typename T >
electrical::TwoPortWithChild< myMatrixType > *  CreateSampleNetwork( std::vector<electrical::TwoPort<myMatrixType>* > &twoPortVector, double value =10.0 )
{
    electrical::ParallelTwoPort< myMatrixType > *parallel( new electrical::ParallelTwoPort< myMatrixType >() );

    T* res( new T(new object::ConstObj<double>( value )));
    T* res1( new T(new object::ConstObj<double>( value )));
    T* res2( new T(new object::ConstObj<double>( value )));

    parallel->AddChild(res);
    parallel->AddChild(res1);
    parallel->AddChild(res2);


    twoPortVector.push_back(res);
    twoPortVector.push_back(res1);
    twoPortVector.push_back(res2);

    electrical::ParallelTwoPort< myMatrixType > *parallel2( new electrical::ParallelTwoPort< myMatrixType >() );

    res  =  new T(new object::ConstObj<double>( value ));
    res1 =  new T(new object::ConstObj<double>( value ));
    res2 =  new T(new object::ConstObj<double>( value ));

    parallel2->AddChild(res);
    parallel2->AddChild(res1);
    parallel2->AddChild(res2);

    parallel->AddChild(parallel2);

    twoPortVector.push_back(res);
    twoPortVector.push_back(res1);
    twoPortVector.push_back(res2);
    twoPortVector.push_back(parallel2);
    twoPortVector.push_back(parallel);

    return parallel;
}

class TestStateSystem: public CxxTest::TestSuite
{
public:
        void testSystemWithCapacity();
        void testSystemWithParallelPort();
        void testSystemWithSerialPort();
        void testDaeSystemWithParallelPortCurrentOhmic();
        void testDaeSystemWithParallelPortCurrentCapacity();
        void testDaeSystemWithParallelPortCurrentVoltage();
        void TestMixedSystem();
};
#endif /* _TESTSTATESYSTEM_ */
