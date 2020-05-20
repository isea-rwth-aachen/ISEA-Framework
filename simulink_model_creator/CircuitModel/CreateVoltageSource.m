function[] = CreateVoltageSource(destination, isIU ...
                               , objRowPoints, objColPoints, objMatrix)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isIU
    load_system('ElectricalTwoPorts/VoltageSource');
    add_block('VoltageSource/VoltageSource', destination, 'Position', position);
    SetObject(destination, 'VoltageValue', 'U/1', objRowPoints, objColPoints, objMatrix);
else
    throw MException('CreateVoltageSource:1', 'U to I block for voltage source is not possible');
end

end
