function[] = ConnectCircuitWithStates(system)

allBlocks = GetAllBlocksInCircuit(system);

signs = '+';
for i = 2:size(allBlocks, 1)
    signs = [signs '+'];
end
set_param([system '/AddPower'], 'Inputs', signs);

for i = 1:size(allBlocks, 1)
    add_line(system, 'T/1', strcat(allBlocks(i,:), '/2'), 'autorouting', 'on');
    add_line(system, 'SoC/1', strcat(allBlocks(i,:), '/3'), 'autorouting', 'on');
    add_line(system, strcat(allBlocks(i,:), '/2'), ['AddPower/' num2str(i)], 'autorouting', 'on');
end

end
