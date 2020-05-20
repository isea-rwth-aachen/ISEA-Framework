function [] = InitializeMatrixModel(location, numberOfCellelements, dglCount, algCount)
ClearGlobalCache();
cleanup;

addpath([ fileparts(which('InitializeMatrixModel')) '..\'] );
load_system('MatrixModel/Component');
load_system('MatrixModel/MatrixModel');
load_system('MatrixModel/ElectricalElements');
load_system('simulink');
filename = extractBefore(location, "/");
if filename == ""
    filename = location;
end
load_system(filename);

add_block('MatrixModel/Battery_MultiCell', [location '/Battery_MultiCell'], 'Position', [0, 0, 300, 300]);

system = [location '/Battery_MultiCell/Battery'];

add_block('ElectricalElements/TwoPortWithChild', [system '/ElectricalCircuit'], 'Position', [180 0 240 60]);

add_block('MatrixModel/Matrix', [system '/MatrixAxC'], 'Position', [300 60 360 120]);
DeleteSeparator([system '/MatrixAxC']);

add_block('MatrixModel/Matrix', [system '/MatrixX_alg'], 'Position', [360 60 420 120]);
add_block('simulink/Signal Routing/From', [system '/MatrixX_alg/InputCurrent'], 'Position', [60 -60 120 0]);
set_param([system '/MatrixX_alg/InputCurrent'], 'GotoTag', 'InputCurrent');
set_param([system '/MatrixX_alg/InputCurrent'], 'BlockRotation', '270');
set_param([system '/MatrixX_alg/InputCurrent'], 'BlockMirror', 'On');
add_block('simulink/Signal Routing/From', [system '/MatrixX_alg/StatesBus'], 'Position', [0 -120 60 -60]);
set_param([system '/MatrixX_alg/StatesBus'], 'GotoTag', 'StatesBus');
% set_param([system '/MatrixX_alg/StatesBus'], 'TagVisibility', 'global');
set_param([system '/MatrixX_alg/StatesBus'], 'BlockRotation', '270');
set_param([system '/MatrixX_alg/StatesBus'], 'BlockMirror', 'On');


add_block('simulink/Signal Routing/From', [system '/MatrixX_alg/BalancerBus'], 'Position', [-60 -180 0 -120]);
set_param([system '/MatrixX_alg/BalancerBus'], 'GotoTag', 'BalancerBus');
set_param([system '/MatrixX_alg/BalancerBus'], 'TagVisibility', 'global');
set_param([system '/MatrixX_alg/BalancerBus'], 'BlockRotation', '270');
set_param([system '/MatrixX_alg/BalancerBus'], 'BlockMirror', 'On');


DeleteSeparator([system '/MatrixX_alg']);
add_block('MatrixModel/Matrix', [system '/CellCurrentMatrices'], 'Position', [1155 80 1215 140]);
add_block('MatrixModel/Matrix', [system '/CellVoltageMatrices'], 'Position', [1305 -20 1365 40]);
DeleteSeparator([system '/CellCurrentMatrices']);
DeleteSeparator([system '/CellVoltageMatrices']);
add_line(system, 'CellVoltageMatrices/1', 'VoltageVector/1');

add_block('MatrixModel/ActiveBalancingCellCurrents', [system '/ActiveBalancing'], 'Position', [180 480 240 540]);
add_line(system, 'BalancingCurrent/1', 'ActiveBalancing/1', 'autorouting', 'on');

add_block('simulink/Signal Routing/Demux', [system '/ActiveBalancing/Demux'], 'Position', [120 30 125 60*numberOfCellelements]);
set_param([system '/ActiveBalancing/Demux'], 'Outputs', num2str(numberOfCellelements));
add_line([system '/ActiveBalancing'], 'CellCurrents/1', 'Demux/1', 'autorouting', 'on');
for i = 1:numberOfCellelements
    add_block('simulink/Signal Routing/Goto', [system '/ActiveBalancing/Goto' num2str(i)], 'Position', [180 -30+i*60 300 i*60]);
    set_param([system '/ActiveBalancing/Goto' num2str(i)], 'GotoTag', ['ActBal' num2str(i)]);
    set_param([system '/ActiveBalancing/Goto' num2str(i)], 'TagVisibility', 'global');
    add_line([system '/ActiveBalancing'], ['Demux/' num2str(i)], ['Goto' num2str(i) '/1'], 'autorouting', 'on');
end


stateCount = dglCount + algCount;
add_block('MatrixModel/SystemStates', [system '/SystemStates'], 'Position', [780 60 840 120]);

add_block('simulink/Signal Routing/Mux', [system '/SystemStates/Mux'], 'Position', [-20 30 -15 min(32767, 60*stateCount)]);
set_param([system '/SystemStates/Mux'], 'BlockMirror', 'on');
set_param([system '/SystemStates/Mux'], 'Inputs', num2str(stateCount));
% add_line([system '/SystemStates'], 'States/1', 'Mux/1', 'autorouting', 'on');
x_offset = 0;
for i = 1:stateCount
    positionAdder = -30+i*60;
    if positionAdder >= 32767
        positionAdder = mod( -30+i*60, 32767) ;
        x_offset = 60 * floor(( ( -30+i*60) / 32767));
    end
    pos = [(180+x_offset) positionAdder (300 + x_offset) (positionAdder+15)];
    add_block('simulink/Signal Routing/From', [system '/SystemStates/From' num2str(i)], 'Position', pos);
    set_param([system '/SystemStates/From' num2str(i)], 'GotoTag', ['State' num2str(i)]);
    set_param([system '/SystemStates/From' num2str(i)], 'BlockMirror', 'on');
%     set_param([system '/SystemStates/Goto' num2str(i)], 'TagVisibility', 'global');
    add_line([system '/SystemStates'],  ['From' num2str(i) '/1'], ['Mux/' num2str(i)], 'autorouting', 'on');
end
add_block('simulink/Signal Routing/Goto', [ system '/SystemStates/StatesBus' ], 'Position', [-60 30 -30 60]);
set_param([system '/SystemStates/StatesBus'], 'GotoTag', 'StatesBus');
set_param([system '/SystemStates/StatesBus'], 'TagVisibility', 'global');

add_line( [ system '/SystemStates/' ], 'Mux/1' , 'StatesBus/1', 'autorouting', 'on');

add_block('MatrixModel/Matrix', [system '/PowerMatrix'], 'Position', [1155 180 1215 240]);
DeleteSeparator([system '/PowerMatrix']);

add_block('MatrixModel/CellPowerValues', [system '/CellPowerValues'], 'Position', [1305 180 1365 240]);
add_block('simulink/Signal Routing/Demux', [system '/CellPowerValues/Demux'], 'Position', [120 30 125 60*numberOfCellelements]);
set_param([system '/CellPowerValues/Demux'], 'Outputs', num2str(numberOfCellelements));
add_line([system '/CellPowerValues'], 'CellPower/1', 'Demux/1', 'autorouting', 'on');
add_line(system, 'PowerMatrix/1', 'OhmicHeatGenerationVector/1', 'autorouting', 'on');
x_offset = 0;
for i = 1:numberOfCellelements
    positionAdder = -30+i*60;
    if positionAdder >= 32767
        positionAdder = mod( -30+i*60, 32767) ;
        x_offset = 60 * floor(( ( -30+i*60) / 32767));
    end
    pos = [(180+x_offset) positionAdder (300 + x_offset) (positionAdder+15)];
    add_block('simulink/Signal Routing/Goto', [system '/CellPowerValues/Goto' num2str(i)], 'Position', pos);
    set_param([system '/CellPowerValues/Goto' num2str(i)], 'GotoTag', ['P_CellElement' num2str(i)]);
    set_param([system '/CellPowerValues/Goto' num2str(i)], 'TagVisibility', 'global');
    add_line([system '/CellPowerValues'], ['Demux/' num2str(i)], ['Goto' num2str(i) '/1'], 'autorouting', 'on');
end


add_block('MatrixModel/ActiveBalancingCellCurrents', [system '/CellCurrents'], 'Position', [1305 80 1365 140]);
add_block('simulink/Signal Routing/Demux', [system '/CellCurrents/Demux'], 'Position', [120 30 125 min(32767, 60*numberOfCellelements)]);
set_param([system '/CellCurrents/Demux'], 'Outputs', num2str(numberOfCellelements));
add_line([system '/CellCurrents'], 'CellCurrents/1', 'Demux/1', 'autorouting', 'on');


x_offset = 0;
for i = 1:numberOfCellelements
    positionAdder = -30+i*60;
    if positionAdder >= 32767
        positionAdder = mod( -30+i*60, 32767) ;
        x_offset = 60 * floor(( ( -30+i*60) / 32767));
    end
    pos = [(180+x_offset) positionAdder (300 + x_offset) (positionAdder+15)];
    add_block('simulink/Signal Routing/Goto', [system '/CellCurrents/Goto' num2str(i)], 'Position', pos);
    set_param([system '/CellCurrents/Goto' num2str(i)], 'GotoTag', ['I_CellElement' num2str(i)]);
    set_param([system '/CellCurrents/Goto' num2str(i)], 'TagVisibility', 'global');
    add_line([system '/CellCurrents'], ['Demux/' num2str(i)], ['Goto' num2str(i) '/1'], 'autorouting', 'on');
end
add_block('simulink/Signal Routing/Goto', [system '/ActiveBalancing/BalancerBus'], 'Position', [780 300 840 360]);
add_line([system '/ActiveBalancing/'], 'CellCurrents/1', 'BalancerBus/1', 'autorouting', 'on');
set_param([system '/ActiveBalancing/BalancerBus'], 'TagVisibility', 'global');
set_param([system '/ActiveBalancing/BalancerBus'], 'GotoTag', 'BalancerBus');

add_block('simulink/Signal Routing/Goto', [system '/InputCurrent'], 'Position', [120 395 240 425]);
set_param([system '/InputCurrent'], 'GotoTag', 'InputCurrent');
set_param([system '/InputCurrent'], 'TagVisibility', 'global');

add_line(system, 'PowerMatrix/1', 'CellPowerValues/1');
add_line(system, 'CellCurrentMatrices/1', 'CellCurrents/1', 'autorouting', 'on');
add_line(system, 'CellCurrentMatrices/1', 'CurrentVector/1', 'autorouting', 'on');

set_param([system '/SoC_Vector/Mux'], 'Inputs', num2str(numberOfCellelements));
set_param([system '/Temperature_Vector/Mux'], 'Inputs', num2str(numberOfCellelements));
for i = 1:numberOfCellelements
    add_block('simulink/Signal Routing/From', [system '/SoC_Vector/SoC_' num2str(i)], 'Position', [50, 50*i, 100, (50*i)+25], 'GotoTag', ['SoC_CellElement' num2str(i)]);
    add_line([system '/SoC_Vector'], ['SoC_' num2str(i) '/1'], ['Mux/' num2str(i)], 'autorouting', 'on');
    add_block('simulink/Signal Routing/From', [system '/Temperature_Vector/T_' num2str(i)], 'Position', [50, 50*i, 100, (50*i)+25], 'GotoTag', ['T_CellElement' num2str(i)]);
    add_line([system '/Temperature_Vector'], ['T_' num2str(i) '/1'], ['Mux/' num2str(i)], 'autorouting', 'on');
end

add_line(system, 'ExcitationCurrent/1', 'InputCurrent/1', 'autorouting', 'on');

add_block('simulink/Sources/Constant', [system '/NumCellElements'], 'Value', num2str(numberOfCellelements), 'Position', [1315 690 1355 730]);
add_line(system, 'NumCellElements/1', 'NumberOfBatteries/1');

end
