function[] = MakeStateComponent(system, symbolicString, row, col, dglCount)
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
destination = [system '/StateComponent' num2str(componentCounter)];
add_block('StateComponent/Component', destination, 'Position', position);
set_param([destination '/RowConst'], 'Value', num2str(row));
set_param([destination '/ColConst'], 'Value', num2str(col));
set_param([destination '/State'], 'GotoTag', ['State' num2str(row+1) ]);

%% Set Reset if we are in the alg States, and set external reset for dgl states
if dglCount <= row
    set_param([destination '/FromResetStateValue'], 'GotoTag', ['FromResetStateValue' num2str(row-dglCount) ] );
    set_param([destination '/FromResetStateValue'], 'TagVisibility', 'global');
    add_line(destination, 'From/1', 'Integrator/2',  'autorouting', 'on');
    add_line(destination, 'FromResetStateValue/1', 'Integrator/3',  'autorouting', 'on');
else
    delete_block([destination '/FromResetStateValue']);
    set_param([destination '/Integrator'], 'ExternalReset', 'rising' );
    set_param([destination '/Integrator'], 'InitialConditionSource', 'internal' );
    set_param([destination '/Integrator'], 'InitialCondition', '0' );
    set_param([destination '/From'], 'GotoTag', 'Reset');
    add_line(destination, 'From/1', 'Integrator/2',  'autorouting', 'on');
end

component.(origsystemstr) = component.(origsystemstr) +1;
totalDepth = GetTotalDepth(symbolicString);
global posVector;
posVector = ones(1, totalDepth);

PlaceSymbol(destination, symbolicString, totalDepth, 'Integrator/1');

end
