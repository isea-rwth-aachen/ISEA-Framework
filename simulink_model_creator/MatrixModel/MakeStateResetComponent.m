function[] = MakeStateResetComponent(system, symbolicString, row, col, totalStates, totalcells)
global component;
if isempty( component )
    component = struct();
end

origsystemstr = system;
while 1
    tmp = strrep (origsystemstr, '/', '');
    if strcmp(tmp, origsystemstr)
        break;
    end
    origsystemstr = tmp;
end

if ~isfield( component, origsystemstr )
    component.(origsystemstr) = [];
    component.(origsystemstr) = 0;
end
componentCounter = component.(origsystemstr);

% componentCounter = size(GetAllComponents(system), 1);
position = [60 min(32767-60, 60+componentCounter*120) 120 min(32767,120+componentCounter*120)];
destination = [system '/StateResetComponent' num2str(componentCounter)];
add_block('StateResetComponent/Component', destination, 'Position', position);
add_line(system, 'StatesBus/1', ['StateResetComponent' num2str(componentCounter) '/1'],'autorouting','on');
add_line(system, 'BalancerBus/1', ['StateResetComponent' num2str(componentCounter) '/2'],'autorouting','on');
%% Einkommentieren wenn getriggert
%add_line(system, 'InputCurrent/1', ['StateResetComponent' num2str(componentCounter) '/trigger'],'autorouting','on');
%%
set_param([destination '/RowConst'], 'Value', num2str(row));
set_param([destination '/ColConst'], 'Value', num2str(col));
%% Set Reset if we are in the alg States

add_block('simulink/Signal Routing/Goto', [system '/FromResetStateValue' num2str(componentCounter)], 'Position', position + [-120 0 -120 0]);
set_param([system '/FromResetStateValue' num2str(componentCounter)], 'BlockMirror', 'on');
set_param([system '/FromResetStateValue' num2str(componentCounter)], 'TagVisibility', 'global');
set_param([system '/FromResetStateValue' num2str(componentCounter)], 'GotoTag', ['FromResetStateValue' num2str(componentCounter)] );
add_line(system, ['StateResetComponent' num2str(componentCounter) '/3'], ['FromResetStateValue' num2str(componentCounter) '/1'], 'autorouting','on');


%set_param([system '/FromResetStateValue' num2str(componentCounter)], 'GotoTag', ['FromResetStateValue' num2str(componentCounter)] );

pos = get_param([system '/StateResetComponent' num2str(componentCounter) '/StatesBusInput'], 'Position');
pos = pos + [60 0 60 0];
add_block('simulink/Signal Routing/Demux', [system '/StateResetComponent' num2str(componentCounter) '/Demux'] , 'Position', pos);
set_param([system '/StateResetComponent' num2str(componentCounter) '/Demux'],  'Outputs', num2str(totalStates) )
add_line([system '/StateResetComponent' num2str(componentCounter) ],  'StatesBusInput/1',  'Demux/1', 'autorouting','on' )
% add_line([system '/StateResetComponent' num2str(componentCounter) ],  'BalancerBusInput/1',  'Demux/1', 'autorouting','on' )
for i = 1: totalStates 
    elemet = [system '/StateResetComponent' num2str(componentCounter) '/State' num2str(i) ];
    add_block('simulink/Signal Routing/Goto',elemet , 'Position', position + [-100 0 -100 0]);
    set_param(elemet,  'GotoTag', ['State' num2str(i)] )
    set_param(elemet,  'TagVisibility', 'local' )
    add_line([system '/StateResetComponent' num2str(componentCounter) ],  ['Demux/' num2str(i)  ],  ['State' num2str(i) '/1'], 'autorouting','on' )
end


pos = get_param([system '/StateResetComponent' num2str(componentCounter) '/BalancerBusInput'], 'Position');
pos = pos + [60 0 60 0];
add_block('simulink/Signal Routing/Demux', [system '/StateResetComponent' num2str(componentCounter) '/Demux_balancer'] , 'Position', pos);
set_param([system '/StateResetComponent' num2str(componentCounter) '/Demux_balancer'],  'Outputs', num2str(totalcells) )
add_line([system '/StateResetComponent' num2str(componentCounter) ],  'BalancerBusInput/1',  'Demux_balancer/1', 'autorouting','on' )
% add_line([system '/StateResetComponent' num2str(componentCounter) ],  'BalancerBusInput/1',  'Demux/1', 'autorouting','on' )
for i = 1: totalcells
    elemet = [system '/StateResetComponent' num2str(componentCounter) '/ActBal' num2str(i) ];
    add_block('simulink/Signal Routing/Goto',elemet , 'Position', position + [-100 0 -100 0]);
    set_param(elemet,  'GotoTag', ['ActBal' num2str(i)] )
    set_param(elemet,  'TagVisibility', 'local' )
    add_line([system '/StateResetComponent' num2str(componentCounter) ],  ['Demux_balancer/' num2str(i)  ],  ['ActBal' num2str(i) '/1'], 'autorouting','on' )
end

component.(origsystemstr) = component.(origsystemstr) +1;
totalDepth = GetTotalDepth(symbolicString);
global posVector;
posVector = ones(1, totalDepth);

PlaceSymbol(destination, symbolicString, totalDepth, '3/1');

end
