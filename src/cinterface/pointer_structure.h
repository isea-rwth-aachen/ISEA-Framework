#ifndef _POINTER_STRUCTURE_
#define _POINTER_STRUCTURE_

#include "../misc/matrixInclude.h"
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace simulation
{
template < typename Matrix, typename Scalar, bool filterTypeChoice >
class ElectricalSimulation;

template < typename Matrix, typename Scalar, bool filterTypeChoice >
class ThermalSimulation;

template < typename Matrix, typename Scalar, bool filterTypeChoice >
class AgingSimulation;
}    // namespace simulation

namespace observer
{
template < typename Scalar >
class ThermalObserver;
}

#ifndef _SYMBOLIC_

struct PointerStructure
{
    boost::scoped_ptr< simulation::ElectricalSimulation< myMatrixType, double, false > > mElectricalSimulation;
};

struct PointerStructure_Th
{
    boost::scoped_ptr< simulation::ThermalSimulation< myMatrixType, double, false > > mThermalSimulation;
    real_T *mProbeTemperature;
    boost::shared_ptr< observer::ThermalObserver< double > > mThermalVisualizer;
    size_t mVisualizerCounter;
    double mVisualizerTimeDiffFrames;
    size_t mVisualizerMaxNumberOfFrames;
};

struct PointerStructure_ThEl
{
    boost::shared_ptr< simulation::ThermalSimulation< myMatrixType, double, false > > mThermalSimulation;
    boost::shared_ptr< simulation::ElectricalSimulation< myMatrixType, double, false > > mElectricalSimulation;
    boost::shared_ptr< observer::ThermalObserver< double > > mThermalVisualizer;
    size_t mVisualizerCounter;
    double mVisualizerTimeDiffFrames;
    size_t mVisualizerMaxNumberOfFrames;
};

#ifdef BUILD_AGING_SIMULATION

struct PointerStructure_ThEl_Aging : PointerStructure_ThEl
{
    boost::scoped_ptr< simulation::AgingSimulation< myMatrixType, double, false > > mAgingSimulation;
};

struct PointerStructure_Aging
{
    boost::scoped_ptr< simulation::AgingSimulation< myMatrixType, double, false > > mAgingSimulation;
};

#endif
#endif
#endif
