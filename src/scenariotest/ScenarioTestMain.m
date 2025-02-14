%% Main
function ScenarioTestMain(pathToFinalRelease, exitFlag) % libraryPath: 'D:\fh975128_ISEA_Framework_build\Release'

%% Change '\' with '/' for the pathToFinalRelease
myBackslashIndex = (pathToFinalRelease == '\');
pathToFinalRelease(myBackslashIndex) = '/';

% Addpath to simulink_libraries
mySimulinkLibraryPath = [pathToFinalRelease, '/model/simulink_libraries/'];
addpath(mySimulinkLibraryPath);

%% Create and open log file
myTimeStamp = datestr(now, 'yyyymmdd_HHMMSS');
myFilenameRoot = ['log_' myTimeStamp];
fileID = fopen([myFilenameRoot, '.txt'], 'w');

%% Use try and catch to enable deleting of created files in case of an error
try
    
    %% Auxiliary variables for loading scenario data
    myDirectoryScenarioData = 'scenarioData/';
    myConfigFiles = dir ([ myDirectoryScenarioData '*data*']);
    
    %% Run scenario tests
    myErrorCounter = 0;
    for myIndex = 1:size(myConfigFiles, 1)
        myStruct = load([myDirectoryScenarioData myConfigFiles(myIndex).name]);
        myErrorCounterTemp = ScenarioTest(pathToFinalRelease, myStruct.scenarioName, myStruct.diga, myStruct.xmlFilename, myStruct.modelDirectory, myFilenameRoot, num2str(myIndex));
        myErrorCounter = myErrorCounter + myErrorCounterTemp;
    end
    
    %% Write the different text files into one text file and delete the not needed ones
    for myIndex = 1:size(myConfigFiles, 1)
        myTextFile = [myFilenameRoot, '_', num2str(myIndex), '.txt'];
        myTempString = fileread(myTextFile);
        fprintf(fileID, myTempString);
        delete(myTextFile);
    end
    
    %% Write status
    myStatusString = sprintf('%d of %d test(s) passed.', size(myConfigFiles, 1) - myErrorCounter, size(myConfigFiles, 1));
    fprintf(fileID, [GetTimeStamp(), ': ', myStatusString]);
    disp(myStatusString);
    
    %% Close log file and give free mexw-file
    fclose(fileID);
    clear mex;
    
    %% Evaluate via error counter
    if(exitFlag)
        if(myErrorCounter > 0)
            exit(1);
        else
            exit(0);
        end
    end
    
catch myError % Enable deleting of created files
    fclose(fileID);
    clear mex;
    rethrow(myError)
end

end