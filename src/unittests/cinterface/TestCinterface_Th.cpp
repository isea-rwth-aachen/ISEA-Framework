#include "TestCinterface_Th.h"
#include "../../cinterface/cinterface_Th.h"

#include <fstream>
#include <string>
#include <boost/scoped_ptr.hpp>

static const double sDelta = 0.000001;



void TestCinterface_Th::TestCreation()
{
    size_t pointerStructureAddress;
    std::vector<real_T> emplacementPoints(6, 1.0);
    std::vector<real_T> cellTemperature(2, 0.0);
    std::vector<real_T> powerDissipation(2, 5.0);
    std::vector<const real_T *> powerDissipationPtr;
    powerDissipationPtr.push_back(&powerDissipation.at(0));
    powerDissipationPtr.push_back(&powerDissipation.at(0) + 1);
    std::vector<real_T> states(56, - 1.0);
    std::vector<real_T> derivatives(56, 0.0);
    real_T *emplacementPointsOutputVector = &emplacementPoints.at(0);
    real_T *cellTemperatureOutputVector = &cellTemperature.at(0);
    const real_T * const *powerDissipationInputVector = &powerDissipationPtr.at(0);
    real_T *stateVector = &states.at(0);
    real_T *dxDt = &derivatives.at(0);

    //Read inpuit file
    std::string line, file;
    std::ifstream xmlFile("TestPouchBag.xml");
    while(std::getline(xmlFile, line))
        file += line + "\n";
    const char* configStr = file.c_str();

    //Size data
    size_t numberOfCells, stateSize, definedNumberOfCells, definedStateSize;
    GetRealSizes_Th(configStr, &numberOfCells, &stateSize);
    //void GetDefinedSizes(const char* configStr, size_t *stateSize, size_t *observerSize = 0 , size_t *numberOfCells = 0, size_t *numberOfProbes = 0 );
    GetDefinedSizes(configStr, &definedStateSize,0, &definedNumberOfCells, 0 );
    SetEmplacementPoints_Th(configStr, emplacementPointsOutputVector);
    TS_ASSERT_EQUALS(numberOfCells, 2);
    TS_ASSERT_EQUALS(stateSize, 56);
    TS_ASSERT_EQUALS(definedNumberOfCells, 4);
    TS_ASSERT_EQUALS(definedStateSize, 2);
    TS_ASSERT_DELTA(emplacementPointsOutputVector[0], 0.0, sDelta);
    TS_ASSERT_DELTA(emplacementPointsOutputVector[1], 0.0, sDelta);
    TS_ASSERT_DELTA(emplacementPointsOutputVector[2], 0.0, sDelta);
    TS_ASSERT_DELTA(emplacementPointsOutputVector[3], 0.0, sDelta);
    TS_ASSERT_DELTA(emplacementPointsOutputVector[4], 0.05, sDelta);
    TS_ASSERT_DELTA(emplacementPointsOutputVector[5], 0.0, sDelta);

    InitializeSystem_Th(configStr, &pointerStructureAddress, 0);
    DeleteSystem_Th(&pointerStructureAddress);

    InitializeSystem_Th(configStr, &pointerStructureAddress, stateVector);
    for(size_t i = 0; i < 56; ++i)
    {
        TS_ASSERT_DELTA(stateVector[i], 27.0,sDelta);
    }

    CellTemperatureOutput_Th( &pointerStructureAddress, cellTemperatureOutputVector );
    TS_ASSERT_DELTA(cellTemperatureOutputVector[0], 27.0,sDelta);
    TS_ASSERT_DELTA(cellTemperatureOutputVector[1], 27.0,sDelta);

    SetPowerDissipation_Th(&pointerStructureAddress, powerDissipationInputVector);
    RunSystem_Th(&pointerStructureAddress, stateVector, dxDt);
    DeleteSystem_Th(&pointerStructureAddress);
}

void TestCinterface_Th::TestScenario()
{
    boost::scoped_ptr<size_t> pointerStructureAddress (new size_t(0));
    size_t numberOfCells, stateSize;

    //Read inpuit file
    std::string line, file;
    std::ifstream xmlFile("TestPouchBag.xml");
    while(std::getline(xmlFile, line))
        file += line + "\n";
    const char* configStr = file.c_str();

    //mdlInitializeSizes
    GetRealSizes_Th(configStr, &numberOfCells, &stateSize);
    //mdlInitializeSizes

    std::vector<real_T> emplacementPoints(3 * numberOfCells, 0.0);
    std::vector<real_T> cellTemperature(numberOfCells, 0.0);
    std::vector<real_T> powerDissipation(numberOfCells, 5.0);
    std::vector<real_T *> powerDissipationPtr;
    for(size_t i = 0; i < numberOfCells; ++i)
        powerDissipationPtr.push_back(&powerDissipation.at(i));
    std::vector<real_T> states(stateSize, 0.0);
    std::vector<real_T> derivatives(stateSize, 0.0);
    real_T *emplacementPointsOutputVector = &emplacementPoints.at(0);
    real_T *cellTemperatureOutputVector = &cellTemperature.at(0);
    real_T * const *powerDissipationInputVector = &powerDissipationPtr.at(0);
    real_T *stateVector = &states.at(0);
    real_T *dxdtVector = &derivatives.at(0);

    //mdlStart
    InitializeSystem_Th(configStr, pointerStructureAddress.get(), stateVector);
    SetEmplacementPoints_Th(configStr, emplacementPointsOutputVector);
    CellTemperatureOutput_Th(pointerStructureAddress.get(), cellTemperatureOutputVector);
    //mdlStart

    //Used for assertions
    std::vector<real_T> previousTemperatures(numberOfCells, - 1000.0);
    size_t simCounter = 0;
    //Used for assertions

    //Emulates Simulink engine
    double taskTime = 0.0;
    const double simulinkDt = 0.1;
    double savedTimeFromLastRun = 0.0;

    //mdlOutputs
    CellTemperatureOutput_Th(pointerStructureAddress.get(), cellTemperatureOutputVector);
    //mdlOutputs

    while(taskTime < 10.0)
    {
        //mdlOutputs
        CellTemperatureOutput_Th(pointerStructureAddress.get(), cellTemperatureOutputVector);
        //mdlOutputs

        //Test assertions
        if(simCounter % 10 == 0)
        {
            for(size_t i = 0; i < numberOfCells; ++i)
            {
                if(taskTime < 4.9 && taskTime > 0.1)
                {
                    TS_ASSERT_LESS_THAN(previousTemperatures[i], cellTemperatureOutputVector[i]);
                }
                else if(taskTime > 5.1)
                {
                    TS_ASSERT_LESS_THAN(cellTemperatureOutputVector[i], previousTemperatures[i]);
                }
            }
            for(size_t i = 0; i < numberOfCells; ++i)
                previousTemperatures[i] = cellTemperatureOutputVector[i];
        }
        ++simCounter;
        //Test assertions

        //Set power dissipation
        if(taskTime < 5.0)
            for(size_t i = 0; i < numberOfCells; ++i)
                *powerDissipationInputVector[i] = 5.0;
        else
            for(size_t i = 0; i < numberOfCells; ++i)
                *powerDissipationInputVector[i] = - 5.0;
        //Set power dissipation

        //Set simulink time
        taskTime += simulinkDt;
        //Set simulink time

        //mdlDerivatives
        //Calculate last dt
        double dt = taskTime - savedTimeFromLastRun;
        savedTimeFromLastRun = taskTime;
        if ( dt <= 0)
        {
            dt = 0;
            TS_FAIL("Negative dt occured\n");
        }

        SetPowerDissipation_Th(pointerStructureAddress.get(), powerDissipationInputVector);
        RunSystem_Th(pointerStructureAddress.get(), stateVector,  dxdtVector);
        //mdlDerivatives

        //Solve with implicit Euler
        for(size_t i = 0; i < stateSize; ++i)
            stateVector[i] += dxdtVector[i] * simulinkDt;
    }

    //mdlTerminate
    DeleteSystem_Th( pointerStructureAddress.get() );
    //mdlTerminate
}
