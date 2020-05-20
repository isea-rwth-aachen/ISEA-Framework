function[] = MatrixConnectCellelement(system)

global CellElementCounter;
allBlocks = GetAllBlocksInCircuit(system);

gotoTemp = ['T_CellElement' num2str(CellElementCounter)];
gotoCurrent = ['I_CellElement' num2str(CellElementCounter)];
gotoSoC = ['SoC_CellElement' num2str(CellElementCounter)];
set_param([system '/Temperature'], 'GotoTag', gotoTemp);
set_param([system '/CellCurrent'], 'GotoTag', gotoCurrent);
set_param([system '/CellSoC'], 'GotoTag', gotoSoC);

for i = 1:size(allBlocks, 1)
    add_line(system, 'Temperature/1', strcat(allBlocks(i,:), '/1'), 'autorouting', 'on');
    add_line(system, 'SoC/1', strcat(allBlocks(i,:), '/2'), 'autorouting', 'on');
end

end