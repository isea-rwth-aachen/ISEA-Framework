%% System test
function [errorCounter] = ScenarioTest(pathToFinalRelease, scenarioName, diga, xmlFilename, model, myFilenameRoot, numberOfScenario)

try % Enable deleting of created files

% Write scenarioName into text file
fileID = fopen([myFilenameRoot, '_', numberOfScenario, '.txt'], 'w');

% Extern functions
PrepareSimulation(pathToFinalRelease, scenarioName, diga, xmlFilename, model, fileID);
SimulateScenario(scenarioName, fileID);

errorCounterElectrical = 0;
% Simulation type: electrical, thermal_electrical, thermal_electrical_disc
if(~strcmp(model, 'thermal'))
[errorCounterElectrical] = EvaluateResults(diga, 'electrical', fileID);
end

errorCounterThermal = 0;
% Simulation type: thermal, thermal_electrical, thermal_electrical_disc
if(~strcmp(model, 'electrical'))
[errorCounterThermal] = EvaluateResults(diga, 'thermal', fileID);
end

errorCounter = errorCounterElectrical + errorCounterThermal;

fclose(fileID);

catch myError
    fclose(fileID);
    rethrow(myError);
end

end