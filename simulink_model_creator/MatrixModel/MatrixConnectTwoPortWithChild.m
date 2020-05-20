function[] = MatrixConnectTwoPortWithChild(system)

allBlocks = GetAllBlocksInCircuit(system);

for i = 1:size(allBlocks, 1)
    add_line(system, 'T/1', strcat(allBlocks(i,:), '/1'), 'autorouting', 'on');
    add_line(system, 'SoC/1', strcat(allBlocks(i,:), '/2'), 'autorouting', 'on');
end

end
