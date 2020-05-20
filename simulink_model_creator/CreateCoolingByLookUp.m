function[] = CreateCoolingByLookUp(destination, data, measurementPoints, dataPoints)

baseDestination = destination;
destination = [destination '/CoolingByLookUp'];
[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

add_block('ThermalModel/CoolingByLookUp', destination, 'Position', position);

set_param([destination '/Memory'], 'X0', mat2str(data(:, 4)));

conductanceMat = data(:, 3) ./ data(:, 7) .* data(:, 6);
conductanceString = mat2str(conductanceMat);
set_param([destination '/Conductance'], 'Gain', conductanceString);

set_param([destination '/LookUpTable'], 'BP1', mat2str(measurementPoints));
set_param([destination '/LookUpTable'], 'Table', mat2str(dataPoints));

set_param([destination '/Areas'], 'Gain', mat2str(data(:, 3)));

SetCoolingSelector(destination, data);
SetCoolingAdder(data, baseDestination);

end
