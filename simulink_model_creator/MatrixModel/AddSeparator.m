function [] = AddSeparator(system)

allComponents = GetAllComponents(system);
numberOfComponents = size(allComponents, 1);

seperators = get_param([system '/SepVals'], 'Value');
seperators = seperators(1:(end-1));
seperators = [seperators ' ' num2str(numberOfComponents) ']'];
set_param([system '/SepVals'], 'Value', seperators);

end

