function[] = ConnectAllCoolings(destination)

allBlocks = GetAllBlocksInCircuit(destination);

if (~isempty(allBlocks))

    add_block('built-in/Mux', [destination '/Mux'], 'Position', [400 260 405 260+120*size(allBlocks, 1)]);
    set_param([destination '/Mux'], 'Inputs', num2str(size(allBlocks, 1)));
    add_line(destination, 'Mux/1', 'CoolAdder/1', 'autorouting', 'on');

    for i = 1:size(allBlocks, 1)
        add_line(destination, 'T_FV/1', strcat(allBlocks(i,:), '/1'), 'autorouting', 'on');
        add_line(destination, strcat(allBlocks(i,:), '/1'), ['Mux/' num2str(i)], 'autorouting', 'on');
    end

end

end
