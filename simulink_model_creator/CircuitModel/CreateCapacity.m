function[] = CreateCapacity(destination, isIU ...
                          , derivativeTimeConstant ...
                          , objRowPoints, objColPoints, objMatrix)

[position, number] = GetPosition(destination);
destination = SetElemNoToDestination(destination, number);

if isIU
    load_system('ElectricalTwoPorts/Capacity');
    add_block('Capacity/Capacity', destination, 'Position', position);
    SetObject(destination, 'CapacityValue', 'Divide/2', objRowPoints, objColPoints, objMatrix);
else
    load_system('ElectricalTwoPorts/Capacity_UI');
    add_block('Capacity_UI/Capacity', destination, 'Position', position);
    SetObject(destination, 'CapacityValue', 'Product/2', objRowPoints, objColPoints, objMatrix);
    set_param([destination '/Derivative'], 'CoefficientInTFapproximation', num2str(derivativeTimeConstant));
end

end
