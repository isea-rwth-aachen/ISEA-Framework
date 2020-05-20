/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : thermal_probe.h
* Creation Date : 04-03-2015
* Last Modified : Fr 06 Mär 2015 10:53:40 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _ERMAL_PROBE_
#define _ERMAL_PROBE_

#include <vector>

#include "../geometry/cartesian.h"
#include "../thermal/blocks/elements/thermal_element.h"


class TestFactories;
/// Namespace for probes
namespace probe
{

/// DESCRIPTION
class ThermalProbe
{
    friend class ::TestFactories;
    public:
        /// Constructor for vector
        ThermalProbe(const std::vector<double> &vec);
        /// Constructor for x-y-z coordinates
        ThermalProbe(const double x, const double y, const double z);
        ~ThermalProbe(){};
        geometry::Cartesian<double> GetCoordinates() const
        {
            return mProbeCoordinates;
        };

        void SetCorrespondingThermalElement( boost::shared_ptr< thermal::ThermalElement<double> > mCorr)
        {
            mCorrespondingThermalElement = mCorr;
        };

        thermal::ThermalElement<double> * GetCorrespondingThermalElement() const
        {
            return mCorrespondingThermalElement.get() ;
        };

        double GetTemperature() const
        {
            return mCorrespondingThermalElement->GetTemperature() ;
        };


    private:
        geometry::Cartesian<double> mProbeCoordinates;
        boost::shared_ptr< thermal::ThermalElement<double> > mCorrespondingThermalElement;
    protected:
};

}

#endif /* _ERMAL_PROBE_ */
