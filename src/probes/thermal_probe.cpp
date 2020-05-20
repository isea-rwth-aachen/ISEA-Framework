/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : thermal_probe.cpp
* Creation Date : 04-03-2015
* Last Modified : Do 05 Mär 2015 12:12:41 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#include "thermal_probe.h"

namespace probe
{

ThermalProbe::ThermalProbe(const std::vector<double> &vec)
    :
        mProbeCoordinates(vec.at(0), vec.at(1), vec.at(2) )
{ }

ThermalProbe::ThermalProbe(const double x, const double y, const double z)
    :
        mProbeCoordinates(x, y, z )
{ }

} /* namespace probe */
