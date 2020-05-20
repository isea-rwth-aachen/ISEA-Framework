function[] = CreateDirichletConditions(destination, data)

baseDestination = destination;
destination = [destination '/DirichletConditions'];
[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

add_block('ThermalModel/DirichletConditions', destination, 'Position', position);

set_param([destination '/A_cooling'], 'Gain', mat2str(-data(:,2)) );
set_param([destination '/c_cooling'], 'Value', mat2str(-data(:,3)) );

SetCoolingSelector(destination, data);
SetCoolingAdder(data, baseDestination);

end
