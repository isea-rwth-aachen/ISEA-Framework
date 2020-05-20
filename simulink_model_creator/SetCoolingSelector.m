function[] = SetCoolingSelector(destination, data)

global numberOfFiniteVolumes;
set_param([destination '/Selector'], 'InputPortWidth', num2str(numberOfFiniteVolumes));
selectorString = cell(1);
selectorString{1} = mat2str(data(:, 1)' + 1);
set_param([destination '/Selector'], 'IndexParamArray', selectorString);

end
