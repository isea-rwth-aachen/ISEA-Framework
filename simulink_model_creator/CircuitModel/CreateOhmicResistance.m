function[] = CreateOhmicResistance(destination, isIU ...
                                 , objRowPoints, objColPoints, objMatrix)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isIU
    load_system('ElectricalTwoPorts/OhmicResistance');
    add_block('OhmicResistance/OhmicResistance', destination, 'Position', position);
    SetObject(destination, 'ResistanceValue', 'Product/2', objRowPoints, objColPoints, objMatrix);
else
    load_system('ElectricalTwoPorts/OhmicResistance_UI');
    add_block('OhmicResistance_UI/OhmicResistance', destination, 'Position', position);
    SetObject(destination, 'ResistanceValue', 'Divide/2', objRowPoints, objColPoints, objMatrix);
end

end
