function[destination] = MatrixCreateCellelement(destination, capacityInASec, initialSoC)

global CellElementCounter;

CellElementCounter = CellElementCounter + 1;
[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

add_block('ElectricalElements/CellElement', destination, 'Position', position);
set_param([destination '/SoCFactor'], 'Gain', num2str(100.0 / capacityInASec));
set_param([destination '/SoC'], 'InitialCondition', initialSoC);

end
