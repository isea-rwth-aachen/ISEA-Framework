function[] = ConnectAccordingly(system)

Inport = 'I';
Outport = 'U';
if strcmp(get_param([system '/U'], 'BlockType'), 'Inport')
    Inport = 'U';
    Outport = 'I';
end
allBlocks = GetAllBlocksInCircuit(system);

signs = '+';
for i = 2:size(allBlocks, 1)
    signs = [signs '+'];
end
add_block('built-in/Sum', [system '/Sum'], 'Position', [380 260 440 320]);
set_param([system '/Sum'], 'Inputs', signs);

for i = 1:size(allBlocks, 1)
    add_line(system, [Inport '/1'], strcat(allBlocks(i,:), '/1'), 'autorouting', 'on');
    add_line(system, strcat(allBlocks(i,:), '/1'), ['Sum/' num2str(i)], 'autorouting', 'on');
end
add_line(system, 'Sum/1', [Outport '/1'], 'autorouting', 'on');

end
