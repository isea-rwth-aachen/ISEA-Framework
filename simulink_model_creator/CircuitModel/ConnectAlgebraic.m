function[] = ConnectAlgebraic(system)

Inport = 'I';
Outport = 'U';
if strcmp(get_param([system '/U'], 'BlockType'), 'Inport')
    Inport = 'U';
    Outport = 'I';
end
allBlocks = GetAllBlocksInCircuit(system);

for i = 2:size(allBlocks, 1)
    add_block('built-in/Sum', [system '/Sum' num2str(i)], 'Position', [380 110+120*i 440 170+120*i]);
    set_param([system '/Sum' num2str(i)], 'Inputs', '+-');
    add_block('simulink/Math Operations/Algebraic Constraint', [system '/AlgebraicConstraint' num2str(i)], 'Position', [500 110+120*i 560 170+120*i]);

    add_line(system, strcat(allBlocks(i-1,:), '/1'), ['Sum' num2str(i) '/1'], 'autorouting', 'on');
    add_line(system, strcat(allBlocks(i,:), '/1'), ['Sum' num2str(i) '/2'], 'autorouting', 'on');
    add_line(system, ['Sum' num2str(i) '/1'], ['AlgebraicConstraint' num2str(i) '/1'], 'autorouting', 'on');
    add_line(system, ['AlgebraicConstraint' num2str(i) '/1'], strcat(allBlocks(i,:), '/1'), 'autorouting', 'on');
end
add_line(system, strcat(allBlocks(1,:), '/1'), [Outport '/1'], 'autorouting', 'on');



signs = '-';
for i = 1:size(allBlocks, 1)
    signs = [signs '+'];
end
load_system('ElectricalTwoPorts/Miscellaneous');
add_block('Miscellaneous/FlippedSum', [system '/Sum1'], 'Position', [620 230 680 290]);
set_param([system '/Sum1'], 'Inputs', signs);
add_block('Miscellaneous/FlippedAlgebraicConstraint', [system '/AlgebraicConstraint1'], 'Position', [500 230 560 290]);



add_line(system, 'Sum1/1', 'AlgebraicConstraint1/1', 'autorouting', 'on');
add_line(system, 'AlgebraicConstraint1/1', strcat(allBlocks(1,:), '/1'), 'autorouting', 'on');
add_line(system, [Inport '/1'], 'Sum1/1', 'autorouting', 'on');
for i = 1:size(allBlocks, 1)
    add_line(system, ['AlgebraicConstraint' num2str(i) '/1'], ['Sum1/' num2str(i+1)], 'autorouting', 'on');
end

end
