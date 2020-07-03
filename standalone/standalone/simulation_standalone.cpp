#include "simulation_standalone.h"

#include "../../src/container/matio_file.h"

extern template class electrical::TimeSeries< double, electrical::EvalNoInterpolation >;

namespace standalone
{

SimulationStandalone::SimulationStandalone( const std::string &name )
    : Standalone( name )
    , mStepTime( 1.0 )
    , mSocStopCriterion( 5 )
    , mPowerStopCriterion( 1 )
    , mThermalStopCriterion( 5.0 )
    , mQuiet( false )
    , mOutputDecimation( -1.0 )
    , mCycles( 1 )
    , mProfileChangeTime( 0.0 )
    , mProfileLength( 0.0 )
{
}

void SimulationStandalone::AddOptions()
{
    Standalone::AddOptions();
    mApp.add_option( "xml-file", mXmlFilename, "XML configuration file" )->required();
    mApp.add_option( "current-profile", mProfileFilename, "Current profile" )->required();
    mApp.add_flag( "-q,--quiet", mQuiet, "Suppress almost all output" );
    mApp.add_option( "-d,--decimation", mOutputDecimation, "Decimation for the default stdout filters" );
}

bool SimulationStandalone::ParseCommandLine( int argc, char *argv[] )
{
    if ( !Standalone::ParseCommandLine( argc, argv ) )
        return false;

    try
    {
        mParser.reset( new xmlparser::tinyxml2::XmlParserImpl() );
        mParser->ReadFromFile( mXmlFilename );
    }
    catch ( std::exception &e )
    {
        printf( "At 1. parameter: xml-file\n" );
        printf( "%s\n", e.what() );
        return false;
    }

    try
    {
        if ( mProfileFilename.rfind( ".mat" ) != std::string::npos )
        {
            matlab::MatFile matFile( mProfileFilename );
            matlab::MatioData &matStruct = matFile["diga"]["daten"];
            std::vector< double > time = matStruct["Programmdauer"].operator()< matlab::MatioData::X_DIM >( 0 );
            std::vector< double > current = matStruct["Strom"].operator()< matlab::MatioData::X_DIM >( 0 );
            mProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( time, current ) );
        }
        else
        {
            std::ifstream profileFile( mProfileFilename );
            mProfile.reset( new electrical::TimeSeries< double, electrical::EvalNoInterpolation >( profileFile ) );
        }
        mProfileLength = mProfile->GetMaxTime();
    }
    catch ( const std::exception &e )
    {
        printf( "At 2. parameter: profile\n" );
        printf( "%s\n", e.what() );
        return false;
    }
    return true;
}

void SimulationStandalone::ReadXmlOptions()
{
    boost::shared_ptr< xmlparser::XmlParameter > optionsNode = mParser->GetRoot()->GetElementChild( "Options" );
    if ( optionsNode->HasElementDirectChild( "StepTime" ) )
        mStepTime = optionsNode->GetElementDoubleValue( "StepTime" );
    else
        printf( "Could not read steptime from XML file, assuming default value of %.1f\n", mStepTime );
    if ( optionsNode->HasElementDirectChild( "PowerStopCriterion" ) )
        mPowerStopCriterion = optionsNode->GetElementDoubleValue( "PowerStopCriterion" );
    if ( optionsNode->HasElementDirectChild( "SocStopCriterion" ) )
        mSocStopCriterion = optionsNode->GetElementDoubleValue( "SocStopCriterion" );
    if ( optionsNode->HasElementDirectChild( "ThermalStopCriterionInDegreeC" ) )
        mThermalStopCriterion = optionsNode->GetElementDoubleValue( "ThermalStopCriterionInDegreeC" );
    if ( optionsNode->HasElementDirectChild( "Cycles" ) )
        mCycles = optionsNode->GetElementUnsignedIntValue( "Cycles" );
    if ( mOutputDecimation < 0.0 )    // set default value if no decimation was given on the command line
        mOutputDecimation = mStepTime;
}

void SimulationStandalone::FreeXml() { mParser.reset(); }

double SimulationStandalone::GetProfileLength() { return mProfileLength; }

}    // namespace standalone