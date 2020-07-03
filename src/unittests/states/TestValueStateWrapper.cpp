/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestValueStateWrapper.cpp
* Creation Date : 21-02-2014
* Last Modified : Fr 21 Feb 2014 12:17:35 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "TestValueStateWrapper.h"

#include "../../state/valueStateWrapper.h"

void TestValueStateWrapper::testCreation()
{
    double x = 10;
    state::ValueStateWrapper<double> stateX(&x);
    TS_ASSERT_EQUALS(stateX.GetValue(), x);
}

