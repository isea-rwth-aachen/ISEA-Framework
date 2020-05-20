function [allBlocks] = GetAllComponents(system)
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

if ~isfield( component, origsystemstr ) || component.(origsystemstr) == 0
    allBlocks = {};
    return;
end
orderedBlocks = cell(1, component.(origsystemstr));
for i = 1: component.(origsystemstr)
    orderedBlocks(i) = {['Component' num2str(i-1)]};
end
allBlocks = char(orderedBlocks);
% 
% load_system(system);
% pathNameLength = size(system, 2);
% rawData = find_system(system, 'SearchDepth', 1);
% 
% allBlocks = cell(0);
% indices = [];
% for i = 1:size(rawData, 1)
%     if(strcmp(system,rawData{i}))
%         continue;
%     end;
% 
%     position = get_param(rawData{i}, 'Position');
%     x = mean([position(1) position(3)]);
%     if(60 < x && x < 120)
%         nameWithoutPath = rawData{i}( (pathNameLength+2) : end );
%         allBlocks = [allBlocks nameWithoutPath];
%         indices = [indices (position(2)+60)/120];
% %         indices = [indices counter];
% %         counter = counter +1;
%     end
% end
% 
% numberOfBlocks = size(allBlocks, 2);
% orderedBlocks = cell(1, numberOfBlocks);
% for i = 1:numberOfBlocks
%     orderedBlocks(indices(i)) = allBlocks(i);
% end
% 
% allBlocks = char(orderedBlocks);

end
