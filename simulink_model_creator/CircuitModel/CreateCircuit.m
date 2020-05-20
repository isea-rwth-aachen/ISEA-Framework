function[destination] = CreateCircuit(destination, isIU)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isIU
    load_system('ElectricalTwoPorts/Circuit');
    add_block('Circuit/Circuit', destination, 'Position', position);
else
    load_system('ElectricalTwoPorts/Circuit_UI');
    add_block('Circuit_UI/Circuit', destination, 'Position', position);
end

end
