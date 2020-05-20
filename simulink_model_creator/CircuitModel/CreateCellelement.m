function[destination] = CreateCellelement(destination, isIU, capacityInASec, initialSoC)

global CellElementCounter;
CellElementCounter = CellElementCounter + 1;
[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isIU
    load_system('ElectricalTwoPorts/CellElement');
    add_block('CellElement/CellElement', destination, 'Position', position);
    set_param([destination '/SoCFactor'], 'Gain', num2str(100.0 / capacityInASec));
    set_param([destination '/SoC'], 'InitialCondition', num2str(initialSoC));
else
    load_system('ElectricalTwoPorts/CellElement_UI');
    add_block('CellElement_UI/CellElement', destination, 'Position', position);
    set_param([destination '/SoCFactor'], 'Gain', num2str(100.0 / capacityInASec));
    set_param([destination '/SoC'], 'InitialCondition', num2str(initialSoC));
end

end
