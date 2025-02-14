%% Prepare simulation
function PrepareSimulation(pathToFinalRelease, scenarioName, diga, xmlFilename, model, fileID)

fprintf(fileID, '------------ \n');
fprintf(fileID, 'Scenario test for %s: \n', scenarioName);
fprintf(fileID, '------------ \n');

% Get verificationData
assignin('base', 'diga', diga);

% Copy XML-file
CopyXMLFile(pathToFinalRelease, xmlFilename, model, fileID);

% Get the subsystem name
if(strcmp(model, 'electrical'))
    subSystemString = 'MultiCellLibrary/Battery_MultiCell_El';
    mexw32String = 'simu_dgl_system.mexw32';
    mexw64String = 'simu_dgl_system.mexw64';
elseif(strcmp(model, 'thermal'));
    subSystemString = 'MultiCellLibrary/Battery_MultiCell_Th';
    mexw32String = 'simu_dgl_system_Th.mexw32';
    mexw64String = 'simu_dgl_system_Th.mexw64';
elseif(strcmp(model, 'thermal_electrical'));
    subSystemString = 'MultiCellLibrary/Battery_MultiCell_ThEl';
    mexw32String = 'simu_dgl_system_ThEl.mexw32';
    mexw64String = 'simu_dgl_system_ThEl.mexw64';
elseif(strcmp(model, 'thermal_electrical_disc'));
    subSystemString = 'MultiCellLibrary/Battery_MultiCell_ThEl_Disc';
    mexw32String = 'simu_dgl_system_ThEl_disc.mexw32';
    mexw64String = 'simu_dgl_system_ThEl_disc.mexw64';
else
    error('Model name is invalid. It must be electrical, thermal, thermal_electrical or thermal_electrical_disc.');
end

% Copy mewx-file
CopyMexwFile(pathToFinalRelease, model, mexw32String, mexw64String, fileID);

% Change subsystem, e.g. for electrical simulations
ChangeSubsystem(model, subSystemString, fileID);

end