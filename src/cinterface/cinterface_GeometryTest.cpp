/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : cinterface.cpp
* Creation Date : 23-05-2013
* Last Modified : Mo 27 Jan 2014 17:03:30 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

#include "cinterface_GeometryTest.h"
#include "Geometry2DTest.h"

#ifndef _SYMBOLIC_

#ifdef __cplusplus
extern "C"
{
#endif

    unsigned int Geometry2DTestWrapper() { return Geometry2DTest(); }

#ifdef __cplusplus
}
#endif
#endif
