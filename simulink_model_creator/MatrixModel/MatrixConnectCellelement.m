function[] = MatrixConnectCellelement(system)

global CellElementCounter;
allBlocks = GetAllBlocksInCircuit(system);

gotoTemp = ['T_CellElement' num2str(CellElementCounter)];
gotoCurrent = ['I_CellElement' num2str(CellElementCounter)];
gotoSoC = ['SoC_CellElement' num2str(CellElementCounter)];
gotoInitialSoC = ['InitialSoC_CellElement' num2str(CellElementCounter)];

set_param([system '/Temperature'], 'GotoTag', gotoTemp);
set_param([system '/CellCurrent'], 'GotoTag', gotoCurrent);
set_param([system '/CellSoC'], 'GotoTag', gotoSoC);
set_param([system '/InitialSoC'], 'GotoTag', gotoInitialSoC);

for i = 1:size(allBlocks, 1)
    add_line(system, 'Temperature/1', strcat(allBlocks(i,:), '/1'), 'autorouting', 'on');
    add_line(system, 'SoC/1', strcat(allBlocks(i,:), '/2'), 'autorouting', 'on');
    if startsWith(allBlocks(i,:), 'Rmphn')
        rmphnSystem = [system '/' strtrim(allBlocks(i,:))];
        rmphnBlocks = GetAllBlocksInCircuit(rmphnSystem);
        for j = 1:size(rmphnBlocks, 1)
            add_line(rmphnSystem, 'T/1', [rmphnBlocks(j,:) '/1']);
            add_line(rmphnSystem, 'SoC/1', [rmphnBlocks(j,:) '/2']);
        end
    end
end

end
