function [] = AddSocInput(modelLocation)

if nargin == 0
    modelLocation = 'BuildMatrixModel';
end

initialSoC_block = [modelLocation '/Battery_MultiCell/Battery/InitialSoC_Vector'];
constBlocks = find_system(initialSoC_block, 'BlockType', 'Constant');
numCells = size(constBlocks, 1);
add_block('simulink/Signal Routing/Demux', [initialSoC_block '/Demux'], 'Outputs', num2str(numCells));

for i = 1:numCells;
    delete_line(initialSoC_block, ['InitialSoC_Elem' num2str(i) '/1'], ['Goto_Elem' num2str(i) '/1']);
    delete_block([initialSoC_block '/InitialSoC_Elem' num2str(i)]);
    add_line(initialSoC_block, ['Demux/' num2str(i)], ['Goto_Elem' num2str(i) '/1'], 'autorouting', 'on');
end

add_block('simulink/Ports & Subsystems/In1', [initialSoC_block '/InitialSoC']);
add_line(initialSoC_block, 'InitialSoC/1', 'Demux/1', 'autorouting', 'on');
add_block('simulink/Ports & Subsystems/In1', [modelLocation '/Battery_MultiCell/Battery/InitialSoC'], 'Position', [0, 838, 30, 852]);
add_line([modelLocation '/Battery_MultiCell/Battery'], 'InitialSoC/1', 'InitialSoC_Vector/1', 'autorouting', 'on');
add_block('simulink/Ports & Subsystems/In1', [modelLocation '/Battery_MultiCell/Initial SoC [%]'], 'Position', [160 460 190 475]);
ports = get_param([modelLocation '/Battery_MultiCell/Battery'], 'PortHandles');
lastInputPort = num2str(length(ports.Inport));
add_line([modelLocation '/Battery_MultiCell'], 'Initial SoC [%]/1', ['Battery/' lastInputPort], 'autorouting', 'on');

end