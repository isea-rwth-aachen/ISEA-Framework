function [] = LoadInitialStateToWorkspace(structName, socArray, temperatureArray)
% Assigns the values for the initial states to a struct in the workspace.
% The simulink blocks read their initial state from that workspace struct.

initialState = [];

for i = 1:length(socArray)
    cellName = ['CellElement_' num2str(i)];
    initialState.(cellName).SoC = socArray(i);
    initialState.(cellName).temperature = temperatureArray(i);
end

% copy the struct to the workspace
assignin('base', 'tmpInitialState__', initialState);
evalin('base', [structName ' = tmpInitialState__;']);
evalin('base', 'clear tmpInitialState__;');

end