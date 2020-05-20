function[position, numberOfElement] = GetPosition(destination)

i = max(strfind(destination, '/'));
circuit = destination(1:(i-1));
allBlocks = GetAllBlocksInCircuit(circuit);

n =  size(allBlocks, 1);
position = [260 260+120*n 320 320+120*n];
numberOfElement = n + 1;

end
