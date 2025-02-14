%% Simulate scenario
function SimulateScenario(scenarioName, fileID)

myStringSimulation = sprintf('Simulation running for %s. Please wait.', scenarioName);
disp(myStringSimulation);

fprintf(fileID, [GetTimeStamp(), ' Simulation started. \n']);
close_system('MultiCellInterface'); % If it should be open
sim('MultiCellInterface.mdl');
close_system('MultiCellInterface');
fprintf(fileID, [GetTimeStamp(), ' Simulation ended. \n\n']);

end