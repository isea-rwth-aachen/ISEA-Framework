%% Evaluate results
function [errorCounter] = EvaluateResults(diga, electrical_or_thermal, fileID)

% Electrical or thermal simulation
load('SignalBus');
if(strcmp(electrical_or_thermal, 'electrical'));
    myType = 'Voltage';
    myUnit = '[V]';
    mySimulationData = SignalBus.Total_Voltage_V;
    myRealData = diga.daten.Spannung;
    myAllowedMaximumOfDifference = diga.daten.allowedMaximumOfDifference_voltage;
    myAllowedStandardDeviationOfDifference = diga.daten.allowedStandardDeviationOfDifference_voltage;
elseif(strcmp(electrical_or_thermal, 'thermal'))
    myType = 'Temperature';
    myUnit = '[degC]';
    mySimulationData = SignalBus.Temperature_Vector_degC;
    myRealData = diga.daten.Temperatur;
    myAllowedMaximumOfDifference = diga.daten.allowedMaximumOfDifference_temperature;
    myAllowedStandardDeviationOfDifference = diga.daten.allowedStandardDeviationOfDifference_temperature;
end

% Calculate difference between real and simulated voltage
[myTime, myValue1, myValue2] = GetComparisonDataForTwoGraphs(mySimulationData.Time, mySimulationData.Data(:,1), diga.daten.Programmdauer, myRealData); % External function
myDifference = myValue1 - myValue2;

% Calculate filtered difference of voltage
filteredDifference = WindowFunction(10, myDifference); % 10 is default

% Get standard deviation of difference
myStandardDeviationOfDifference = std(myDifference);

% Write maximum of difference and standard deviation of difference
fprintf(fileID, ['Results ', myType, ':\n']);
fprintf(fileID, ['Allowed maximum of difference: %f ', myUnit, '\n'], myAllowedMaximumOfDifference);
fprintf(fileID, ['Actual maximum of difference: %f ', myUnit, '\n'], max(abs(filteredDifference)));
fprintf(fileID, 'Allowed standard deviation of difference: %f. \n', myAllowedStandardDeviationOfDifference);
fprintf(fileID, 'Actual standard deviation of difference: %f. \n', myStandardDeviationOfDifference);

% Check for fail of pass and raise errorCounter if fail
if(max(abs(filteredDifference)) > myAllowedMaximumOfDifference  || myStandardDeviationOfDifference > myAllowedStandardDeviationOfDifference)
    fprintf(fileID, [GetTimeStamp, ' Scenario test failed. \n']);
    errorCounter = 1;
else
    fprintf(fileID, [GetTimeStamp, ' Scenario test passed. \n']);
    errorCounter = 0;
end
fprintf(fileID, '\n');

end