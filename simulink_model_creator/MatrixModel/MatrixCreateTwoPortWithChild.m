function[destination] = MatrixCreateTwoPortWithChild(destination, isWarburg)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isWarburg
    add_block('ElectricalElements/TwoPortWithChild_States', destination, 'Position', position);
else
    add_block('ElectricalElements/TwoPortWithChild', destination, 'Position', position);
end

end
