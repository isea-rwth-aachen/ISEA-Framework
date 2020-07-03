#include "standalone/aging_standalone.h"

// Global variables
std::vector< std::vector< boost::shared_ptr< ThermalState< double > > > > thermalStatesOfCellBlocks;
boost::shared_ptr< observer::ThermalObserver< double > > thermalVisualizer;

void setOutputFilenames( boost::shared_ptr< xmlparser::XmlParameter > rootNode, int agingStep )
{
    boost::shared_ptr< xmlparser::XmlParameter > observerNode = rootNode->GetElementChild( "Observer" );
    std::vector< boost::shared_ptr< xmlparser::XmlParameter > > observerTypeNodes = observerNode->GetElementChildren();
    for ( size_t i = 0; i < observerTypeNodes.size(); i++ )
    {
        std::string observerType = observerTypeNodes[i]->GetElementName();
        std::vector< boost::shared_ptr< xmlparser::XmlParameter > > observerNodes = observerTypeNodes[i]->GetElementChildren();
        for ( size_t k = 0; k < observerNodes.size(); k++ )
        {
            std::string filterType = observerNodes[k]->GetElementAttributeCharPtrValue( "class" );
            if ( !( filterType == "MatlabFilter" || filterType == "CsvFilter" ) )
                continue;
            if ( agingStep == 0 )
            {
                _tinydir_mkdir( filterType.c_str() );
                _tinydir_mkdir( ( filterType + "/" + observerType ).c_str() );
            }
            std::string filepath = observerNodes[k]->GetElementStringValue( "Filename" );
            size_t lastSlash = filepath.find_last_of( '/' );
            size_t filenameStart;
            if ( lastSlash == string::npos )
                filenameStart = 0;
            else
                filenameStart = lastSlash + 1;
            size_t extensionStart = filepath.find( '.', filenameStart );
            if ( extensionStart == string::npos )
                extensionStart = filepath.length();
            size_t filenameLength;
            if ( agingStep > 0 && observerType != "Aging" )    // filename already has a previously added suffix
            {
                size_t filenameEnd = filepath.find_last_of( '_' );
                if ( filenameEnd == string::npos )
                    filenameEnd = extensionStart - 1;
                filenameLength = filenameEnd - filenameStart;
            }
            else
            {
                filenameLength = extensionStart - filenameStart;
            }
            std::string filename = filepath.substr( filenameStart, filenameLength );
            std::string extension = filepath.substr( extensionStart );
            if ( observerType != "Aging" )
                filename += "_" + std::to_string( agingStep );
            std::string newPath = filterType + "/" + observerType + "/" + filename + extension;
            observerNodes[k]->GetElementChild( "Filename" )->SetValue( newPath.c_str() );
        }
    }
}

// Main function
int main( int argc, char *argv[] )
{
    standalone::AgingStandalone agingStandalone( "ISEA-Framework Aging Simulation Standalone" );
    if ( !agingStandalone.ParseCommandLine( argc, argv ) )
        return EXIT_FAILURE;

    agingStandalone.ReadXmlOptions();

    setOutputFilenames( agingStandalone.mParser->GetRoot(), 0 );

    if ( !agingStandalone.CreateAgingSimulation() )
        return EXIT_FAILURE;

    agingStandalone.InitializeSimulation();

    // Run simulation

    // If the profile is longer than an aging step, ignore the number of cycles and aging steps,
    // instead simulate the profile once and do aging steps during the profile
    if ( agingStandalone.GetProfileLength() > agingStandalone.mAgingStepTime * 86400 )
    {
        agingStandalone.mSteadyStateCycles = 0;
        agingStandalone.mAgingCycles = 0;
        double agingStepTime = agingStandalone.mAgingStepTime * 86400;
        size_t actualAgingStep = 0;
        while ( !agingStandalone.HasElectricalSimulationEnded() )
        {
            while ( agingStandalone.GetTimeSinceLastAgingStep() < agingStepTime && !agingStandalone.HasElectricalSimulationEnded() )
            {
                agingStandalone.DoElectricalStep();
                agingStandalone.DoThermalStep();
                agingStandalone.CollectAgingData();
            }
            agingStandalone.DoAgingStep( actualAgingStep, false );
            ++actualAgingStep;
        }
    }

    // Simulate the profile for the given amount of cycles, then scale up the aging to the aging step time
    else
    {
        for ( size_t actualAgingStep = 0; actualAgingStep < agingStandalone.mAgingSteps; ++actualAgingStep )
        {
            agingStandalone.SetCollectAgingData( false );    // steady state cycles
            for ( size_t actualCycle = 0; actualCycle < agingStandalone.mAgingCycles; ++actualCycle )
            {
                if ( actualCycle == agingStandalone.mSteadyStateCycles )
                    agingStandalone.SetCollectAgingData( true );
                if ( actualCycle > 0 )
                {
                    agingStandalone.StartNewCycle();
                }

                while ( !agingStandalone.HasElectricalSimulationEnded() )
                {
                    agingStandalone.DoElectricalStep();
                    agingStandalone.DoThermalStep();
                    agingStandalone.CollectAgingData();
                }
            }
            setOutputFilenames( agingStandalone.mParser->GetRoot(), actualAgingStep + 1 );
            agingStandalone.DoAgingStep( actualAgingStep );
            if ( actualAgingStep < agingStandalone.mAgingSteps - 1 )
            {
                agingStandalone.ResetThElSimulation();
            }
        }
    }

    // Sucessful exit
    printf( "Run was succesful\n" );
    return EXIT_SUCCESS;
}
