/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestChoiceOfSystem.h
* Creation Date : 09-09-2014
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TESTCHOICESYSTEM_
#define _TESTCHOICESYSTEM_
#include <cxxtest/TestSuite.h>
#include <vector>
#include<cstring>

#include "../../misc/matrixInclude.h"

#include "../../factory/electricalEquationSystem/chooseElectricSystemType.h"

//BOOST
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

//electrical Elements
#include "../../electrical/ohmicresistance.h"
#include "../../electrical/capacity.h"
#include "../../electrical/serialtwoport.h"
#include "../../electrical/paralleltwoport.h"
#include "../../object/const_obj.h"
#include "../../system/stateSystemGroup.h"
#include "../../system/generalizedsystem.h"

class TestChoiceSystem: public CxxTest::TestSuite
{
    public:
        void TestChoiceOfSystem();

};
#endif /* _TESTCHOICESYSTEM_*/
