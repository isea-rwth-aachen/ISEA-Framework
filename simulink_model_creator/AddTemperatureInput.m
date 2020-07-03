function [] = AddTemperatureInput(modelLocation)

if nargin == 0
    modelLocation = 'BuildMatrixModel';
end

batteryLocation = [modelLocation '/Battery_MultiCell/Battery'];
vectorBlock = [batteryLocation '/Thermal/InitialTemperatures_Vector'];
initialTempString = get_param([vectorBlock '/InitialTemperatures_Const'], 'Value');
initialTempVector = split(strtrim(erase(initialTempString, ["[", "]"])));
numCells = size(find_system([batteryLocation '/Thermal/CellToFV'], 'BlockType', 'From'), 1);
delete_line(vectorBlock, 'InitialTemperatures_Const/1', 'InitialTemperatures/1');
delete_block([vectorBlock '/InitialTemperatures_Const']);
add_block('simulink/Signal Routing/Demux', [vectorBlock '/Demux'], 'Outputs', num2str(numCells));
add_block('simulink/Signal Routing/Mux', [vectorBlock '/Mux'], 'Inputs', num2str(length(initialTempVector)));
add_line(vectorBlock, 'Mux/1', 'InitialTemperatures/1', 'autorouting', 'on');

numConstants = 0;
for i = 1:length(initialTempVector)
    tempString = initialTempVector{i};
    if isnan(str2double(tempString))
        numCell = extractBefore(extractAfter(tempString, 'CellElement_'), '.temperature');
        add_line(vectorBlock, ['Demux/' num2str(numCell)], ['Mux/' num2str(i)]);
    else
        numConstants = numConstants + 1;
        blockName = ['Const' num2str(numConstants)];
        add_block('built-in/Constant', [vectorBlock '/' blockName], 'Value', tempString);
        add_line(vectorBlock, [blockName '/1'], ['Mux/' num2str(i)]);
    end
end

add_block('simulink/Ports & Subsystems/In1', [vectorBlock '/InitialCellTemperatures']);
add_line(vectorBlock, 'InitialCellTemperatures/1', 'Demux/1', 'autorouting', 'on');
add_block('simulink/Ports & Subsystems/In1', [batteryLocation '/Thermal/InitialCellTemperatures'], 'Position', [0, 838, 30, 852]);
add_line([batteryLocation '/Thermal'], 'InitialCellTemperatures/1', 'InitialTemperatures_Vector/1', 'autorouting', 'on');
add_block('simulink/Ports & Subsystems/In1', [batteryLocation '/InitialCellTemperatures'], 'Position', [160 460 190 475]);
add_line(batteryLocation, 'InitialCellTemperatures/1', 'Thermal/1', 'autorouting', 'on');
add_block('simulink/Ports & Subsystems/In1', [modelLocation '/Battery_MultiCell/Initial Cell Temperatures [°C]'], 'Position', [160 460 190 475]);
ports = get_param([modelLocation '/Battery_MultiCell/Battery'], 'PortHandles');
lastInputPort = num2str(length(ports.Inport));
add_line([modelLocation '/Battery_MultiCell'], 'Initial Cell Temperatures [°C]/1', ['Battery/' lastInputPort], 'autorouting', 'on');

end