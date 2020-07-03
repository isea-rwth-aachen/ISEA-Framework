#ifndef _SIMULATIONSTANDALONE_H
#define _SIMULATIONSTANDALONE_H

#include "standalone.h"

#include "../../libraries/tinydir/tinydir.h"
#include "../../src/misc/matrixInclude.h"
#include "../../src/time_series/time_series.h"
#include "../../src/xmlparser/tinyxml2/xmlparserimpl.h"

#include <boost/scoped_ptr.hpp>

namespace standalone
{
/// base class for all standalones that run a simulation
class SimulationStandalone : public Standalone
{
    public:
    SimulationStandalone( const std::string& name );
    bool ParseCommandLine( int argc, char* argv[] ) override;
    virtual void ReadXmlOptions();
    void FreeXml();
    double GetProfileLength();

    // options from command line and XML
    double mStepTime;
    double mSocStopCriterion;
    double mPowerStopCriterion;
    double mThermalStopCriterion;
    bool mQuiet;
    /// steptime for the default stdout filters. Default = -1.0, which means the StepTime parameter from the XML is used. 0.0 means no decimation
    double mOutputDecimation;
    size_t mCycles;

    boost::scoped_ptr< xmlparser::XmlParser > mParser;

    protected:
    void AddOptions() override;

    boost::scoped_ptr< electrical::TimeSeries< double, electrical::EvalNoInterpolation > > mProfile;
    std::string mXmlFilename;
    std::string mProfileFilename;
    double mProfileLength;
    double mProfileChangeTime;

    // electrical simulation
};
}    // namespace standalone

#endif