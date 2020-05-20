#ifndef _RECTANGULARBLOCKTESTER_
#define _RECTANGULARBLOCKTESTER_

#include <cxxtest/TestSuite.h>
#include "../../geometry/area.h"
#include <boost/scoped_ptr.hpp>
#include "../../thermal/blocks/rectangular_block.h"



struct RectangularBlockTester
{
    void ThermalElementsTest();
    void SurfaceAreasTest();
    bool IsInsideBlock(const geometry::Cartesian<> &vertex);
    void ConductivityMatrixTest();
    std::vector<geometry::TwoDim<> > GetBaseVertices();

    std::vector<boost::shared_ptr<thermal::ThermalElement<> > > mThermalElements;
    std::vector<std::vector<thermal::IndexedValue<double> > > mConductivityMatrix;
    std::list<thermal::IndexedArea<double> > mSurfaceAreas;
    size_t mNX;
    size_t mNY;
    size_t mNZ;
    double mDimX;
    double mDimY;
    double mDimZ;
    geometry::Cartesian<> mOrigin;
    thermal::Material<> *mMaterial;
    double mTemperature;
    double mPowerDissipation;
};

#endif
