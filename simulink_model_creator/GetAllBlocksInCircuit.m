function[allBlocks] = GetAllBlocksInCircuit(circuit)

load_system(circuit);
pathNameLength = size(circuit, 2);
rawData = find_system(circuit, 'SearchDepth', 1);

allBlocks = cell(0);
indices = [];
for i = 1:size(rawData, 1)
    if(strcmp(circuit,rawData{i}))
        continue;
    end;

    position = get_param(rawData{i}, 'Position');
    x = mean([position(1) position(3)]);
    y = mean([position(2) position(4)]);
    y = mod(y - 260, 120);
    if(260 < x && x < 320 && 0 < y && y < 60)
        nameWithoutPath = rawData{i}( (pathNameLength+2) : end );
        allBlocks = [allBlocks nameWithoutPath];
        indices = [indices (position(2)-140)/120];
    end
end

numberOfBlocks = size(allBlocks, 2);
orderedBlocks = cell(1, numberOfBlocks);
for i = 1:numberOfBlocks
    orderedBlocks(indices(i)) = allBlocks(i);
end

allBlocks = char(orderedBlocks);

end
