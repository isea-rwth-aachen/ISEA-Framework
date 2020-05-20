function[] = ConnectDiametrically(system)

Inport = 'I';
Outport = 'U';
if strcmp(get_param([system '/U'], 'BlockType'), 'Inport')
    Inport = 'U';
    Outport = 'I';
end
allBlocks = GetAllBlocksInCircuit(system);

signs = '+';
for i = 2:size(allBlocks, 1)
    signs = [signs '-'];
end
add_block('simulink/Math Operations/Add', [system '/AddIn'], 'Position', [380 200 440 260]);
set_param([system '/AddIn'], 'Inputs', signs);
add_line(system, [Inport '/1'], 'AddIn/1', 'autorouting', 'on');
add_line(system, 'AddIn/1', strcat(allBlocks(1,:), '/1'), 'autorouting', 'on');
add_line(system, strcat(allBlocks(1,:), '/1'), [Outport '/1'], 'autorouting', 'on');

for i = 2:size(allBlocks, 1)
    add_line(system, strcat(allBlocks(1,:), '/1'), strcat(allBlocks(i,:), '/1'), 'autorouting', 'on');
    add_line(system, strcat(allBlocks(i,:), '/1'), ['AddIn/' num2str(i)], 'autorouting', 'on');
end

end
