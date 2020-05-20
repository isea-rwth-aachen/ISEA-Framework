/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : main.cpp
* Creation Date : 31-10-2012
* Last Modified : Do 12 Mär 2015 17:10:06 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/

//STD
#include <string>
#include <iterator>
#include <fstream>

#include "../src/observer/observer.h"
#include "../src/observer/filter/decimatefilter.h"
#include "../src/observer/filter/stdoutfilter.h"

#include "../src/electrical/twoport.h"
#include "../src/system/stateSystemGroup.h"
#include "../src/system/dae_sys.h"
#include "../src/observer/observer.h"

#include "../src/cinterface/cinterface_common_functions.h"
#include "../src/cinterface/cinterface_ThEl.h"

#include "../src/misc/matrixInclude.h"

//BOOST
#include "boost/date_time.hpp"
#ifdef __EXCEPTIONS__
#include <stdexcept>
#endif



void DieDieDie()
{
    #ifdef __EXCEPTIONS__
            throw(std::runtime_error("Couldn't find file"));
    #else
            std::cerr << "Couldn't find file"   <<std::endl;
            abort();
            return;
    #endif
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        DieDieDie();
    
    std::fstream filestream(argv[1], std::fstream::in);
    if (!filestream.is_open())
        DieDieDie();

    std::string xmlString((std::istreambuf_iterator<char>(filestream)), std::istreambuf_iterator<char>());
    std::vector<char> configStrArray(xmlString.length() + 1);
    for(size_t i = 0; i < xmlString.length(); ++i)
        configStrArray.at(i) = xmlString.at(i);
    configStrArray.back() = '\0';


    //benchmark
    boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();



    //mdlInitializeSizes
    //Evaluate config string to get sizes
    char *configStr = &configStrArray.at(0);
    size_t realNumberOfCells, realStateSize, realOutputSize, definedNumberOfCells, definedStateSize, definedOutputSize, probeSize, defineProbeSize;
    GetRealSizes_ThEl(configStr, 0, &realStateSize, &realOutputSize, &realNumberOfCells, &probeSize);
    GetDefinedSizes(configStr, &definedStateSize, &definedOutputSize, &definedNumberOfCells, &defineProbeSize);
    const size_t initialNumberOfCells = PickLargerSize(definedNumberOfCells, realNumberOfCells);
    const size_t initialOutputSize = PickLargerSize(definedOutputSize, realOutputSize);
    const size_t initialStateSize = PickLargerSize(definedStateSize, realStateSize);
    const size_t initialProbSize = PickLargerSize(defineProbeSize, probeSize);

    //Input ports
    std::vector<double> currentPort(1, 0.0);
    std::vector<double> airTemperaturePort(1, 0.0);

    //Output ports
    std::vector<double> voltageVectorPort(initialOutputSize, 0.0);
    std::vector<double> currentVectorPort(initialOutputSize, 0.0);
    std::vector<double> powerVectorPort(initialOutputSize, 0.0);
    std::vector<double> socVectorPort(initialOutputSize, 0.0);
    std::vector<double> surfaceSocVectorPort(initialOutputSize, 0.0);

    std::vector<double> totalVoltagePort(1, 0.0);
    std::vector<double> totalCurrentPort(1, 0.0);
    std::vector<double> totalPowerPort(1, 0.0);
    std::vector<double> totalBatteriesPort(1, 0.0);
    std::vector<double> totalStatesPort(1, 0.0);

    std::vector<double> totalProbePort(initialProbSize, 0.0);

    std::vector<double> temperatureVectorPort(initialNumberOfCells, 0.0);

    //States
    std::vector<double> statesPort(initialStateSize, 0.0);
    std::vector<double> dxdtPort(initialStateSize, 0.0);
    //mdlInitializeSizes


    //mdlStart
    //Initialize System and get sizes
    size_t *pointerStructureAddress = new size_t(0);
    InitializeSystem_ThEl(configStr, pointerStructureAddress, &statesPort.at(0));
    
    // Assign output vectors to electrical observer
    SetOutputVectors_ThEl(pointerStructureAddress
                        , &voltageVectorPort[0]
                        , &currentVectorPort[0]
                        , &powerVectorPort[0]
                        , &socVectorPort[0]
                        , &surfaceSocVectorPort[0]
            );
    totalBatteriesPort[0] = realNumberOfCells;
    totalStatesPort[0] = realStateSize;
    //mdlStart


    //benchamrk
    boost::posix_time::time_duration elapsed = boost::posix_time::microsec_clock::local_time() - start;
    std::cout << "Creating model took " << elapsed << std::endl;
    start = boost::posix_time::microsec_clock::local_time();


    //simulink engine
    const double dt = 0.1;
    double taskTime = 0.0;
    double current = - 100.0;
    const size_t cycleCount = 100;


    for(size_t cycle = 0; cycle < cycleCount; ++cycle)
    {
        current *= -1.0;



        //mdlOutputs
        //Get input and set current
        SetCurrent_ThEl(pointerStructureAddress, current); //< Sets the new current
        RunAlgebraicElectricalSystemPart_ThEl(pointerStructureAddress, &statesPort.at(0), dt);
        
        // Set output vectors
        OutputRootElementData_ThEl(pointerStructureAddress
                                 , &totalVoltagePort[0]
                                 , &totalCurrentPort[0]
                                 , &totalPowerPort[0]);
        TriggerObserverAndCellTemperatureOutput_ThEl(pointerStructureAddress, taskTime, &temperatureVectorPort[0], &totalProbePort[0]);
        //mdlOutputs


        //mdlDerivatives
        //Run System
        RunSystem_ThEl(pointerStructureAddress, &statesPort[0], &dxdtPort[0]);
        //mdlDerivatives


        //simulink engine
        taskTime += dt;
        for(size_t i = 0; i < statesPort.size(); ++i)
            statesPort[i] = statesPort[i] + dt * dxdtPort[i];
    }


    //mdlTerminate
    delete pointerStructureAddress;
    //mdlTerminate



    elapsed = boost::posix_time::microsec_clock::local_time() - start;
    std::cout << "Running model took " << elapsed << " with " << cycleCount << "cycles" << std::endl;
}
