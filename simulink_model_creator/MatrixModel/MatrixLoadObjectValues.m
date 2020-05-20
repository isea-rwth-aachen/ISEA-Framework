function [] = MatrixLoadObjectValues(data)

for i = 1:length(data)
    if isempty(data(i).destination)
        continue;
    end
    blockName = data(i).destination;
    if isscalar(data(i).values) % constant
        set_param(blockName, 'Value', mat2str(data(i).values(1,1)));
    elseif isempty(data(i).colPoints) % 1d lookup
        set_param(blockName, 'BP1', mat2str(data(i).rowPoints));
        set_param(blockName, 'Table', mat2str(data(i).values));
    else % 2d lookup
        set_param(blockName, 'BP1', mat2str(data(i).rowPoints));
        set_param(blockName, 'BP2', mat2str(data(i).colPoints));
        set_param(blockName, 'Table', mat2str(data(i).values));
    end
end

end