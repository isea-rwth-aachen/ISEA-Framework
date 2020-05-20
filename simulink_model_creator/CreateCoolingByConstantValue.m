function[] = CreateCoolingByConstantValue(destination, data, value)

baseDestination = destination;
destination = [destination '/CoolingByConstantValue'];
[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

add_block('ThermalModel/CoolingByConstantValue', destination, 'Position', position);

paramMat = value * data(:, 3);
paramString = mat2str(paramMat);
set_param([destination '/CoolingValues'], 'Value', paramString);

SetCoolingAdder(data, baseDestination);

end
