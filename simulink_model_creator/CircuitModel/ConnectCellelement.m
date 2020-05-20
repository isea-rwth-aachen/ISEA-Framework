function[] = ConnectCellElement(system)

global CellElementCounter;
allBlocks = GetAllBlocksInCircuit(system);

gotoTemp = ['T_CellElement' num2str(CellElementCounter)];
gotoPower = ['P_CellElement' num2str(CellElementCounter)];
set_param([system '/Temperature'], 'GotoTag', gotoTemp);
set_param([system '/PowerDissipation'], 'GotoTag', gotoPower);

signs = '+';
for i = 2:size(allBlocks, 1)
    signs = [signs '+'];
end
set_param([system '/AddPower'], 'Inputs', signs);

for i = 1:size(allBlocks, 1)
    add_line(system, 'Temperature/1', strcat(allBlocks(i,:), '/2'), 'autorouting', 'on');
    add_line(system, 'SoC/1', strcat(allBlocks(i,:), '/3'), 'autorouting', 'on');
    add_line(system, strcat(allBlocks(i,:), '/2'), ['AddPower/' num2str(i)], 'autorouting', 'on');
end

if strcmp(get_param([system '/I'], 'BlockType'), 'Outport')  
    add_line(system, strcat(allBlocks(1,:), '/1'), 'SoCFactor/1', 'autorouting', 'on');
end

end
