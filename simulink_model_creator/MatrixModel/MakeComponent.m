function[] = MakeComponent(system, symbolicString, row, col)
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
destination = [system '/Component' num2str(componentCounter)];
add_block('Component/Component', destination, 'Position', position);
set_param([destination '/RowConst'], 'Value', num2str(row));
set_param([destination '/ColConst'], 'Value', num2str(col));
component.(origsystemstr) = component.(origsystemstr) +1;
symbolicString = InsertApproximations(symbolicString);
totalDepth = GetTotalDepth(symbolicString);
global posVector;
posVector = ones(1, totalDepth);

PlaceSymbol(destination, symbolicString, totalDepth, 'Value/1');

end
