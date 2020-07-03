function [] = MatrixSetInitialSoC(initialSoC_block, initialSoC)

global CellElementCounter;

constName = SetElemNoToDestination('InitialSoC', CellElementCounter);
gotoName = SetElemNoToDestination('Goto', CellElementCounter);
constDestination = [initialSoC_block '/' constName];
gotoDestination = [initialSoC_block '/' gotoName];
gotoTag = ['InitialSoC_CellElement' num2str(CellElementCounter)];
constPosition = [260 140+120*CellElementCounter 320 200+120*CellElementCounter];
gotoPosition = constPosition + [100 0 100 0];

add_block('simulink/Sources/Constant', constDestination, 'Position', constPosition, 'Value', initialSoC);
add_block('simulink/Signal Routing/Goto', gotoDestination, 'Position', gotoPosition, 'GotoTag', gotoTag, 'TagVisibility', 'global');
add_line(initialSoC_block, [constName '/1'], [gotoName '/1'], 'autorouting', 'on');

end